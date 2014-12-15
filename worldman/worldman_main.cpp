/*
World Manager Main
Date: Sept-20-2014

Desc: Program allows the creation and generation of entities (worlds) such as stars, planets,
and moons. Each entity has its own set of attributes and properties that will later affect
resource distribution, tile layout and other effects (ResMan, TileMan etc).

Useful Links
[1] Global Warming: http://my.net-link.net/~malexan/Climate-Model.htm
[2] Planet Temp Calc: http://www2.astro.indiana.edu/~gsimonel/temperature1.html
[3] Estimate Planet Temps: http://bartonpaullevenson.com/NewPlanetTemps.html
[4] Habitabale Range: http://www.planetarybiology.com/calculating_habitable_zone.html
[5] NASA fact sheet: http://nssdc.gsfc.nasa.gov/planetary/factsheet/
[6] Earth's Early Atmo: http://pubs.acs.org/subscribe/archive/ci/30/i12/html/12learn.html
[7] Earth's Early temps: http://mygeologypage.ucdavis.edu/cowen/historyoflife/earlyEarth.html
[8] Data for eqn water boiling point: http://www.engineeringtoolbox.com/boiling-point-water-d_926.html
[9] Early Earth temp: http://www.nap.edu/openbook.php?record_id=12161&page=19
[10] Stellar Class: http://en.wikipedia.org/wiki/Stellar_classification
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <random>
#include <iostream>
#include <unistd.h>
#include <windows.h>

#include "worldman.h"


// The world
Worldman starSys;

// Print a list from an array of strings
void printList (std::string list[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%i - %s\n", i, list[i].c_str());
    }
    printf("\n");
}

// Check if a selection option is between two values, min and max
size_t checkValue (size_t min, size_t max) {
    size_t value;

    while (true) {
        printf("Enter a value (%d-%d): ", min, max);
        std::cin >> value;
        if (std::cin && value >= min && value <= max) {
            std::cin.ignore();
            return value;
        }
        std::cin.clear();
        std::cin.ignore();
        printf("Invalid Input\n");
    }
}

// Provide a list of options to the user so they may narrow down what they want to generate
void listOptions() {

    int type, subtype, arraySize, entID = -1;
    std::string input;

    // All the strings used in the options below
    //std::string optionsType[6] = {"Asteroid/Comet", "Dwarf Planet/Moon", "Terrestrial Planet", "Gas Giant", "Star", "Compact Objects"};
    std::string optionsType[4] = {"Generate Star System", "Generate Stellar Object", "Load Info", "Manual Generation"};
    /*
    std::string optionsSub1[6] = {"Ice Comet", "Iron Asteroid", "Carboneous Asteroid", "???", "???", "???"};
    std::string optionsSub2[6] = {"Random Dwarf Planet", "Ice Dwarf Planet", "Barren Dwarf Planet", "???", "???", "???"};
    std::string optionsSub3[7] = {"Random Planet", "Terrestrial Planet", "Volcanic Planet", "Oceanic Planet", "Barren Planet", "Arctic Planet", "Desert Planet"};
    std::string optionsSub4[6] = {"Random Gas Giant", "Hydrogen Gas Giant", "Ice Giant", "Hot Jupiter", "Super Jupiter", "Gas Dwarf"};
    std::string optionsSub5[6] = {"Random Star", "Main Sequence Star", "Red Giant Star", "Red Supergiant Star", "Red Dwarf", "Brown Dwarf"};
    std::string optionsSub6[6] = {"White Dwarf", "Neutron Star", "Quark Star", "Black Hole", "SMBH", "???"};
    */

    // Walk user through creation process
    printf("\nCreating new space object. Select option:\n");
    arraySize = sizeof(optionsType) / sizeof(optionsType[0]);   // Byte size of array divided by bytes per entry
    printList(optionsType, arraySize);
    type = checkValue(0, arraySize - 1);


    // Subtype choosing
    printf("\nSelect sub-type:\n");
    switch (type) {
        case 0: // Generate Star System
            printf("\n***Generating a new Star System...***\n");
            starSys.createStarSys();
            break;
        case 1: // Generate Stellar Object
            printf("Generating a new Stellar Object...\n");
            break;
        case 2: // Load Info
            printf("Enter entity ID: ");
            subtype = checkValue(0, 999);
            break;
        case 3: // Manual Generation
            printf("Entering manual generation. Enter a star entity ID, or leave blank to enter star parameters.\n");
            // Ask for star params, save/write, move on to planet params if planets were generated
            break;
    }

    // For manual planets only?
    while (type == 3) {
        printf("Enter entity ID of an existing star (leave blank for a new star): ");
        std::getline(std::cin, input);

        // Generate random star entity on blank/no input
        if (input.empty()) {
            printf("Blank detected: Creating random star...\n");
            starSys.createStarSys();  // Random Star
            printf("Star created, creating planet...\n");
            entID = starSys.getLastID();
            break;
        }
        // All planets need a valid parent star. If non-blank input, check if input is a positive number
        else if (input.find_first_not_of("0123456789") == std::string::npos) {
            entID = atoi(input.c_str());    // Convert string to integer
            if (starSys.checkTypebyID(entID, 4))
                break;
            else
                printf("Entity is not a star.\n");
        }
        // If non-blank input but not a positive integer
        else {
            printf("Invalid input.\n");
        }
    }
    // Generation starts here


}

// Main program
int main(int argc, char *argv[]) {
    // Read the index file with all entities
    starSys.init();
    //ents.readIndex();

    // Check for parameters passed on program execution
    // Create a new set of worlds
    while (true) {
        if (argc == 1 || strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--create") == 0) {
            listOptions();
        }
        // Return information on a specified world
        else if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0) {
            printf("\n Getting info\n");
            return 1;
        }
        // Display the help menu if incorrect number of arguments or help command requested
        else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            fprintf(stderr, "Usage: %s [OPTION]\n\nOptions:\n", argv[0]);
            fprintf(stderr, "  -h, --help			Show this help message\n");
            fprintf(stderr, "  -c, --create NUMBER		Create a number of new objects of the same type\n");
            fprintf(stderr, "  -i, --info FILE		Show this help message\n");
            return 1;
        }
        // All other cases, most likely invalid arguments
        else {
            fprintf(stderr, "Error: Invalid argument. Use -h for help.\n");
            return 1;
        }
    }
    /*
    // write in a nice readable way
    Json::StyledWriter styledWriter;
    std::cout << styledWriter.write(fromScratch);

    // ---- parse from string ----

    // write in a compact way
    Json::FastWriter fastWriter;
    std::string jsonMessage = fastWriter.write(fromScratch);

    Json::Value parsedFromString;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
    if (parsingSuccessful)
    {
        std::cout << styledWriter.write(parsedFromString) << std::endl;
    }
    */

    // End of Program
    //system("pause");
    return 0;
}
