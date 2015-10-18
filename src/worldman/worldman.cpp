/*
World Manager Functions
Date: Sept-20-2014
*/

#include <imgui/imgui.h>

#include <modules/worldman.h>
#include <modules/toolman.h>
#include <modules/fileman.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

const char *filepath[6];

// Initialize necessary files and folders
void Worldman::init() {
    const char *datapath = "database";
    const char *filename[6] = {"00_index.json", "01_stars.json", "02_planets.json", "03_moons.json", "04_minorPlanets.json", "05_other.json"};
    int len = strlen(datapath) + 2, arraySize = sizeof(filename) / sizeof(filename[0]);
    char *tmpPath;

    makeDir(datapath);

    // Create database files
    for (int i = 0; i < arraySize; i++) {
        // Setup the name of the file path
        tmpPath = new char[len + strlen(filename[i])];
        strcpy(tmpPath, datapath);
        strcat(tmpPath, "\\");
        strcat(tmpPath, filename[i]);
        filepath[i] = tmpPath;

        makeFile(filepath[i]);
    }
}

// Search the index array for matching ID and return the type
int Worldman::getTypeByID(int id) {
    Fileman tmp;
    int si;
    tmp.readFile(filepath[0]); // Read index file
    si = tmp.data.size();

    // Return if requested ID is out of index bounds
    if (id < 0 || id > si) {
        printf("ERROR: ID is out of range.\n");
        return 0;
    }
    // Scan through all entries in the index file for a matching ID
    for (int i = 0; i <= si; i++) {
        if (tmp.data[i]["entityID"] == id)
            return tmp.data[i]["type"].asInt();
    }
    printf("ERROR: ID was not found.\n");
    return 0;
}

void Worldman::getIndex() {
    Fileman tmp;
    int si;

    tmp.readFile(filepath[0]); // Read index file
    si = tmp.data.size();

    int arr[si][2];

    // Scan through all entries in the index file
    for (int i = 0; i <= si; i++) {
        arr[i][0] = tmp.data[i]["entityID"].asInt();
        arr[i][1] = tmp.data[i]["type"].asInt();
    }
}

// Save data to latest entry position
bool Worldman::saveEntity(Worldman* w) {
    Fileman tmp;
    int sd, si;

    // Read existing index file and get size (last entry position)
    tmp.readFile(filepath[0]);
    si = tmp.data.size();
    // Skip reading index if file is empty
    if (!si && !w->entID) {
        // Check if entity already exists, if so update it
        for (int i = 0; i <= si; i++) {
            if (tmp.data[i]["entityID"] == w->entID) {
                si = i;
                break;
            }
        }
    }
    // Add entity id and entity type to the index database
    tmp.data[si]["entityID"] = si;
    tmp.data[si]["type"] = w->type;
    tmp.writeFile(filepath[0]);

    // Read data file and get number of existing entities before appending
    tmp.readFile(filepath[w->type]);
    sd = tmp.data.size();
    // Skip reading data if file is empty
    if (!sd && !w->entID) {
        // Check if entity already exists, if so update it
        for (int i = 0; i <= sd; i++) {
            if (tmp.data[i]["entityID"] == w->entID) {
                sd = i;
                break;
            }
        }
    }
    // Assign values to the content of entry
    switch(w->type) {
        case 1:     // Stars
            w->entID = si;
            tmp.data[sd]["entityID"]        = w->entID;
            if (w->counter == 1)
                tmp.data[sd]["parentID"]    = w->entID;
            else
                tmp.data[sd]["parentID"]    = w->parentID;
            // Save stored data to object vars
            tmp.data[sd]["numStars"]        = w->numStars;
            tmp.data[sd]["numSats"]         = w->numSats;
            tmp.data[sd]["name"]            = w->name;
            tmp.data[sd]["s_radius"]        = deciTrunc(w->radius / SOL_RADIUS, 3);
            tmp.data[sd]["s_mass"]          = deciTrunc(w->mass / SOL_MASS, 3);
            tmp.data[sd]["s_gravity"]       = round(w->gravity / EARTH_GRAV);
            tmp.data[sd]["s_temp"]          = w->temp;
            tmp.data[sd]["s_age"]           = w->age;
            tmp.data[sd]["s_luminosity"]    = deciTrunc(w->luminosity / SOL_LUMINOSITY, 3);
            tmp.data[sd]["s_habitable_min"] = deciTrunc(w->habitable_min / AU, 3);
            tmp.data[sd]["s_habitable_max"] = deciTrunc(w->habitable_max / AU, 3);
            break;
        case 2:     // Planets
            w->entID = si;
            tmp.data[sd]["entityID"]        = w->entID;
            tmp.data[sd]["parentID"]        = w->parentID;
            tmp.data[sd]["numSats"]         = w->numSats;
            tmp.data[sd]["name"]            = w->name;
            tmp.data[sd]["p_radius"]        = deciTrunc(w->radius / EARTH_RADIUS, 3);
            tmp.data[sd]["p_mass"]          = deciTrunc(w->mass / EARTH_MASS, 3);
            tmp.data[sd]["p_gravity"]       = deciTrunc(w->gravity / EARTH_GRAV, 2);
            tmp.data[sd]["p_temp"]          = round(w->temp);
            tmp.data[sd]["p_age"]           = w->age;
            tmp.data[sd]["p_dist"]          = deciTrunc(w->dist / AU, 3);
            tmp.data[sd]["p_period"]        = deciTrunc(w->period, 1);
            tmp.data[sd]["p_solarConst"]    = round(w->solarConst);
            tmp.data[sd]["p_pressure"]      = deciTrunc(w->pressure / 101325, 3);
            tmp.data[sd]["p_albedo"]        = deciTrunc(w->albedo, 3);
            tmp.data[sd]["p_emissivity"]    = deciTrunc(w->emissivity, 3);
            tmp.data[sd]["p_greenhouseEff"] = deciTrunc(w->greenhouseEff, 3);
            tmp.data[sd]["p_coreComp"]      = w->coreComp;
            tmp.data[sd]["p_atmDens"]       = w->atmDens;
            tmp.data[sd]["p_atmComp"]       = w->atmComp;
            break;
        case 3:     // Moons/Satellites
            w->entID = si;
            tmp.data[sd]["entityID"]        = w->entID;
            tmp.data[sd]["parentID"]        = w->parentID;
            //tmp.data[sd]["numSats"]       = w->numSats;
            tmp.data[sd]["name"]            = w->name;
            tmp.data[sd]["m_radius"]        = deciTrunc(w->radius / EARTH_RADIUS, 3);
            tmp.data[sd]["m_mass"]          = deciTrunc(w->mass / EARTH_MASS, 3);
            tmp.data[sd]["m_gravity"]       = deciTrunc(w->gravity / EARTH_GRAV, 3);
            tmp.data[sd]["m_temp"]          = round(w->temp);
            //tmp.data[sd]["m_age"]         = w->age;
            break;
        case 4:     // Minor Planets
             tmp.data[sd]["entityID"]       = si;
             break;
        case 5:     // Other objects
            tmp.data[sd]["entityID"]        = si;
            break;
        default:
            printf("ERROR: No valid data to save!\n");
            return true;
    }
    tmp.writeFile(filepath[w->type]);  // Save entity properties from memory to disk if type was valid
	return false;
}

// Load data by entID
bool Worldman::loadEntity(Worldman* w, int id) {
    Fileman tmp;
    int sd;
    w->type = getTypeByID(id);

    // Check if id is valid and returns an entity type
    if (!w->type) {
        return true;
    }
    // Check if the type is valid (between 1-5).
    if (w->type < 0 || w->type > 5) {
        printf("ERROR: Invalid DATA type.\n");
        return true;
    }
    tmp.readFile(filepath[type]); // Read entity data
    sd = tmp.data.size();

    // Scan through all entries of the particular data type until a matching ID is found
    for (int i = 0; i <= sd; i++) {
        if (tmp.data[i]["entityID"] == id) {
            // Load different data depending on entity type
            switch (w->type) {
                case 1:   // Stars
                    w->entID             = tmp.data[i]["entityID"].asUInt();
                    w->parentID          = tmp.data[i]["parentID"].asUInt();
                    w->numSats           = tmp.data[i]["numSats"].asUInt();
                    w->name              = tmp.data[i]["name"].asString();
                    w->radius            = tmp.data[i]["s_radius"].asDouble() * SOL_RADIUS;
                    w->mass              = tmp.data[i]["s_mass"].asDouble() * SOL_MASS;
                    w->gravity           = tmp.data[i]["s_gravity"].asDouble() * EARTH_GRAV;
                    w->temp              = tmp.data[i]["s_temp"].asDouble();
                    w->age               = tmp.data[i]["s_age"].asDouble();
                    w->luminosity        = tmp.data[i]["s_luminosity"].asDouble() * SOL_LUMINOSITY;
                    w->habitable_min     = tmp.data[i]["s_habitable_min"].asDouble() * AU;
                    w->habitable_max     = tmp.data[i]["s_habitable_max"].asDouble() * AU;
                    break;
                case 2:  // Planets
                    w->entID             = tmp.data[i]["entityID"].asUInt();
                    w->parentID          = tmp.data[i]["parentID"].asUInt();
                    w->numSats           = tmp.data[i]["numSats"].asUInt();
                    w->name              = tmp.data[i]["name"].asString();
                    w->radius            = tmp.data[i]["p_radius"].asDouble() * EARTH_RADIUS;
                    w->mass              = tmp.data[i]["p_mass"].asDouble() * EARTH_MASS;
                    w->gravity           = tmp.data[i]["p_gravity"].asDouble() * EARTH_GRAV;
                    w->temp              = tmp.data[i]["p_temp"].asDouble();
                    w->age               = tmp.data[i]["p_age"].asDouble();
                    w->dist              = tmp.data[i]["p_dist"].asDouble() * AU;
                    w->period            = tmp.data[i]["p_period"].asDouble();
                    w->solarConst        = tmp.data[i]["p_solarConst"].asDouble();
                    w->pressure          = tmp.data[i]["p_pressure"].asDouble() * 101325;
                    w->albedo            = tmp.data[i]["p_albedo"].asDouble();
                    w->emissivity        = tmp.data[i]["p_emissivity"].asDouble();
                    w->greenhouseEff     = tmp.data[i]["p_greenhouseEff"].asDouble();
                    w->coreComp          = tmp.data[i]["p_coreComp"].asString();
                    w->atmDens           = tmp.data[i]["p_atmDens"].asString();
                    w->atmComp           = tmp.data[i]["p_atmComp"].asString();
                    break;
                case 3:    // Moons/Satellites
                    w->entID             = tmp.data[i]["entityID"].asUInt();
                    w->parentID          = tmp.data[i]["parentID"].asUInt();
                    //w->numSats         = tmp.data[i]["numSats"].asUInt();
                    w->name              = tmp.data[i]["name"].asString();
                    w->radius            = tmp.data[i]["m_radius"].asDouble() * EARTH_RADIUS;
                    w->mass              = tmp.data[i]["m_mass"].asDouble() * EARTH_MASS;
                    w->gravity           = tmp.data[i]["m_gravity"].asDouble() * EARTH_GRAV;
                    w->temp              = tmp.data[i]["m_temp"].asDouble();
                    //w->age             = tmp.data[i]["m_age"].asDouble();
                    break;
                case 4:  // Minor planets
                    //w->entID           = tmp.data[i]["entityID"].asUInt();
                    break;
                case 5:  // Other objects
                    //w->entID           = tmp.data[i]["entityID"].asUInt();
                    break;
                default:
                    printf("ERROR: No valid data to load!\n");
                    return true;
            }
            break;  // Exit for loop
        }
        else if (i == sd) {
            printf("ERROR: ID %d not found, it is likely not of type %d.\n", id, w->type);
            return true;
        }
    }

	return false;
}

// Display information about an entity in a stylized ASCII tree
void Worldman::printEntTree(int id, int type, int numSats) {
    // Variables for the satellite (child) entity
    std::string name2;
    int num = 0, id2, type2 = type + 1, numSats2, sd;

    Fileman sat;
    sat.readFile(filepath[type2]); // Read the entire data file of the child entity
    sd = sat.data.size();

    // Search through entire data file
    for (int i = 0; i < sd; i++) {
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
bool Worldman::printEntityInfo(int id) {
    Worldman w;

    // Load entity info using ID
    if (loadEntity(&w, id)) {
        printf("ERROR: ID did not return results.\n");
        return true;
    }

    switch (w.type) {
        case 1: // Stars
            ImGui::Text("\n%s (%d)\n", w.name.c_str(), w.entID);
            //printEntTree(w.entID, w.type, w.numSats);
            ImGui::Text("\n");
            ImGui::Text("Name:             %s (%d)\n", w.name.c_str(), w.entID);
            ImGui::Text("Solar Masses:     %.2f\n", w.mass / SOL_MASS);
            ImGui::Text("Solar Radii:      %.2f\n", w.radius / SOL_RADIUS);
            ImGui::Text("Solar Luminosity: %.2f\n", w.luminosity / SOL_LUMINOSITY);
            ImGui::Text("Surface Gravity:  %.0f G\n", w.gravity / EARTH_GRAV);
            ImGui::Text("Surface Temp:     %.0f K\n", w.temp);
            ImGui::Text("Planets:          %d\n\n", w.numSats);
            break;
        case 2: // Planets
            ImGui::Text("\n%s (%d)\n", w.name.c_str(), w.entID);
            //printEntTree(w.entID, w.type, p.numSats);
            ImGui::Text("\n");
            ImGui::Text("Name:             %s (%d)\n", w.name.c_str(), w.entID);
            ImGui::Text("Earth Masses:     %.2f\n", w.mass / EARTH_MASS);
            ImGui::Text("Radius:           %.0f Km\n", w.radius / 1000.0);
            ImGui::Text("Surface Gravity:  %.2f G\n", w.gravity / EARTH_GRAV);
            ImGui::Text("Semi-Major Axis:  %.3f AU\n", w.dist / AU);
            ImGui::Text("Orbital Period:   %.1f Days\n", w.period);
            //printf("Rotational Period:   %.1f Days\n", p.period);
            ImGui::Text("Core Compostion:  %s\n", w.coreComp.c_str());
            ImGui::Text("Atmopshere:       %s %s\n", w.atmDens.c_str(), w.atmComp.c_str());
            ImGui::Text("Surface Pressure: %.0f ATM\n", w.pressure / 101325);
            ImGui::Text("Surface Temp:     %.0f K\n", w.temp);
            ImGui::Text("Moons:            %d\n\n", w.numSats);
            break;
        case 3: // Dwarf Planets
            ImGui::Text("\n%s (%d)\n", w.name.c_str(), w.entID);
            ImGui::Text("\n");
            ImGui::Text("Name:              %s (%d)\n", w.name.c_str(), w.entID);
            ImGui::Text("Earth Masses:      %.2f\n", w.mass / EARTH_MASS);
            ImGui::Text("Radius:            %.0f Km\n", w.radius / 1000.0);
            ImGui::Text("Surface Gravity:   %.2f G\n", w.gravity / EARTH_GRAV);
            //printf("Semi-Major Axis:   %.3f AU\n", m.dist / AU);
            //printf("Orbital Period:    %.1f Days\n", p.period);
            //printf("Rotational Period: %.1f Days\n", p.period);
            //printf("Surface Pressure:  %.3f ATM\n", m.pressure);
            ImGui::Text("Surface Temp:      %.0f K\n\n", w.temp);
            break;
        case 4: // Minor planets
            ImGui::Text("Description: A dwarf planet or possibly asteroid.\n");
            break;
        case 5: // Other
            ImGui::Text("Description: Some ball of matter.\n");
            break;
        default:
            ImGui::Text("ERROR: No valid data to display!\n");
            return true;
	}

    return false;
}

// Generate a star system randomly with variable stars, plants, moons etc.
void Worldman::generateStarSystem() {
    Star s;
    Planet p;
    Moon m;

    printf("\n***Generating a new Star System...***\n");
    //s.numStars = 1; // TODO: Randomize this value
    // Create a star system including a star(s), orbiting planets, their moons and any other nearby celestial objects such as comets or asteroids
    for (s.counter = 1; s.counter <= s.numStars; s.counter++) {
        s.create(s.subtype, &s);
        printf("\nGenerated star named %s...\n", s.name.c_str());
        saveEntity(&s);
        // Generate planets orbiting the star
        for (p.counter = 1; p.counter <= s.numSats;  p.counter++) {
            p.Init(p.subtype, &s);
            p.create(p.subtype, &s);
            printf("Generated planet named %s...\n", p.name.c_str());
            saveEntity(&p);
            // Generate moons orbiting the planets
            for (m.counter = 1; m.counter <= p.numSats; m.counter++) {
                m.create(m.subtype, &p);
                printf("Generated moon named %s...\n", m.name.c_str());
                saveEntity(&m);
            }
        }
    }
    // Generate asteroid fields and other star system objects
    // createEntity(4, p_parentID);
}

// Generate a specified entity given a parentID
int Worldman::generateEntity(int type, int parentID) {
    Star s;
    Planet p;
    Moon m;

    switch (type) {
        case 1:
            if (!s.loadEntity(&s, parentID) && s.type == 1) {
                // Increase number of stars in system and save parent
                s.numStars++;
                //saveEntity(&s);
                // TODO: allow slective saving/editing of existing JSON
                // Create and save new star
                s.create(s.subtype, &s);
                printf("\nGenerated star named %s...\n", s.name.c_str());
                saveEntity(&s);
                return s.entID;
            }
            break;
        case 2:
            if (!s.loadEntity(&s, parentID) && s.type == 1) {
                // Increase number of planets in system and save parent
                s.numSats++;
                //saveEntity(&s);
                // Create and save new planet
                p.create(p.subtype, &s);
                printf("Generated planet named %s...\n", p.name.c_str());
                saveEntity(&p);
                return p.entID;
            }
        case 3:
            if (!p.loadEntity(&p, parentID) && p.type == 2) {
                // Increase number of moons in system and save parent
                p.numSats++;
                //saveEntity(&p);
                // Create and save new moon
                m.create(m.subtype, &p);
                printf("Generated moon named %s...\n", m.name.c_str());
                saveEntity(&m);
                return m.entID;
            }
        case 4:
            if (!s.loadEntity(&s, parentID)) {
                //p.create(p.subtype, &s);
                return 0;
            }

    }
    return 0;
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

    // TODO: Randomize subtype? Allow for custom subtypes such as desert planets in manual gen
    type = getTypeByID(id);
    if (type == 1) {
        printf("Generating a new planet around star.\n");
        //loadEntity(id, type);
        //p.createManual(s);
    }
    else if (type == 2) {
        printf("Generating a new moon around planet.\n");
        //loadEntity(id, type);
        //m.createManual(p);
    }
    else {
        printf("Invalid Type!\n");
    }
}
