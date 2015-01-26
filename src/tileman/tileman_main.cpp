/*
Tile Manager Main
Date: Jan-7-2015

Desc: Creates and manages a coordinate system for entities with surfaces and various parameters associated
with each location (resource quantities, structures, physical characteristics etc).

*/

#include <modules\tileman.h>
#include <modules\toolman.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

// Tile Manager main
int main(int argc, char *argv[]) {

    int option = 0, arraySize, entID;
    std::string optionsType[4] = {"Generate Tiles", "Scan Tile", "Load Info", "Exit"};
    Tileman tile;
    // Create initial data files
    tile.init();

    // Check for parameters passed on program execution
    if (argc == 1) {
        while(option != 4) {
            // Walk user through creation process
            printf("\nWelcome to the Tile Manager. Select option:\n");
            arraySize = sizeof(optionsType) / sizeof(optionsType[0]);   // Byte size of array divided by bytes per entry
            printList(optionsType, arraySize);
            option = checkValue(0, arraySize - 1);

            // Provide a list of options to the user so they may narrow down what they want to generate
            switch (option) {
                case 0: // Generate tiles for an entity
                    printf("Enter entity ID: ");
                    entID = checkValue(0, 999);
                    tile.genTiles(entID);
                    break;
                case 1: // Populate generated tiles with resources
                    tile.popTiles();
                    break;
                case 2: // Print entity tile info
                    printf("Enter entity ID: ");
                    entID = checkValue(0, 999);
                    tile.printTileInfo(entID);
                    break;
                case 3: // End program
                    return 0;
            }
        }
    }
    else if ((strcmp(argv[1], "-g") == 0 || strcmp(argv[1], "--gen") == 0) && atoi(argv[2]) >= 0) {
        tile.genTiles(atoi(argv[2]));
        return 0;
    }
    // Enter manual entity generation
    else if (argc == 1 || strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--scan") == 0) {
        tile.popTiles();
        return 0;
    }
    // Return information on a specified entity
    else if ((strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0) && atoi(argv[2]) >= 0) {
        tile.printTileInfo(atoi(argv[2]));
        return 0;
    }
    // Display the help menu if incorrect number of arguments or help command requested
    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s [OPTION]\n\nOptions:\n", argv[0]);
        fprintf(stderr, "  -h, --help           Show this help message\n");
        fprintf(stderr, "  -g, --gen ID         Generate tiles for an entity\n");
        fprintf(stderr, "  -s, --scan ID        Scan for resources on a specific entity\n");
        fprintf(stderr, "  -i ID, --info ID     Display tile information about a specific entity\n");
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
