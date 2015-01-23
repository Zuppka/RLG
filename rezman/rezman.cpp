/*
Resource Manager Main
Date: Jan-7-2015

Desc: Generates resources and manages resource attributes

*/

#include "include\rezman.h"
#include <fileman.h>
#include <toolman.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

Rezman rez;
const char *datapath = "resources";
std::string path = "", indexPath = "";

// Initialize necessary files and folders
void init() {

    Fileman file;
    file.makeDir(datapath);
    // Index file location
    indexPath += datapath;
    indexPath += "\\index.json";
    file.makeFile(indexPath);

}

// Iterative function that creates the path name using entity types
void pathName(int curr) {
    Fileman tmp;
    int next = curr + 1;

    path += rez.type[curr];
    // Check if the following subtype is blank
    if (rez.type[next].empty()) {
        path += ".json";
        tmp.makeFile(path); // Create resource file
    }
    // Append path, create sub folder and call self
    else {
        tmp.makeDir(path);
        path += "\\";
        pathName(next);
    }
}

// Copy resource data to JSON object and save both Index and Data file
bool saveRezInfo() {
    Fileman tmp;
    int si, sd;

    tmp.readFile(indexPath); // Read existing index file
    si = tmp.sizeData;
    tmp.data[si]["RezID"] = si;
    tmp.data[si]["Name"] = rez.name;
    tmp.data[si]["Path"] = path;

    tmp.writeFile(indexPath); // Read existing index file

    tmp.readFile(path); // Read existing data file
    sd = tmp.sizeData; // Get number of existing entities before appending

    tmp.data[sd]["RezID"] = si;
    tmp.data[sd]["Name"] = rez.name;
    tmp.data[sd]["Type1"] = rez.type[1];
    tmp.data[sd]["Type2"] = rez.type[2];
    tmp.data[sd]["Type3"] = rez.type[3];
    tmp.data[sd]["Attr0"] = rez.attr[0];
    tmp.data[sd]["Attr1"] = rez.attr[1];
    tmp.data[sd]["Attr2"] = rez.attr[2];

    tmp.writeFile(path);
    path = "";  // Reset the path
    printf("\"%s\" successfully created!\n", rez.name.c_str());
    return false;
}

// Load resource information from file by ID
bool loadRezInfo(int id) {
    Fileman tmp;

    tmp.readFile(indexPath); // Read existing index file
    if (id < 0 || id > tmp.sizeData) {
        printf("ID not found!\n");
        return true;
    }
    path = tmp.data[id]["Path"].asString();

    // Read the data file using path from index
    tmp.readFile(path);

    for (int i = 0; i <= tmp.sizeData; i++) {
        if (tmp.data[i]["RezID"].asInt() == id) {
            rez.rezID = tmp.data[i]["RezID"].asInt();
            rez.name = tmp.data[i]["Name"].asString();
            rez.type[1] = tmp.data[i]["Type1"].asString();
            rez.type[2] = tmp.data[i]["Type2"].asString();
            rez.type[3] = tmp.data[i]["Type3"].asString();

            rez.attr[0] = tmp.data[i]["Attr0"].asInt();
            rez.attr[1] = tmp.data[i]["Attr1"].asInt();
            rez.attr[2] = tmp.data[i]["Attr2"].asInt();
            path = "";  // Reset the path

            return false;
        }
    }
    return true;
}

// Print resource info in a nice, organized way
bool printRezInfo (int id) {
    if (loadRezInfo(id))
        return true;
    printf("\n");
    printf("|___%s\n", rez.type[1].c_str());
    printf("     |___%s\n", rez.type[2].c_str());
    // Print additional subtypes if resource uses them
    if (!rez.type[3].empty()) {
        printf("          |___%s\n", rez.type[3].c_str());
        printf("               |___%s\n\n", rez.name.c_str());
    }
    else
        printf("          |___%s\n\n", rez.name.c_str());
    // Print name and attribute values
    printf("Name:      %s (%d)\n", rez.name.c_str(), rez.rezID);
    printf("Attr1:     %d\n", rez.attr[0]);
    printf("Attr2:     %d\n", rez.attr[1]);
    printf("Attr3:     %d\n", rez.attr[2]);

    return false;
}

// Check  names in index and compare to string provided to function
bool chekRezName(std::string name) {
    Fileman tmp;
    int sd;
    std::string existName;

    tmp.readFile(indexPath); // Read existing index file
    sd = tmp.sizeData;

    // Convert the input string to lower case for comparison
    for (size_t j = 0; j < strlen(name.c_str()); j++) {
        name[j] = tolower(name[j]);
    }

    // Loop through all entries
    for (int i = 0; i < sd; i++) {
        existName = tmp.data[i]["Name"].asCString();
        // Convert the data string to lower case for comparison
        for (size_t j = 0; j < strlen(existName.c_str()); j++) {
            existName[j] = tolower(existName[j]);
        }
        if (existName == name)
            return true;
    }
    return false; // No match found

}

// Create resource entities
void generateResource() {
    int num, num2, num3, arraySize;
    // Reset resource types string array
    for(int i = 0; i <= 4; i++)
        rez.type[i].clear();
    // Create a random name and check that it does not already exist
    rez.name = randName();
    while (chekRezName(rez.name))
        rez.name = randName();

    // Type 0 - Datapath
    rez.type[0] = datapath;
    // Type 1 - Mineral, Chemical, Flora, Fauna
    arraySize = sizeof(rez.cat1) / sizeof(rez.cat1[0]);
    num = rng(0, arraySize - 1);
    rez.type[1] = rez.cat1[num];    // Assign type 1
    // Type 2
    if (rez.type[1] == "Mineral")
        num2 = rng(0, 9);
    else if (rez.type[1] == "Chemical")
        num2 = rng(0, 3);
    else if (rez.type[1] == "Flora")
        num2 = rng(0, 4);
    else if (rez.type[1] == "Fauna")
        num2 = rng(0, 3);
    rez.type[2] = rez.cat2[num][num2];  // Assign type 2
    // Type 3
    if (rez.type[2] == "Petrochemical" || rez.type[2] == "Compounds") {
        arraySize = sizeof(rez.cat3B[num2]) / sizeof(rez.cat3B[num2][num2]);
        num3 = rng(0, arraySize - 1);
        rez.type[3] = rez.cat3B[num2][num3];    // Assign type 3
    }
    else if (rez.type[2] == "Cereals" || rez.type[2] == "Fruit" || rez.type[2] == "Vegetables") {
        arraySize = sizeof(rez.cat3C[num2]) / sizeof(rez.cat3C[num2][num2]);
        num3 = rng(0, arraySize - 1);
        rez.type[3] = rez.cat3C[num2][num3];    // Assign type 3
    }
    // Assign attribute values
    rez.attr[0] = rng(0, 1000);
    rez.attr[1] = rng(0, 1000);
    rez.attr[2] = rng(0, 1000);
    // Assign a path name and save resource
    pathName(0);
    saveRezInfo();
}

void generateManual() {
    int num, num2, num3, arraySize;
    // Reset resource types string array
    for(int i = 0; i <= 4; i++)
        rez.type[i].clear();

    printf("\nEntering manual generation.\n");
    // Obtain a resource name
    printf("Enter resource name: ");
    getline(std::cin, rez.name);
    // Check if the name is empty or already in use
    while (chekRezName(rez.name) || rez.name.empty()) {
        printf("\nName already in use or blank name entered.\n");
        printf("Enter resource name: ");
        getline(std::cin, rez.name);
    }

    // Type 0 - Datapath
    rez.type[0] = datapath;
    // Type 1 - Mineral, Chemical, Flora, Fauna
    printf("\nSelect a type.\n");
    printList(rez.cat1, 4);

    arraySize = sizeof(rez.cat1) / sizeof(rez.cat1[0]);
    num = checkValue(0, arraySize - 1);
    rez.type[1] = rez.cat1[num];    // Assign type 1
    // Type 2
    if (rez.type[1] == "Mineral") {
        printf("\nSelect a subtype.\n");
        printList(rez.cat2[num], 10);
        num2 = checkValue(0, 9);
    }
    else {
        printf("\nSelect a subtype.\n");
        printList(rez.cat2[num], 4);
        num2 = checkValue(0, 3);
    }
    rez.type[2] = rez.cat2[num][num2];  // Assign type 2
    // Type 3
    if (rez.type[2] == "Petrochemical" || rez.type[2] == "Compounds") {
        printf("\nSelect a subtype.\n");
        printList(rez.cat3B[num2], arraySize - 1);
        arraySize = sizeof(rez.cat3B[num2]) / sizeof(rez.cat3B[num2][num2]);
        num3 = checkValue(0, arraySize - 1);
        rez.type[3] = rez.cat3B[num2][num3];    // Assign type 3
    }
    else if (rez.type[2] == "Cereals" || rez.type[2] == "Fruit" || rez.type[2] == "Vegetables") {
        printf("\nSelect a subtype.\n");
        printList(rez.cat3C[num2], arraySize - 1);
        arraySize = sizeof(rez.cat3C[num2]) / sizeof(rez.cat3C[num2][num2]);
        num3 = checkValue(0, arraySize - 1);
        rez.type[3] = rez.cat3C[num2][num3];    // Assign type 3
    }

    printf("\nAttribute1\n");
    rez.attr[0]  = checkValue(0, 1000);
    printf("\nAttribute2\n");
    rez.attr[1]  = checkValue(0, 1000);
    printf("\nAttribute3\n");
    rez.attr[2]  = checkValue(0, 1000);


    // Assign a path name and save resource
    pathName(0);
    saveRezInfo();
}

// Resource manager main
int main(int argc, char *argv[]) {

    int option = 0, arraySize, entID;
    std::string optionsType[4] = {"Generate Random", "Generate Manually", "Load Info", "Exit"};

    // Create initial data files
    init();

    // Check for parameters passed on program execution
    if (argc == 1) {
        while(option != 4) {
            // Walk user through creation process
            printf("\nWelcome to the Resource Manager. Select option:\n");
            arraySize = sizeof(optionsType) / sizeof(optionsType[0]);   // Byte size of array divided by bytes per entry
            printList(optionsType, arraySize);
            option = checkValue(0, arraySize - 1);

            // Provide a list of options to the user so they may narrow down what they want to generate
            switch (option) {
                case 0: // Generate a random resource
                    generateResource();
                    break;
                case 1: // Generate a resource based on user inputs
                    generateManual();
                    break;
                case 2: // Manual Generation
                    printf("Enter entity ID: ");
                    entID = checkValue(0, 999);
                    printRezInfo(entID);
                    break;
                case 3: // End program
                    return 0;
            }
        }
    }
    else if (strcmp(argv[1], "-g") == 0 || strcmp(argv[1], "--gen") == 0) {
        generateResource();
        return 0;
    }
    // Enter manual entity generation
    else if (argc == 1 || strcmp(argv[1], "-m") == 0 || strcmp(argv[1], "--man") == 0) {
        generateManual();
        return 0;
    }
    // Return information on a specified entity
    else if ((strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0) && atoi(argv[2]) >= 0) {
        printRezInfo(atoi(argv[2]));
        return 0;
    }
    // Display the help menu if incorrect number of arguments or help command requested
    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s [OPTION]\n\nOptions:\n", argv[0]);
        fprintf(stderr, "  -h, --help           Show this help message\n");
        fprintf(stderr, "  -g, --gen            Generate a new resource\n");
        fprintf(stderr, "  -m, --man            Manually generate a new resource\n");
        fprintf(stderr, "  -i ID, --info ID     Display information about a specific resource\n");
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
