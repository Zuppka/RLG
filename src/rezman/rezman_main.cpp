/*
Resource Manager Main
Date: Jan-7-2015

Desc: Generates resources and manages resource attributes

*/

#include <modules\rezman.h>
#include <modules\toolman.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

// Resource manager main
int main(int argc, char *argv[]) {

    int option = 0, arraySize, entID;
    std::string optionsType[4] = {"Generate Random", "Generate Manually", "Load Info", "Exit"};
    Rezman rez;

    // Create initial data files
    rez.init();

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
                    rez.generateResource();
                    break;
                case 1: // Generate a resource based on user inputs
                    rez.generateManual();
                    break;
                case 2: // Manual Generation
                    printf("Enter entity ID: ");
                    entID = checkValue(0, 999);
                    rez.printRezInfo(entID);
                    break;
                case 3: // End program
                    return 0;
            }
        }
    }
    else if (strcmp(argv[1], "-g") == 0 || strcmp(argv[1], "--gen") == 0) {
        rez.generateResource();
        return 0;
    }
    // Enter manual entity generation
    else if (argc == 1 || strcmp(argv[1], "-m") == 0 || strcmp(argv[1], "--man") == 0) {
        rez.generateManual();
        return 0;
    }
    // Return information on a specified entity
    else if ((strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0) && atoi(argv[2]) >= 0) {
        rez.printRezInfo(atoi(argv[2]));
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
