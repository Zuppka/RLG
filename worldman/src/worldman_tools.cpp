/*
World Manager Generator
Date: Sept-20-2014

Desc: The math behind the generation of all entity properties including planets and stars.
*/

#include "..\include\worldman.h"
#include "..\include\star.h"
#include "..\include\planet.h"
#include "..\include\moon.h"
#include <fileman.h>

#include <cstdio>
#include <ctime>
#include <random>

Fileman ent;    // Main list of entities
Star s;
Planet p;
Moon m;

// Shared varaibles
int numStars = 1;

// Random number generator related properties
//std::random_device rd;
std::mt19937 mt(time(0));   // Bit more advnaced randomization of the seed

// Dummy initialize for Fileman
void Worldman::init(){
    ent.init();
}

int Worldman::rng(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
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

// Small function to round doubles to desired precision
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

// Save data to latest entry position
size_t saveEntityInfo (size_t type) {
    ent.readData(type); // Read existing data file in order to obtain number of existing entities before appending
    int sd = ent.sizeData;

    switch(type) {
        case 1:     // Stars
            ent.data[sd]["entityID"] = ent.sizeIndex;
            ent.data[sd]["s_parentID"] = s.parentID;
            ent.data[sd]["s_numSats"] = s.numSats;
            ent.data[sd]["s_name"] = s.name;
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
            // Assign values to the content of entry
            ent.data[sd]["entityID"] = ent.sizeIndex;
            ent.data[sd]["p_parentID"] = p.parentID;
            ent.data[sd]["p_numSats"] = p.numSats;
            ent.data[sd]["p_name"] = p.name;
            ent.data[sd]["p_radius"] = deciTrunc(p.radius / EARTH_RADIUS, 3);
            ent.data[sd]["p_mass"] = deciTrunc(p.mass / EARTH_MASS, 3);
            ent.data[sd]["p_gravity"] = deciTrunc(p.gravity / EARTH_GRAV, 3);
            ent.data[sd]["p_temp"] = round(p.temp);
            ent.data[sd]["p_age"] = p.age;
            ent.data[sd]["p_dist"] = deciTrunc(p.dist / AU, 3);
            ent.data[sd]["p_period"] = deciTrunc(p.period, 1);
            ent.data[sd]["p_solarConst"] = round(p.solarConst);
            ent.data[sd]["p_pressure"] = deciTrunc(p.pressure, 3);
            ent.data[sd]["p_albedo"] = deciTrunc(p.albedo, 3);
            ent.data[sd]["p_emissivity"] = deciTrunc(p.emissivity, 3);
            ent.data[sd]["p_greenhouseEff"] = deciTrunc(p.greenhouseEff, 3);
            ent.data[sd]["p_coreComp"] = p.coreComp;
            ent.data[sd]["p_atmDens"] = p.atmDens;
            ent.data[sd]["p_atmComp"] = p.atmComp;
            break;
        case 3:     // Moons/Satellites
            ent.data[sd]["entityID"] = ent.sizeIndex;
            ent.data[sd]["m_parentID"] = m.parentID;
            //ent.data[sd]["m_numSats"] = m.numSats;
            ent.data[sd]["m_name"] = m.name;
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

// Search the index array for matching ID and return the type
size_t getTypeByID (int id) {
    ent.readIndex(); // Read index file before checking

    // Return if requested ID is out of bounds of index
    if (id == -1) { // Special case for primary stars
        //if (debugLvl)
            printf("DEBUG: Type is 1.\n");
        return 1;
    }
    else if (id < 0 || id > ent.sizeIndex) {
        printf("ERROR: ID is out of range.\n");
        return 0;
    }
    for (int i = 0; i <= ent.sizeIndex; i++) {
        if (ent.index[i]["entityID"] == id)
            return ent.index[i]["type"].asInt();
    }
    printf("ERROR: ID was not found.\n");
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
                    s.parentID = loadEnt.data[i]["s_parentID"].asUInt();
                    s.numSats = loadEnt.data[i]["s_numSats"].asUInt();
                    s.name = loadEnt.data[i]["s_name"].asString();
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
                    p.parentID = loadEnt.data[i]["p_parentID"].asUInt();
                    p.numSats = loadEnt.data[i]["p_numSats"].asUInt();
                    p.name = loadEnt.data[i]["p_name"].asString();
                    p.radius = loadEnt.data[i]["p_radius"].asDouble() * EARTH_RADIUS;
                    p.mass = loadEnt.data[i]["p_mass"].asDouble() * EARTH_MASS;
                    p.gravity = loadEnt.data[i]["p_gravity"].asDouble() * EARTH_GRAV;
                    p.temp = loadEnt.data[i]["p_temp"].asDouble();
                    p.age = loadEnt.data[i]["p_age"].asDouble();
                    p.dist = loadEnt.data[i]["p_dist"].asDouble() * AU;
                    p.period = loadEnt.data[i]["p_period"].asDouble();
                    p.solarConst = loadEnt.data[i]["p_solarConst"].asDouble();
                    p.pressure = loadEnt.data[i]["p_pressure"].asDouble();
                    p.albedo = loadEnt.data[i]["p_albedo"].asDouble();
                    p.emissivity = loadEnt.data[i]["p_emissivity"].asDouble();
                    p.greenhouseEff = loadEnt.data[i]["p_greenhouseEff"].asDouble();
                    p.coreComp = loadEnt.data[i]["p_coreComp"].asCString();
                    p.atmDens = loadEnt.data[i]["p_atmDens"].asCString();
                    p.atmComp = loadEnt.data[i]["p_atmComp"].asCString();
                    break;
                case 3:    // Moons/Satellites
                    m.entID = loadEnt.data[i]["entityID"].asUInt();
                    m.parentID = loadEnt.data[i]["m_parentID"].asUInt();
                    //m.numSats = loadEnt.data[i]["m_numSats"].asUInt();
                    m.name = loadEnt.data[i]["m_name"].asString();
                    m.radius = loadEnt.data[i]["m_radius"].asDouble() * EARTH_RADIUS;
                    m.mass = loadEnt.data[i]["m_mass"].asDouble() * EARTH_MASS;
                    m.gravity = loadEnt.data[i]["m_gravity"].asDouble() * EARTH_GRAV;
                    m.temp = loadEnt.data[i]["m_temp"].asDouble();
                    //m.age = loadEnt.data[i]["m_age"].asDouble();
                    break;
                case 4:  // Minor planets
                    // p_radius = loadEnt.data[i]["p_radius"] * 1000;
                    break;
                case 5:  // Other objects
                    // p_radius = loadEnt.data[i]["p_radius"] * 1000;
                    break;
                default:
                    printf("ERROR: No valid data to load!\n");
                    return 1;
            }
            break;  // Exit for loop
        }
        else if (i == loadEnt.sizeData) {
            printf("ERROR: ID not found. Likely it was not written.\n");
            return 1;
        }
    }

	return 0;
}

// Print out final generated attributes and properties of entity
size_t Worldman::printEntityInfo (int id) {
    loadEntityByID(id);
    int type = getTypeByID(id);
    if (!type)
        return 1;

    if (type == 1) {   // Stars
        printf("Description: The star \"%s\" has a luminosity of %.2f L_sol and mass of %.2f M_sol. ", s.name.c_str(), s.luminosity / SOL_LUMINOSITY, s.mass / SOL_MASS);
        printf("The star has a radius of %.2f R_sol and a surface temperature is %.0f.\n\n", s.radius / SOL_RADIUS, s.temp);
    }
    else if (type == 2) {   // Planets
        printf("Description: The planet \"%s\" orbits a star of luminosity %.2f L_sol and mass of %.2f M_sol. ", p.name.c_str(), s.luminosity / SOL_LUMINOSITY, s.mass / SOL_MASS);
        printf("The planet's semi-major axis is %.3f AU and has a mass of %.2f M_earth with a gravitational acceleration of %.2f G. ",  p.dist / AU, p.mass / EARTH_MASS, p.gravity / EARTH_GRAV);
        printf("It takes the planet %.0f days to orbit its star. The solar constant at the plant is %.0f W / m^2. ", p.period, p.solarConst);
        printf("The plant has a %s core. The %s atmosphere composed primarily of %s and the pressure is about %.3f bar. ", p.coreComp, p.atmDens, p.atmComp, p.pressure / 1e5);
        printf("The average surface temperature of the planet is %.0f K (%.0f C).\n\n", p.temp, p.temp - 273);
    }
    else if (type == 3) {    // Dwarf Planets
        printf("Description: A rocky looking moon \"%s\".\n", m.name.c_str());
    }
    else if (type == 4) {   // Minor planets
        printf("Description: A large, colourful gas giant.\n");
    }
    else if (type == 5) {   // Other
        printf("Description: Some ball of matter.\n");
    }
    else {
        printf("ERROR: No valid data to display!\n");
        return 1;
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

    return 0;
}

// Create a new entity entry
size_t createEntity (size_t type, size_t subtype, int parID) {
    //if (debugLvl)
        printf("DEBUG: ParentID: %d Type: %d Subtype: %d\n", parID, type, subtype);
    // Load information of parent entity if it exists. Index and entity info is updated here
    loadEntityByID(parID);

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
    //printEntityInfo(type);  // Output info about generated entity to console
    // Write the new data
    saveEntityInfo(type);   // Copy entity properties to JSON data file in memory
    ent.writeData(type);   // Save entity properties from memory to disk
    printf("***%d Created***\n\n", type);

    return 0;
}

// Create a star system including a star(s), orbiting planets, their moons and any other nearby celestial objects such as comets or asteroids
size_t Worldman::createStarSys () {
    p.counter = 1;
    // Generate the star(s)
    for (int i = 0; i < numStars; i++) {
        if (numStars <= 1)
            createEntity(1, 1, -1);
        else    // Secondary stars are linked to primary star
            createEntity(1, 1, s.parentID);
        // Generate the planets surrounding the star
        for (int j = 0; j < s.numSats; j++) {
            createEntity(2, 0, p.parentID);
            p.counter++;
            // Generate moons for the planets
            for (int k = 0; k < p.numSats; k++) {
                createEntity(3, 0, m.parentID);
            }
        }
    }
    // Generate asteroid fields and other star system objects
    // createEntity(4, 0, p_parentID);

    return 0;
}

