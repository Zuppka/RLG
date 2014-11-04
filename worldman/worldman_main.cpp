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
#include <string.h>
#include <random>
#include <iostream>
#include <unistd.h>
#include <windows.h>

#include "worldman.h"
#include "fileman.h"

// The world
Fileman world(0);
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
            return value;
        }
        printf("Invalid Input\n");
        std::cin.clear();
        std::cin.ignore();
    }
}

// Provide a list of options to the user so they may narrow down what they want to generate
void listOptions() {

    size_t type, subtype;
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
    printList(optionsType, sizeof(optionsType) / sizeof(optionsType[0]));

    type = checkValue(0, 5);

    printf("\n Select sub-type:\n");
    switch (type) {
    case 0: // Asteroid/Comet
        printList(optionsSub1, sizeof(optionsSub1) / sizeof(optionsSub1[0]));
        subtype = checkValue(0, 5);

        switch (subtype) {
        case 0:
            printf("Generating %s...\n", optionsSub1[0].c_str());
            break;
        case 1:
            printf("Generating %s...\n", optionsSub1[1].c_str());
            break;
        case 2:
            printf("Generating %s...\n", optionsSub1[2].c_str());
            break;
        case 3:
            printf("Generating %s...\n", optionsSub1[3].c_str());
            break;
        case 4:
            printf("Generating %s...\n", optionsSub1[4].c_str());
            break;
        case 5:
            printf("Generating %s...\n", optionsSub1[5].c_str());
            break;
        }
        break;
    case 1: // Dwarf Planet/Moon
        printList(optionsSub2, sizeof(optionsSub2) / sizeof(optionsSub2[0]));
        subtype = checkValue(0, 6);

        switch (subtype) {
        case 0:
            printf("Generating %s...\n", optionsSub2[0].c_str());
            break;
        case 1:
            printf("Generating %s...\n", optionsSub2[1].c_str());
            break;
        case 2:
            printf("Generating %s...\n", optionsSub2[2].c_str());
            break;
        case 3:
            printf("Generating %s...\n", optionsSub2[3].c_str());
            break;
        case 4:
            printf("Generating %s...\n", optionsSub2[4].c_str());
            break;
        case 5:
            printf("Generating %s...\n", optionsSub2[5].c_str());
            break;
        }
        break;
    case 2: // Terrestrial Planet
        printList(optionsSub3, sizeof(optionsSub3) / sizeof(optionsSub3[0]));
        subtype = checkValue(0, 5);

        switch (subtype) {
        case 0:
            printf("Generating %s...\n\n", optionsSub3[0].c_str());
            //while(true) {
                //Sleep(1000);
                world.readIndex();
                planet.createEntity(type, subtype);
            //}
            break;
        case 1:
            printf("Generating %s...\n", optionsSub3[1].c_str());
            break;
        case 2:
            printf("Generating %s...\n", optionsSub3[2].c_str());
            break;
        case 3:
            printf("Generating %s...\n", optionsSub3[3].c_str());
            break;
        case 4:
            printf("Generating %s...\n", optionsSub3[4].c_str());
            break;
        case 5:
            printf("Generating %s...\n", optionsSub3[5].c_str());
            break;
        }
        break;
    case 3: // Gas Giant
        printList(optionsSub4, sizeof(optionsSub4) / sizeof(optionsSub4[0]));
        subtype = checkValue(0, 5);

        switch (subtype) {
        case 0:
            printf("Generating %s...\n", optionsSub4[0].c_str());
            break;
        case 1:
            printf("Generating %s...\n", optionsSub4[1].c_str());
            break;
        case 2:
            printf("Generating %s...\n", optionsSub4[2].c_str());
            break;
        case 3:
            printf("Generating %s...\n", optionsSub4[3].c_str());
            break;
        case 4:
            printf("Generating %s...\n", optionsSub4[4].c_str());
            break;
        case 5:
            printf("Generating %s...\n", optionsSub4[5].c_str());
            break;
        }
        break;
    case 4: // Star
        printList(optionsSub5, sizeof(optionsSub5) / sizeof(optionsSub5[0]));
        subtype = checkValue(0, 5);

        switch (subtype) {
        case 0:
            printf("Generating %s...\n", optionsSub5[0].c_str());
            break;
        case 1:
            printf("Generating %s...\n", optionsSub5[1].c_str());
            break;
        case 2:
            printf("Generating %s...\n", optionsSub5[2].c_str());
            break;
        case 3:
            printf("Generating %s...\n", optionsSub5[3].c_str());
            break;
        case 4:
            printf("Generating %s...\n", optionsSub5[4].c_str());
            break;
        case 5:
            printf("Generating %s...\n", optionsSub5[5].c_str());
            break;
        }
        break;
    case 5: // Compact object
        printList(optionsSub6, sizeof(optionsSub6) / sizeof(optionsSub6[0]));
        subtype = checkValue(0, 5);

        switch (subtype) {
        case 0:
            printf("Generating %s...\n", optionsSub6[0].c_str());
            break;
        case 1:
            printf("Generating %s...\n", optionsSub6[1].c_str());
            break;
        case 2:
            printf("Generating %s...\n", optionsSub6[2].c_str());
            break;
        case 3:
            printf("Generating %s...\n", optionsSub6[3].c_str());
            break;
        case 4:
            printf("Generating %s...\n", optionsSub6[4].c_str());
            break;
        case 5:
            printf("Generating %s...\n", optionsSub6[5].c_str());
            break;
        }
        break;
    }

    if (!world.readIndex() && !world.readData(type)) {
        world.readData(type);   // Find the size of existing data to know where to write new data
        world.writeData(type);  // Write the new data
        //world.displayData(sizeData);    // Display latest entry

    }
}

// Main program
int main(int argc, char *argv[]) {
    // Read the index file with all entities
    if (world.init())
        return 0;

    // Check for parameters passed on program execution
    // Create a new set of worlds
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
