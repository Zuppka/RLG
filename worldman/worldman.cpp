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

#include "include\worldman.h"
#include "include\star.h"
#include "include\planet.h"
#include "include\moon.h"
#include <fileman.h>

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <random>

Fileman ent; // Main list of entities
Star s;
Planet p;
Moon m;

// Random number generator related properties
//std::random_device rd;
std::mt19937 mt(time(0));   // Bit more advnaced randomization

// Random Number Generator - Uniform
int Worldman::rng(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(mt);
}

// Random Number Generator - Lognormal
double Worldman::rngLog(double a, double b) {
    // Distribution of orbital distances for planets
    std::lognormal_distribution<double> dist(a, b);
    return dist(mt);
}


// Simple random name generator
std::string Worldman::randName() {
    char tmp;
    int len = rng(3, 10);
    std::string result = "";

    // Add one random letter at a time until length reached
    for (int i = 0; i < len ; i++) {
        if (i == 0)
            tmp = rng(65, 90);   // Start with random uppercase letter
        else
            tmp = rng(97, 122);   // Randomize new lowercase letter
        result = result + tmp;  // Add letter to result
    }

    return result;
}

// Print a list from an array of strings
void printList (std::string list[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%i - %s\n", i + 1, list[i].c_str());
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

// Short function to round doubles to desired precision
double deciTrunc (double value, int precision) {
    double roundValue, mult;
    if (precision == 1)
         mult = 10;
    else
        mult = pow(10, precision);
    // Multiply value by a factor of 10, round to nearest integer, then divide by same factor
    roundValue = round(value * mult) / mult;
    return roundValue;
}

// Search the index array for matching ID and return the type
size_t getTypeByID (int id) {
    ent.readIndex(); // Read index file

    // Special case for the primary star in a system
    if (id == -1) {
        return 1;
    }
    // Return if requested ID is out of index bounds
    else if (id < 0 || id > ent.sizeIndex) {
        printf("ERROR: ID is out of range.\n");
        return 0;
    }
    // Scan through all entries in the indix file for a matching ID
    for (int i = 0; i <= ent.sizeIndex; i++) {
        if (ent.index[i]["entityID"] == id)
            return ent.index[i]["type"].asInt();
    }
    printf("ERROR: ID was not found.\n");
    return 0;
}

// Save data to latest entry position
size_t saveEntityInfo (size_t type) {
    ent.readData(type); // Read existing data file
    int sd = ent.sizeData; // Get number of existing entities before appending

    // Assign values to the content of entry
    switch(type) {
        case 1:     // Stars
            s.entID = ent.sizeIndex;
            ent.data[sd]["entityID"] = s.entID;
            ent.data[sd]["parentID"] = s.parentID;
            ent.data[sd]["numSats"] = s.numSats;
            ent.data[sd]["name"] = s.name;
            ent.data[sd]["s_radius"] = deciTrunc(s.radius / SOL_RADIUS, 3);
            ent.data[sd]["s_mass"] = deciTrunc(s.mass / SOL_MASS, 3);
            ent.data[sd]["s_gravity"] = round(s.gravity / EARTH_GRAV);
            ent.data[sd]["s_temp"] = s.temp;
            ent.data[sd]["s_age"] = s.age;
            ent.data[sd]["s_luminosity"] = deciTrunc(s.luminosity / SOL_LUMINOSITY, 3);
            ent.data[sd]["s_habitable_min"] = deciTrunc(s.habitable_min / AU, 3);
            ent.data[sd]["s_habitable_max"] = deciTrunc(s.habitable_max / AU, 3);
            break;
        case 2:     // Planets
            p.entID = ent.sizeIndex;
            ent.data[sd]["entityID"] = p.entID;
            ent.data[sd]["parentID"] = p.parentID;
            ent.data[sd]["numSats"] = p.numSats;
            ent.data[sd]["name"] = p.name;
            ent.data[sd]["p_radius"] = deciTrunc(p.radius / EARTH_RADIUS, 3);
            ent.data[sd]["p_mass"] = deciTrunc(p.mass / EARTH_MASS, 3);
            ent.data[sd]["p_gravity"] = deciTrunc(p.gravity / EARTH_GRAV, 2);
            ent.data[sd]["p_temp"] = round(p.temp);
            ent.data[sd]["p_age"] = p.age;
            ent.data[sd]["p_dist"] = deciTrunc(p.dist / AU, 3);
            ent.data[sd]["p_period"] = deciTrunc(p.period, 1);
            ent.data[sd]["p_solarConst"] = round(p.solarConst);
            ent.data[sd]["p_pressure"] = deciTrunc(p.pressure / 101325, 3);
            ent.data[sd]["p_albedo"] = deciTrunc(p.albedo, 3);
            ent.data[sd]["p_emissivity"] = deciTrunc(p.emissivity, 3);
            ent.data[sd]["p_greenhouseEff"] = deciTrunc(p.greenhouseEff, 3);
            ent.data[sd]["p_coreComp"] = p.coreComp;
            ent.data[sd]["p_atmDens"] = p.atmDens;
            ent.data[sd]["p_atmComp"] = p.atmComp;
            break;
        case 3:     // Moons/Satellites
            m.entID = ent.sizeIndex;
            ent.data[sd]["entityID"] = m.entID;
            ent.data[sd]["parentID"] = m.parentID;
            //ent.data[sd]["numSats"] = m.numSats;
            ent.data[sd]["name"] = m.name;
            ent.data[sd]["m_radius"] = deciTrunc(m.radius / EARTH_RADIUS, 3);
            ent.data[sd]["m_mass"] = deciTrunc(m.mass / EARTH_MASS, 3);
            ent.data[sd]["m_gravity"] = deciTrunc(m.gravity / EARTH_GRAV, 3);
            ent.data[sd]["m_temp"] = round(m.temp);
            //ent.data[sd]["m_age"] = m.age;
            break;
        case 4:     // Minor Planets
             ent.data[sd]["entityID"] = ent.sizeIndex;
             break;
        case 5:     // Other objects
            ent.data[sd]["entityID"] = ent.sizeIndex;
            break;
        default:
            printf("ERROR: No valid data to load!\n");
            return -1;
    }
	return 0;
}

// Load data by entID
size_t loadEntityByID (int id) {
    Fileman loadEnt;
    int type = getTypeByID(id);

    // If type could not be obtained (id out of range or does not exist)
    if (!type)
        return 1;
    loadEnt.readData(type); // Read entity data

    // Do not load any entity information is parentID is -1. Special case for entities without parents.
    if (id < 0)
        return 0;

    // Scan through all entries of the particular data type until a matching ID is found
    for (int i = 0; i <= loadEnt.sizeData; i++) {
        if (loadEnt.data[i]["entityID"] == id) {
            // Load different data depending on entity type
            switch (type) {
                case 1:   // Stars
                    s.entID = loadEnt.data[i]["entityID"].asUInt();
                    s.parentID = loadEnt.data[i]["parentID"].asUInt();
                    s.numSats = loadEnt.data[i]["numSats"].asUInt();
                    s.name = loadEnt.data[i]["name"].asString();
                    s.radius = loadEnt.data[i]["s_radius"].asDouble() * SOL_RADIUS;
                    s.mass = loadEnt.data[i]["s_mass"].asDouble() * SOL_MASS;
                    s.gravity = loadEnt.data[i]["s_gravity"].asDouble() * EARTH_GRAV;
                    s.temp = loadEnt.data[i]["s_temp"].asDouble();
                    s.age = loadEnt.data[i]["s_age"].asDouble();
                    s.luminosity = loadEnt.data[i]["s_luminosity"].asDouble() * SOL_LUMINOSITY;
                    s.habitable_min = loadEnt.data[i]["s_habitable_min"].asDouble() * AU;
                    s.habitable_max = loadEnt.data[i]["s_habitable_max"].asDouble() * AU;
                    break;
                case 2:  // Planets
                    p.entID = loadEnt.data[i]["entityID"].asUInt();
                    p.parentID = loadEnt.data[i]["parentID"].asUInt();
                    p.numSats = loadEnt.data[i]["numSats"].asUInt();
                    p.name = loadEnt.data[i]["name"].asString();
                    p.radius = loadEnt.data[i]["p_radius"].asDouble() * EARTH_RADIUS;
                    p.mass = loadEnt.data[i]["p_mass"].asDouble() * EARTH_MASS;
                    p.gravity = loadEnt.data[i]["p_gravity"].asDouble() * EARTH_GRAV;
                    p.temp = loadEnt.data[i]["p_temp"].asDouble();
                    p.age = loadEnt.data[i]["p_age"].asDouble();
                    p.dist = loadEnt.data[i]["p_dist"].asDouble() * AU;
                    p.period = loadEnt.data[i]["p_period"].asDouble();
                    p.solarConst = loadEnt.data[i]["p_solarConst"].asDouble();
                    p.pressure = loadEnt.data[i]["p_pressure"].asDouble() * 101325;
                    p.albedo = loadEnt.data[i]["p_albedo"].asDouble();
                    p.emissivity = loadEnt.data[i]["p_emissivity"].asDouble();
                    p.greenhouseEff = loadEnt.data[i]["p_greenhouseEff"].asDouble();
                    p.coreComp = loadEnt.data[i]["p_coreComp"].asString();
                    p.atmDens = loadEnt.data[i]["p_atmDens"].asString();
                    p.atmComp = loadEnt.data[i]["p_atmComp"].asString();
                    break;
                case 3:    // Moons/Satellites
                    m.entID = loadEnt.data[i]["entityID"].asUInt();
                    m.parentID = loadEnt.data[i]["parentID"].asUInt();
                    //m.numSats = loadEnt.data[i]["numSats"].asUInt();
                    m.name = loadEnt.data[i]["name"].asString();
                    m.radius = loadEnt.data[i]["m_radius"].asDouble() * EARTH_RADIUS;
                    m.mass = loadEnt.data[i]["m_mass"].asDouble() * EARTH_MASS;
                    m.gravity = loadEnt.data[i]["m_gravity"].asDouble() * EARTH_GRAV;
                    m.temp = loadEnt.data[i]["m_temp"].asDouble();
                    //m.age = loadEnt.data[i]["m_age"].asDouble();
                    break;
                case 4:  // Minor planets
                    //m.entID = loadEnt.data[i]["entityID"].asUInt();
                    break;
                case 5:  // Other objects
                    //m.entID = loadEnt.data[i]["entityID"].asUInt();
                    break;
                default:
                    printf("ERROR: No valid data to load!\n");
                    return 1;
            }
            break;  // Exit for loop
        }
        else if (i == loadEnt.sizeData) {
            printf("ERROR: ID %d not found. It was likely not written.\n", id);
            return 1;
        }
    }

	return 0;
}

// Display information about an entity in a stylized ASCII tree
void printEntTree(int id, int type, int numSats) {
    // Variables for the satellite (child) entity
    std::string name2;
    int num = 0, id2, type2 = type + 1, numSats2;

    Fileman sat;
    sat.readData(type2); // Read the entire data file of the child entity

    // Search through entire data file
    for (int i = 0; i < sat.sizeData; i++) {
        if (sat.data[i]["parentID"] == id ) {
            num++;
            name2 = sat.data[i]["name"].asString();
            id2 = sat.data[i]["entityID"].asUInt();
            numSats2 = sat.data[i]["numSats"].asUInt();
            // Print out info retrieved from file
            if (type2 == 2)
                printf("  |___%s (%d)\n", name2.c_str(), id2);
            else if (type2 == 3)
                printf("        |___%s (%d)\n", name2.c_str(), id2);
            if (numSats2 > 0)
                printEntTree(id2, type2, numSats2);
        }
        // Exit if the number of satellites has been reached (assuming parent value and number in child data file is the same)
        if (num == numSats)
            break;
    }
}

// Print out final generated attributes and properties of entity
size_t printEntityInfo (int id) {
    if (loadEntityByID(id)) {
        printf("ERROR: ID did not return results.\n");
        return 1;
    }
    int type = getTypeByID(id);
    if (!type)
        return 1;

    if (type == 1) {   // Stars
        printf("\n%s (%d)\n", s.name.c_str(), id);
        printEntTree(id, type, s.numSats);
        printf("\n");
        printf("Name:             %s (%d)\n", s.name.c_str(), s.entID);
        printf("Solar Masses:     %.2f\n", s.mass / SOL_MASS);
        printf("Solar Radii:      %.2f\n", s.radius / SOL_RADIUS);
        printf("Solar Luminosity: %.2f\n", s.luminosity / SOL_LUMINOSITY);
        printf("Surface Gravity:  %.0f G\n", s.gravity / EARTH_GRAV);
        printf("Surface Temp:     %.0f K\n", s.temp);
        printf("Planets:          %d\n\n", s.numSats);
    }
    else if (type == 2) {   // Planets
        printf("\n%s (%d)\n", p.name.c_str(), id);
        printEntTree(id, type, p.numSats);
        printf("\n");
        printf("Name:             %s (%d)\n", p.name.c_str(), p.entID);
        printf("Earth Masses:     %.2f\n", p.mass / EARTH_MASS);
        printf("Radius:           %.0f Km\n", p.radius / 1000.0);
        printf("Surface Gravity:  %.2f G\n", p.gravity / EARTH_GRAV);
        printf("Semi-Major Axis:  %.3f AU\n", p.dist / AU);
        printf("Orbital Period:   %.1f Days\n", p.period);
        //printf("Rotational Period:   %.1f Days\n", p.period);
        printf("Core Compostion:  %s\n", p.coreComp.c_str());
        printf("Atmopshere:       %s %s\n", p.atmDens.c_str(), p.atmComp.c_str());
        printf("Surface Pressure: %.0f ATM\n", p.pressure / 101325);
        printf("Surface Temp:     %.0f K\n", p.temp);
        printf("Moons:            %d\n\n", p.numSats);
    }
    else if (type == 3) {    // Dwarf Planets
        printf("\n%s (%d)\n", m.name.c_str(), id);
        printf("\n");
        printf("Name:              %s (%d)\n", m.name.c_str(), m.entID);
        printf("Earth Masses:      %.2f\n", m.mass / EARTH_MASS);
        printf("Radius:            %.0f Km\n", m.radius / 1000.0);
        printf("Surface Gravity:   %.2f G\n", m.gravity / EARTH_GRAV);
        //printf("Semi-Major Axis:   %.3f AU\n", m.dist / AU);
        //printf("Orbital Period:    %.1f Days\n", p.period);
        //printf("Rotational Period: %.1f Days\n", p.period);
        //printf("Surface Pressure:  %.3f ATM\n", m.pressure);
        printf("Surface Temp:      %.0f K\n\n", m.temp);
    }
    else if (type == 4) {   // Minor planets
        printf("Description: A dwarf planet or possibly asteroid.\n");
    }
    else if (type == 5) {   // Other
        printf("Description: Some ball of matter.\n");
    }
    else {
        printf("ERROR: No valid data to display!\n");
        return 1;
	}

    return 0;
}

// Create a new entity entry
size_t createEntity (size_t type, size_t subtype, int parID) {

    printf("DEBUG: ParentID: %d Type: %d Subtype: %d\n", parID, type, subtype);
    // Load information of parent entity if it exists. Index and entity info is updated here
    if (loadEntityByID(parID)) {
        printf("ERROR: ParentID does not exist.\n");
        return 1;
    }

    // Load the type specific data file for storing
    switch (type) {
        case 1:
            s.create(subtype, s);
            break;
        case 2:
            p.create(subtype, s);
            break;
        case 3:
            m.create(subtype, p);
            break;
        case 4:
            //createMinor(subtype);
            break;
        case 5:
            //createMinor(subtype);
            break;
        default:
            printf("ERROR: Invalid Type selected!\n\n");
            return 1;
    }

    // ***Message about keeping data here? Perhaps verification compared to other entities (planets with same orbital distances)***
    // Write the new data
    saveEntityInfo(type);   // Copy entity properties to JSON data file in memory
    ent.writeData(type);   // Save entity properties from memory to disk
    printf("***%d Created***\n\n", type);

    return 0;
}

// Provide a list of options to the user so they may narrow down what they want to generate
void listOptions() {

    int option, arraySize, entID = -1;
    std::string input;
    std::string optionsType[4] = {"Generate Star System", "Generate Stellar Object", "Load Info", "Manual Generation"};

    // Walk user through creation process
    printf("\nCreating new space object. Select option:\n");
    arraySize = sizeof(optionsType) / sizeof(optionsType[0]);   // Byte size of array divided by bytes per entry
    printList(optionsType, arraySize);
    option = checkValue(0, arraySize - 1);

    // Selecting an option from the menu
    switch (option) {
        case 1: // Generate Star System
            printf("\n***Generating a new Star System...***\n");
            s.numStars = 1;
            // Create a star system including a star(s), orbiting planets, their moons and any other nearby celestial objects such as comets or asteroids
            for (s.counter = 1; s.counter <= s.numStars; s.counter++) {
                if (s.numStars <= 1)
                    createEntity(1, 1, -1);
                else    // Secondary stars are linked to primary star
                    createEntity(1, 1, s.entID);
                // Generate planets orbiting the star
                for (p.counter = 1; p.counter <= s.numSats;  p.counter++) {
                    createEntity(2, 0, s.entID);
                    // Generate moons orbiting the planets
                    for (m.counter = 1; m.counter <= p.numSats; m.counter++) {
                        createEntity(3, 0, p.entID);
                    }
                }
            }
            // Generate asteroid fields and other star system objects
            // createEntity(4, 0, p_parentID);
            break;
        case 2: // Generate Stellar Object
            printf("Generating a new Stellar Object...\n");
            break;
        case 3: // Load Info
            printf("Enter entity ID: ");
            entID = checkValue(0, 999);
            printEntityInfo(entID);
            break;
        case 4: // Manual Generation
            printf("Entering manual generation. Enter a star entity ID, or leave blank to enter star parameters.\n");
            // Ask for star params, save/write, move on to planet params if planets were generated
            break;
    }

    /*
        std::ofstream output;
        output.open ("output.txt", std::ofstream::app);
        output << "Description: The planet orbits a star of luminosity " << s_luminosity / SOL_LUMINOSITY << " L_sol and mass of " << s_mass / SOL_MASS << " M_sol. ";
        output << "The planet's semi-major axis is " << p_dist / AU << " AU and has a mass of " << p_mass / EARTH_MASS << " M_earth with a gravitational acceleration of " << p_gravity / EARTH_GRAV << " G. ";
        output << "It takes the planet " << p_period << " days to orbit its star. The solar constant at the plant is " << p_solarConst << " W / m^2. ";
        output << "The plant has a " << p_coreComp << "  core. The " << p_atmif (type == 2) {
        p_dist = data[entID]["p_dist"].asDouble();Dens << " atmosphere composed primarily of " << p_atmComp << " and the pressure is about " << p_pressure / 1e5 << " bar. ";
        output << "The average surface temperature of the planet is " << p_temp << " K (" << p_temp - 273 << " C).\n\n";
        output.close();
    */
    /*
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
    */

    /*
    // All the strings used in the options below
    //std::string optionsType[6] = {"Asteroid/Comet", "Dwarf Planet/Moon", "Terrestrial Planet", "Gas Giant", "Star", "Compact Objects"};

    std::string optionsSub1[6] = {"Ice Comet", "Iron Asteroid", "Carboneous Asteroid", "???", "???", "???"};
    std::string optionsSub2[6] = {"Random Dwarf Planet", "Ice Dwarf Planet", "Barren Dwarf Planet", "???", "???", "???"};
    std::string optionsSub3[7] = {"Random Planet", "Terrestrial Planet", "Volcanic Planet", "Oceanic Planet", "Barren Planet", "Arctic Planet", "Desert Planet"};
    std::string optionsSub4[6] = {"Random Gas Giant", "Hydrogen Gas Giant", "Ice Giant", "Hot Jupiter", "Super Jupiter", "Gas Dwarf"};
    std::string optionsSub5[6] = {"Random Star", "Main Sequence Star", "Red Giant Star", "Red Supergiant Star", "Red Dwarf", "Brown Dwarf"};
    std::string optionsSub6[6] = {"White Dwarf", "Neutron Star", "Quark Star", "Black Hole", "SMBH", "???"};
    */


}

// Main program
int main(int argc, char *argv[]) {
    // Create inital data files
    ent.init();

    // Check for parameters passed on program execution
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

    // End of Program
    return 0;
}
