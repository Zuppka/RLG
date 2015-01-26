/*
World Manager Functions
Date: Sept-20-2014
*/

#include <modules\worldman.h>
#include <modules\toolman.h>
#include <modules\fileman.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

Star s;
Planet p;
Moon m;

const char *filepath[6];

// Initialize necessary files and folders
void Worldman::init() {
    const char *datapath = "database";
    const char *filename[6] = {"00_index.json", "01_stars.json", "02_planets.json", "03_moons.json", "04_minorPlanets.json", "05_other.json"};
    int len = strlen(datapath) + 2, arraySize = sizeof(filename) / sizeof(filename[0]);
    char *tmpPath;
    Fileman file;

    file.makeDir(datapath);

    // Create database files
    for (int i = 0; i < arraySize; i++) {
        // Setup the name of the file path
        tmpPath = new char[len + strlen(filename[i])];
        strcpy(tmpPath, datapath);
        strcat(tmpPath, "\\");
        strcat(tmpPath, filename[i]);
        filepath[i] = tmpPath;

        file.makeFile(filepath[i]);
    }
}

// Search the index array for matching ID and return the type
int Worldman::getTypeByID (int id) {
    Fileman tmp;
    tmp.readFile(filepath[0]); // Read index file

    // Special case for the primary star in a system
    if (id == -1) {
        return 1;
    }
    // Return if requested ID is out of index bounds
    else if (id < 0 || id > tmp.sizeData) {
        printf("ERROR: ID is out of range.\n");
        return 0;
    }
    // Scan through all entries in the index file for a matching ID
    for (int i = 0; i <= tmp.sizeData; i++) {
        if (tmp.data[i]["entityID"] == id)
            return tmp.data[i]["type"].asInt();
    }
    printf("ERROR: ID was not found.\n");
    return 0;
}

// Save data to latest entry position
bool Worldman::saveEntity (int type) {
    Fileman tmp;

    int sd, si;
    // Read existing index file and get size (last entry position)
    tmp.readFile(filepath[0]);
    si = tmp.sizeData;
    // Add entity id and entity type to the index database
    tmp.data[si]["entityID"] = si;
    tmp.data[si]["type"] = type;
    tmp.writeFile(filepath[0]);

    // Read existing data file and get number of existing entities before appending
    tmp.readFile(filepath[type]);
    sd = tmp.sizeData;
    // Assign values to the content of entry
    switch(type) {
        case 1:     // Stars
            s.entID = si;
            tmp.data[sd]["entityID"] = s.entID;
            if (s.counter == 1)
                tmp.data[sd]["parentID"] = si;
            else
                tmp.data[sd]["parentID"] = s.parentID;
            tmp.data[sd]["numSats"] = s.numSats;
            tmp.data[sd]["name"] = s.name;
            tmp.data[sd]["s_radius"] = deciTrunc(s.radius / SOL_RADIUS, 3);
            tmp.data[sd]["s_mass"] = deciTrunc(s.mass / SOL_MASS, 3);
            tmp.data[sd]["s_gravity"] = round(s.gravity / EARTH_GRAV);
            tmp.data[sd]["s_temp"] = s.temp;
            tmp.data[sd]["s_age"] = s.age;
            tmp.data[sd]["s_luminosity"] = deciTrunc(s.luminosity / SOL_LUMINOSITY, 3);
            tmp.data[sd]["s_habitable_min"] = deciTrunc(s.habitable_min / AU, 3);
            tmp.data[sd]["s_habitable_max"] = deciTrunc(s.habitable_max / AU, 3);
            break;
        case 2:     // Planets
            p.entID = si;
            tmp.data[sd]["entityID"] = p.entID;
            tmp.data[sd]["parentID"] = p.parentID;
            tmp.data[sd]["numSats"] = p.numSats;
            tmp.data[sd]["name"] = p.name;
            tmp.data[sd]["p_radius"] = deciTrunc(p.radius / EARTH_RADIUS, 3);
            tmp.data[sd]["p_mass"] = deciTrunc(p.mass / EARTH_MASS, 3);
            tmp.data[sd]["p_gravity"] = deciTrunc(p.gravity / EARTH_GRAV, 2);
            tmp.data[sd]["p_temp"] = round(p.temp);
            tmp.data[sd]["p_age"] = p.age;
            tmp.data[sd]["p_dist"] = deciTrunc(p.dist / AU, 3);
            tmp.data[sd]["p_period"] = deciTrunc(p.period, 1);
            tmp.data[sd]["p_solarConst"] = round(p.solarConst);
            tmp.data[sd]["p_pressure"] = deciTrunc(p.pressure / 101325, 3);
            tmp.data[sd]["p_albedo"] = deciTrunc(p.albedo, 3);
            tmp.data[sd]["p_emissivity"] = deciTrunc(p.emissivity, 3);
            tmp.data[sd]["p_greenhouseEff"] = deciTrunc(p.greenhouseEff, 3);
            tmp.data[sd]["p_coreComp"] = p.coreComp;
            tmp.data[sd]["p_atmDens"] = p.atmDens;
            tmp.data[sd]["p_atmComp"] = p.atmComp;
            break;
        case 3:     // Moons/Satellites
            m.entID = si;
            tmp.data[sd]["entityID"] = m.entID;
            tmp.data[sd]["parentID"] = m.parentID;
            //tmp.data[sd]["numSats"] = m.numSats;
            tmp.data[sd]["name"] = m.name;
            tmp.data[sd]["m_radius"] = deciTrunc(m.radius / EARTH_RADIUS, 3);
            tmp.data[sd]["m_mass"] = deciTrunc(m.mass / EARTH_MASS, 3);
            tmp.data[sd]["m_gravity"] = deciTrunc(m.gravity / EARTH_GRAV, 3);
            tmp.data[sd]["m_temp"] = round(m.temp);
            //tmp.data[sd]["m_age"] = m.age;
            break;
        case 4:     // Minor Planets
             tmp.data[sd]["entityID"] = si;
             break;
        case 5:     // Other objects
            tmp.data[sd]["entityID"] = si;
            break;
        default:
            printf("ERROR: No valid data to save!\n");
            return true;
    }
    tmp.writeFile(filepath[type]);  // Save entity properties from memory to disk if type was valid
	return false;
}

// Load data by entID
bool Worldman::loadEntity (int id, int type) {
    Fileman tmp;

    // Special case for stars with no parents
    if (id == -1)
        return 0;
     // Special case for child stars (binary systems etc)
    else if (type == 0)
         type++;
    // Check if the type is valid (between 1-5).
    else if (type < 0 || type > 5) {
        printf("ERROR: Invalid DATA type.\n");
        return true;
    }
    tmp.readFile(filepath[type]); // Read entity data

    // Scan through all entries of the particular data type until a matching ID is found
    for (int i = 0; i <= tmp.sizeData; i++) {
        if (tmp.data[i]["entityID"] == id) {
            // Load different data depending on entity type
            switch (type) {
                case 1:   // Stars
                    s.entID = tmp.data[i]["entityID"].asUInt();
                    s.parentID = tmp.data[i]["parentID"].asUInt();
                    s.numSats = tmp.data[i]["numSats"].asUInt();
                    s.name = tmp.data[i]["name"].asString();
                    s.radius = tmp.data[i]["s_radius"].asDouble() * SOL_RADIUS;
                    s.mass = tmp.data[i]["s_mass"].asDouble() * SOL_MASS;
                    s.gravity = tmp.data[i]["s_gravity"].asDouble() * EARTH_GRAV;
                    s.temp = tmp.data[i]["s_temp"].asDouble();
                    s.age = tmp.data[i]["s_age"].asDouble();
                    s.luminosity = tmp.data[i]["s_luminosity"].asDouble() * SOL_LUMINOSITY;
                    s.habitable_min = tmp.data[i]["s_habitable_min"].asDouble() * AU;
                    s.habitable_max = tmp.data[i]["s_habitable_max"].asDouble() * AU;
                    break;
                case 2:  // Planets
                    p.entID = tmp.data[i]["entityID"].asUInt();
                    p.parentID = tmp.data[i]["parentID"].asUInt();
                    p.numSats = tmp.data[i]["numSats"].asUInt();
                    p.name = tmp.data[i]["name"].asString();
                    p.radius = tmp.data[i]["p_radius"].asDouble() * EARTH_RADIUS;
                    p.mass = tmp.data[i]["p_mass"].asDouble() * EARTH_MASS;
                    p.gravity = tmp.data[i]["p_gravity"].asDouble() * EARTH_GRAV;
                    p.temp = tmp.data[i]["p_temp"].asDouble();
                    p.age = tmp.data[i]["p_age"].asDouble();
                    p.dist = tmp.data[i]["p_dist"].asDouble() * AU;
                    p.period = tmp.data[i]["p_period"].asDouble();
                    p.solarConst = tmp.data[i]["p_solarConst"].asDouble();
                    p.pressure = tmp.data[i]["p_pressure"].asDouble() * 101325;
                    p.albedo = tmp.data[i]["p_albedo"].asDouble();
                    p.emissivity = tmp.data[i]["p_emissivity"].asDouble();
                    p.greenhouseEff = tmp.data[i]["p_greenhouseEff"].asDouble();
                    p.coreComp = tmp.data[i]["p_coreComp"].asString();
                    p.atmDens = tmp.data[i]["p_atmDens"].asString();
                    p.atmComp = tmp.data[i]["p_atmComp"].asString();
                    break;
                case 3:    // Moons/Satellites
                    m.entID = tmp.data[i]["entityID"].asUInt();
                    m.parentID = tmp.data[i]["parentID"].asUInt();
                    //m.numSats = tmp.data[i]["numSats"].asUInt();
                    m.name = tmp.data[i]["name"].asString();
                    m.radius = tmp.data[i]["m_radius"].asDouble() * EARTH_RADIUS;
                    m.mass = tmp.data[i]["m_mass"].asDouble() * EARTH_MASS;
                    m.gravity = tmp.data[i]["m_gravity"].asDouble() * EARTH_GRAV;
                    m.temp = tmp.data[i]["m_temp"].asDouble();
                    //m.age = tmp.data[i]["m_age"].asDouble();
                    break;
                case 4:  // Minor planets
                    //m.entID = tmp.data[i]["entityID"].asUInt();
                    break;
                case 5:  // Other objects
                    //m.entID = tmp.data[i]["entityID"].asUInt();
                    break;
                default:
                    printf("ERROR: No valid data to load!\n");
                    return true;
            }
            break;  // Exit for loop
        }
        else if (i == tmp.sizeData) {
            printf("ERROR: ID %d not found, it is likely not of type %d.\n", id, type);
            return true;
        }
    }

	return false;
}

// Display information about an entity in a stylized ASCII tree
void Worldman::printEntTree(int id, int type, int numSats) {
    // Variables for the satellite (child) entity
    std::string name2;
    int num = 0, id2, type2 = type + 1, numSats2;

    Fileman sat;
    sat.readFile(filepath[type2]); // Read the entire data file of the child entity

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
bool Worldman::printEntityInfo (int id) {
    Worldman ent;
    int type = getTypeByID(id);

    // Load entity info using ID and type
    if (loadEntity(id, type)) {
        printf("ERROR: ID did not return results.\n");
        return true;
    }

    switch (type) {
        case 1: // Stars
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
            break;
        case 2: // Planets
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
            break;
        case 3: // Dwarf Planets
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
            break;
        case 4: // Minor planets
            printf("Description: A dwarf planet or possibly asteroid.\n");
            break;
        case 5: // Other
            printf("Description: Some ball of matter.\n");
            break;
        default:
            printf("ERROR: No valid data to display!\n");
            return true;
	}

    return false;
}

// Create a new entity entry
bool Worldman::createEntity (int type, int subtype, int parID) {

    //printf("DEBUG: ParentID: %d Type: %d Subtype: %d\n", parID, type, subtype);
    // Load information of parent entity if it exists
    if (loadEntity(parID, type - 1)) {
        printf("ERROR: Parent entity was not loaded.\n");
        return true;
    }

    // Load the type specific data file for storing
    switch (type) {
        case 1:
            s.create(subtype, s);
            printf("\nGenerated star named %s...\n", s.name.c_str());
            break;
        case 2:
            p.create(subtype, s);
            printf("Generated planet named %s...\n", p.name.c_str());
            break;
        case 3:
            m.create(subtype, p);
            printf("Generated moon named %s...\n", m.name.c_str());
            break;
        case 4:
            //createMinor(subtype);
            break;
        case 5:
            //createMinor(subtype);
            break;
        default:
            printf("ERROR: Invalid Type selected!\n\n");
            return true;
    }

    // ***Message about keeping data here? Perhaps verification compared to other entities (planets with same orbital distances)***
    // Copy entity properties to JSON data file in memory
    saveEntity(type);
    return false;
}

// Generate a star system randomly with variable stars, plants, moons etc.
void Worldman::generateStarSystem() {
    printf("\n***Generating a new Star System...***\n");
    s.numStars = 1;
    // Create a star system including a star(s), orbiting planets, their moons and any other nearby celestial objects such as comets or asteroids
    for (s.counter = 1; s.counter <= s.numStars; s.counter++) {
        if (s.counter == 1)
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
}

// Generate objects that do not require a parent star (black holes, degenerate stars, rogue planets etc.)
void Worldman::generateStellarObject() {
    printf("NOT IMPLEMENTED YET.\n");
    printf("Generating a new Stellar Object...\n");
}

// Generate a star system manually according to user input
void Worldman::generateManual() {
    int id, type;
    printf("NOT IMPLEMENTED YET.\n");
    printf("Entering manual generation. Enter the parent ID of an entity: ");
    id = checkValue(0, 999);

    type = getTypeByID(id);
    if (type == 1) {
        printf("Generating a new planet around star.\n");
        loadEntity(id, type);
        //p.createManual(s);
    }
    else if (type == 2) {
        printf("Generating a new moon around planet.\n");
        loadEntity(id, type);
        //m.createManual(p);
    }
    else {
        printf("Invalid Type!\n");
    }
}
