/*
Tile Manager Main
Date: Jan-7-2015

Desc: Creates and manages a map coordinate system for entities with surfaces and various parameters associated
with each location (resource quantities, structures, physical characteristics etc).

*/

#include <modules\mapman.h>
#include <modules\toolman.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

// Map Tile Manager main
int main(int argc, char *argv[]) {
    int option = 0, arraySize, entID;
    std::vector <std::string> optionsType = {"Generate Tiles", "Scan Tile", "Load Info", "Exit"};
    Mapman m;

    // Create initial data files
    m.init();

    // Check for parameters passed on program execution
    if (argc == 1) {
        while(option != 3) {
            // Walk user through creation process
            printf("\nWelcome to the Map Manager. Select option:\n");
            arraySize = optionsType.size();   // Byte size of array divided by bytes per entry
            printList(optionsType, arraySize);
            option = checkValue(0, arraySize - 1);

            // Provide a list of options to the user so they may narrow down what they want to generate
            switch (option) {
                case 0: // Generate a map file for an entity
                    printf("Enter entity ID: ");
                    entID = checkValue(0, 999);
                    m.genMap(entID);
                    break;
                case 1: // Populate generated map with tile data
                    printf("Enter entity ID: ");
                    entID = checkValue(0, 999);
                    m.genTile(entID);
                    break;
                case 2: // Print entity map info
                    printf("Enter entity ID: ");
                    entID = checkValue(0, 999);
                    m.printTileInfo(entID);
                    break;
                case 3: // End program
                    return 0;
            }
        }
    }
    else if ((strcmp(argv[1], "-g") == 0 || strcmp(argv[1], "--gen") == 0) && atoi(argv[2]) >= 0) {
        m.genMap(atoi(argv[2]));
        return 0;
    }
    // Generate tile resource contents based off of a seed
    else if ((strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--scan") == 0) && atoi(argv[2]) >= 0) {
        m.genTile(atoi(argv[2]));
        return 0;
    }
    // Return information on a specified entity
    else if ((strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0) && atoi(argv[2]) >= 0) {
        m.printTileInfo(atoi(argv[2]));
        return 0;
    }
    // Display the help menu if incorrect number of arguments or help command requested
    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s [OPTION]\n\nOptions:\n", argv[0]);
        fprintf(stderr, "  -h, --help           Show this help message\n");
        fprintf(stderr, "  -g, --gen ID         Generate a map file for an entity\n");
        fprintf(stderr, "  -s, --scan ID        Scan for resources on a specific entity\n");
        fprintf(stderr, "  -i ID, --info ID     Display map information about a specific entity\n");
        return 0;
    }
    // All other cases, most likely invalid arguments
    else {
        fprintf(stderr, "Error: Invalid argument. Use -h for help.\n");
        return 0;
    }

    // End of Program
    return 0;
}
