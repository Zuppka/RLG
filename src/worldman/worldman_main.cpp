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

#include <modules\worldman.h>
#include <modules\toolman.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

// Main program
int main(int argc, char *argv[]) {
    Worldman ent;
    int option = 0, arraySize, entID;
    std::string optionsType[5] = {"Generate Star System", "Generate Stellar Object", "Manual Generation", "Load Info", "Exit"};

    // Create initial data files
    ent.init();

    // Check for parameters passed on program execution
    if (argc == 1) {
        while(option != 5) {
            // Walk user through creation process
            printf("\nCreating new space object. Select option:\n");
            arraySize = sizeof(optionsType) / sizeof(optionsType[0]);   // Byte size of array divided by bytes per entry
            printList(optionsType, arraySize);
            option = checkValue(0, arraySize - 1);

            // Provide a list of options to the user so they may narrow down what they want to generate
            switch (option) {
                case 0: // Generate Star System
                    ent.generateStarSystem();
                    break;
                case 1: // Generate Stellar Object i.e black holes
                    ent.generateStellarObject();
                    break;
                case 2: // Manual Generation
                    ent.generateManual();
                    // Ask for star params, save/write, move on to planet params if planets were generated
                    break;
                case 3: // Load Info
                    printf("Enter entity ID: ");
                    entID = checkValue(0, 999);
                    ent.printEntityInfo(entID);
                    break;
                case 4: // End program
                    return 0;
            }
        }
    }
    else if (strcmp(argv[1], "-g") == 0 || strcmp(argv[1], "--gen") == 0) {
        ent.generateStarSystem();
        return 0;
    }
    // Enter manual entity generation
    else if (argc == 1 || strcmp(argv[1], "-m") == 0 || strcmp(argv[1], "--man") == 0) {
        ent.generateManual();
        return 0;
    }
    // Return information on a specified entity
    else if ((strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0) && atoi(argv[2])) {
        ent.printEntityInfo(atoi(argv[2]));
        return 0;
    }
    // Display the help menu if incorrect number of arguments or help command requested
    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s [OPTION]\n\nOptions:\n", argv[0]);
        fprintf(stderr, "  -h, --help           Show this help message\n");
        fprintf(stderr, "  -g, --gen            Generate a new star system\n");
        fprintf(stderr, "  -m, --man            Manually generate a new entity\n");
        fprintf(stderr, "  -i ID, --info ID     Display information about a specific entity\n");
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
