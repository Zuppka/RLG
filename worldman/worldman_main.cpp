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
Worldman planet(0);

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
    std::string optionsType[6] = {"Asteroid/Comet", "Dwarf Planet/Moon", "Terrestrial Planet", "Gas Giant", "Star", "Compact Objects"};
    std::string optionsSub1[6] = {"Ice Comet", "Iron Asteroid", "Carboneous Asteroid", "???", "???", "???"};
    std::string optionsSub2[6] = {"Random Dwarf Planet", "Ice Dwarf Planet", "Barren Dwarf Planet", "???", "???", "???"};
    std::string optionsSub3[7] = {"Random Planet", "Terrestrial Planet", "Volcanic Planet", "Oceanic Planet", "Barren Planet", "Arctic Planet", "Desert Planet"};
    std::string optionsSub4[6] = {"Random Gas Giant", "Hydrogen Gas Giant", "Ice Giant", "Hot Jupiter", "Super Jupiter", "Gas Dwarf"};
    std::string optionsSub5[6] = {"Random Star", "Main Sequence Star", "Red Giant Star", "Red Supergiant Star", "Red Dwarf", "Brown Dwarf"};
    std::string optionsSub6[6] = {"White Dwarf", "Neutron Star", "Quark Star", "Black Hole", "SMBH", "???"};

    // Walk user through creation process
    printf("\n Creating new space object. Select type:\n");
    arraySize = sizeof(optionsType) / sizeof(optionsType[0]);   // Byte size of array divided by bytes per entry
    printList(optionsType, arraySize);
    type = checkValue(0, arraySize - 1);
    printf("Generating %s...\n", optionsType[type].c_str());

    // Subtype choosing
    printf("\n Select sub-type:\n");
    switch (type) {
        case 0: // Asteroid/Comet
            arraySize = sizeof(optionsSub1) / sizeof(optionsSub1[0]);   // Byte size of array divided by bytes per entry
            printList(optionsSub1, arraySize);
            subtype = checkValue(0, arraySize - 1);
            printf("Generating %s...\n", optionsSub1[subtype].c_str());
            break;
        case 1: // Dwarf Planet/Moon
            arraySize = sizeof(optionsSub2) / sizeof(optionsSub2[0]);   // Byte size of array divided by bytes per entry
            printList(optionsSub2, arraySize);
            subtype = checkValue(0, arraySize - 1);
            printf("Generating %s...\n", optionsSub2[subtype].c_str());
            break;
        case 2: // Terrestrial Planet
            arraySize = sizeof(optionsSub3) / sizeof(optionsSub3[0]);   // Byte size of array divided by bytes per entry
            printList(optionsSub3, arraySize);
            subtype = checkValue(0, arraySize - 1);
            printf("Generating %s...\n\n", optionsSub3[subtype].c_str());
            break;
        case 3: // Gas Giant
            arraySize = sizeof(optionsSub4) / sizeof(optionsSub4[0]);   // Byte size of array divided by bytes per entry
            printList(optionsSub4, arraySize);
            subtype = checkValue(0, arraySize - 1);
            printf("Generating %s...\n", optionsSub4[subtype].c_str());
            break;
        case 4: // Star
            arraySize = sizeof(optionsSub5) / sizeof(optionsSub5[0]);   // Byte size of array divided by bytes per entry
            printList(optionsSub5, arraySize);
            subtype = checkValue(0, arraySize - 1);
            printf("Generating %s...\n", optionsSub5[subtype].c_str());
            break;
        case 5: // Compact object
            arraySize = sizeof(optionsSub6) / sizeof(optionsSub6[0]);   // Byte size of array divided by bytes per entry
            printList(optionsSub6, arraySize);
            subtype = checkValue(0, arraySize - 1);
            printf("Generating %s...\n", optionsSub6[subtype].c_str());
            break;
    }

    // For planets only?
    while (type == 2) {
        printf("Enter entity ID of an existing star (leave blank for a new star): ");
        std::getline(std::cin, input);

        // Generate random star entity on blank/no input
        if (input.empty()) {
            printf("Blank detected: Creating random star...\n");
            planet.createEntity(4, 0, -1);  // Random Star
            printf("Star created, creating planet...\n");
            entID = planet.getLastID();
            break;
        }
        // All planets need a valid parent star. If non-blank input, check if input is a positive number
        else if (input.find_first_not_of("0123456789") == std::string::npos) {
            entID = atoi(input.c_str());    // Convert string to integer
            if (planet.checkTypebyID(entID, 4))
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
    planet.createEntity(type, subtype, entID);

}

// Main program
int main(int argc, char *argv[]) {
    // Read the index file with all entities
    planet.init();
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
