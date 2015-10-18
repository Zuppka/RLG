/*
Network Manager Main
Date: Jun-18-2015

Desc: Creates and manages socket connections for the client to connect to

*/

#include "modules\netman.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>

// platform independent wait for n seconds
#if PLATFORM == PLATFORM_WINDOWS
	void Wait(float seconds) {
		Sleep((int) (seconds * 1000.0f));
	}
#else
	#include <unistd.h>
	void wait(float seconds) {
	    usleep((int) (seconds * 1000000.0f ));
    }
#endif

// Open socket on specific port
bool Socket::Open(unsigned short port) {
    // Check if the socket is already open and port in use
    assert(!IsOpen());
    // Create socket
    socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket <= 0) {
        printf("Failed to create socket!\n");
        return false;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    // Bind socket to port
    if (bind(socket, (const sockaddr*) &address, sizeof(address)) < 0) {
        printf("Failed to bind socket!\n");
        Close();
        return false;
    }

    // Set socket to non-blocking mode
    #if PLATFORM == PLATFORM_OSX || PLATFORM == PLATFORM_LINUX
        int nonBlocking = 1;
        if (fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1 ) {
            printf( "Failed to set non-blocking socket!\n" );
            close();
            return false;
        }
    #elif PLATFORM == PLATFORM_WINDOWS
        DWORD nonBlocking = 1;
        if (ioctlsocket(socket, FIONBIO, &nonBlocking) != 0) {
            printf( "Failed to set non-blocking socket!\n" );
            Close();
            return false;
        }
    #endif

    return true;
}

// Close socket
void Socket::Close() {
    if (socket != 0 ) {
        #if PLATFORM == PLATFORM_OSX || PLATFORM == PLATFORM_LINUX
        close(socket);
        #elif PLATFORM == PLATFORM_WINDOWS
        closesocket(socket);
        #endif
        socket = 0;
    }
}

// Check if socket is open
bool Socket::IsOpen() const {
    return socket != 0;
}

// Send packets over UDP
bool Socket::Send(const Address &destination, const void *data, int size) {
    // Check if packet data exists, packet size is greater than zero and that the socket was created
    assert(data);
    assert(size > 0);
    if (socket == 0)
        return false;
    // Check if the address and port were provided
    assert(destination.GetAddress() != 0);
    assert(destination.GetPort() != 0);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(destination.GetAddress());
    address.sin_port = htons((unsigned short) destination.GetPort());
    // Send bytes
    int sentBytes = sendto(socket, (const char*)data, size, 0, (sockaddr*)&address, sizeof(sockaddr_in));

    return sentBytes == size;
}

// Receive packets over UDP
int Socket::Receive(Address &sender, void *data, int size) {
    // Check if packet data exists, packet size is greater than zero and that the socket was created
    assert(data);
    assert(size > 0);
    if (socket == 0)
        return false;

    #if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
    #endif

    sockaddr_in from;
    socklen_t fromLength = sizeof(from);
    // Receive bytes
    int receivedBytes = recvfrom(socket, (char*)data, size, 0, (sockaddr*)&from, &fromLength);

    // Save the sender's address and port
    unsigned int address = ntohl(from.sin_addr.s_addr);
    unsigned short port = ntohs(from.sin_port);
    sender = Address(address, port);

    return receivedBytes;
}

// Send modified UDP packets with headers
bool Connection::SendPacket(const unsigned char data[], int size) {
    // Connection is started and address is valid
    assert(running);
    if (address.GetAddress() == 0)
        return false;
    // Create the header for each packet
    unsigned char packet[size + 4];
    packet[0] = (unsigned char) (protoID >> 24);
    packet[1] = (unsigned char) ((protoID >> 16) & 0xFF);
    packet[2] = (unsigned char) ((protoID >> 8 ) & 0xFF);
    packet[3] = (unsigned char) ((protoID) & 0xFF);
    memcpy(&packet[4], data, size);
    return socket.Send(address, packet, size + 4);
}

// Receive modified UDP packets with headers
int Connection::ReceivePacket(unsigned char data[], int size) {
    // Connection is started
    assert(running);

    unsigned char packet[size + 4];
    Address sender;
    int bytesRead = socket.Receive(sender, packet, size + 4);
    if (bytesRead == 0)
        return 0;
    if (bytesRead <= 4) //
        return 0;
    if (packet[0] != (unsigned char) (protoID >> 24) ||
        packet[1] != (unsigned char) ((protoID >> 16) & 0xFF) ||
        packet[2] != (unsigned char) ((protoID >> 8) & 0xFF) ||
        packet[3] != (unsigned char) (protoID & 0xFF))
        return 0;
    if (mode == Server) {
        printf("Server accepted connection from client %d.%d.%d.%d:%d\n", sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), sender.GetPort());
        state = Connected;
        address = sender;
    }
    if (sender == address) {
        if (mode == Client && state == Connecting) {
            printf("Client completes connection with server!\n");
            state = Connected;
        }
        timeoutAccumulator = 0.0f;
        memcpy(data, &packet[4], size - 4);
        return size - 4;
    }
    return 0;
}

void Connection::Update(float deltaTime) {
    assert(running);
    timeoutAccumulator += deltaTime;
    // Check if connection timed out
    if (timeoutAccumulator > timeout) {
        if (state == Connecting) {
            printf("Timed out trying to connect!\n");
            ClearData();
            state = ConnectFail;
            OnDisconnect();
        }
        else if (state == Connected) {
            printf("Connection timed out!\n" );
            ClearData();
            if (state == Connecting)
                state = ConnectFail;
            OnDisconnect();
        }
    }
}

// Network  manager main
int main(int argc, char *argv[]) {
    // Parse command line parameters
	enum Mode {
		Client,
		Server
	};
	Mode mode = Server;
	Address address;
	Address sender;
	Socket socket;

	const int serverPort = 4001;
    const int clientPort = 4002;
    const int protoID = 0x11112222;
	const float deltaTime = 1.0f;
	const float timeout = 5.0f;
    const int packetSize = 256;
    std::string input;
	unsigned char packet[packetSize];
	unsigned char buffer[packetSize];


    // Read IP address from command line
	if (argc >= 2) {
		int a, b, c, d;
		if (sscanf(argv[1], "%d.%d.%d.%d", &a, &b, &c, &d)) {
			mode = Client;
			address = Address(a, b, c, d, serverPort);
		}
		else {
            printf("Invalid parameter format!\n");
            return 1;
		}
	}

	// Initialize sockets
	if (!InitializeSockets()) {
		printf("Failed to initialize sockets\n");
		return 1;
	}

	ReliableConnection connection(protoID, timeout);

	// If client
    if (mode == Client) {
        if (!connection.Start(clientPort)) {
            printf("Error: Could not start connection on port %d\n", clientPort);
            return 1;
        }
        connection.Connect(address);
    }
    // If server
    if (mode == Server) {
        if (!connection.Start(serverPort)) {
            printf("Error: Could not start connection on port %d\n", serverPort);
            return 1;
        }
        connection.Listen();
    }

    // Main loop
	while (true) {
        buffer[0] = 0;

        if (mode == Client) {
            if (connection.IsConnected()) {
                printf("client connected to server\n");
            }
            if (connection.ConnectFailed()) {
                printf("connection failed\n");
                break;
            }

            printf("Message: ");
            getline(std::cin, input);
            strcpy((char*)packet, input.c_str());
            connection.SendPacket(packet, sizeof(packet));
            printf("SENDTO IP: %d Port: %d size: %d\n", address.GetAddress(), address.GetPort(), sizeof(packet));
            printf("Packet: %s\n", packet);

            while (true) {
                printf("Receiving packet...\n");
                if (connection.ReceivePacket(buffer, sizeof(buffer)) == 0) {
                     break;
                }
                printf("[Server]: %s\n", buffer);
            }
        }
        else if (mode == Server) {
            if (connection.IsConnected()) {
                printf("Message: ");
                getline(std::cin, input);
                strcpy((char*)packet, input.c_str());
                connection.SendPacket(packet, sizeof(packet));
            }
            while (true) {
                // Read bytes
                printf("Receiving packet...\n");
                if (connection.ReceivePacket(buffer, sizeof(buffer)) == 0) {
                     break;
                }
                printf("[Client]: %s\n", buffer);
            }

        }
        else if (connection.ConnectFailed())
            return 1;
        else
            printf("Connecting...\n");

        printf("\n");

		connection.Update(deltaTime);
		Wait(deltaTime);
	}
	ShutdownSockets();

    // End of Program
    return 0;
}
