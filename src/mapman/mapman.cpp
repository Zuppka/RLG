/*
Tile Manager Main
Date: Jan-7-2015

Desc: Creates and manages a coordinate system for entities with surfaces and various parameters associated
with each location (resource quantities, structures, physical characteristics etc).

*/

#include <modules\mapman.h>
#include <modules\rezman.h>
#include <modules\worldman.h>
#include <modules\fileman.h>
#include <modules\toolman.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>

// Initialize necessary files and folders
void Mapman::init() {
    makeDir(datapath);    // Datapath directory "maps"
    makeFile(indexPath);  // Index file location "index.json"
    // Do a file check here to ensure files listed in index exist
}

// Check index for an existing ID. Save path if ID exists
bool Mapman::checkIndex(int id) {
    Fileman tmp;
    int si;

    // Read existing index file
    tmp.readFile(indexPath);
    si = tmp.data.size();

    // Make sure tiles have not already been generated for the entity
    for (int i = 0; i < si; i++) {
        if (tmp.data[i]["EntID"].asInt() == id) {
            return false;  // id was found in the index
        }
    }
    return true;     // id was not found in the index
}

// Read existing resources and choose one based on input parameters
void Mapman::chooseRez(Mapman* m, int num1, int num2, int num3) {
    Fileman tmp;
    Rezman r;
    int sd, num;

    r.init();
    // Generate path name using function parameters
    r.num[0] = num1;
    r.num[1] = num2;
    r.num[2] = num3;
    r.genRezPath(&r);
    num1 = r.num[0];    // Weird stuff happens if r.num[2] is used below and it was changed in fn above
    num2 = r.num[1];
    num3 = r.num[2];

    // Read path and obtain size
    tmp.readFile(r.rezPath);
    sd = tmp.data.size();

    // Generate a new resource if the file is empty and re-read the file
    if (sd == 0) {
        r.genRezAuto(num1, num2, num3);
        tmp.readFile(r.rezPath);
        sd = tmp.data.size();
    }
    // Obtain a random resource from the typ/sub-type determined above
    num = rng(0, sd - 1);
    // Save a copy of the RezID to the map object
    m->mapRez[m->mapRez.size()] = tmp.data[num]["RezID"].asInt();
}

// Save a set of tiles
void Mapman::saveMapData(Mapman m) {
    Fileman tmp;
    std::ostringstream s, pathStream;
    std::string mapPath, rez;
    int si;

    // Read the tile index and add a new entry for the new entity
    tmp.readFile(indexPath);
    si = tmp.data.size();
    for (int i = 0; i <= si; i++) {
        if (tmp.data[i]["EntID"].asInt() == m.entID) {
            break;
        }
        else if (i == si) {
            // Save basic information for quick lookup
            tmp.data[si]["EntID"] = m.entID;
            tmp.data[si]["EntName"] = m.entName;
            // Write and save tile index
            tmp.writeFile(indexPath);
        }
    }
    // Generate a file name and path for individual map data
    pathStream << datapath << "\\ent" << m.entID << ".json";
    mapPath = pathStream.str();
    makeFile(mapPath);
    // Read existing tile data and append to end of file
    tmp.readFile(mapPath);

    // General data for all map files. Only print once when GridA is blank
    if (m.gridA.empty()) {
        tmp.data[0]["EntID"] = m.entID;
        tmp.data[0]["EntName"] = m.entName;
        tmp.data[0]["EntComp"] = m.entComp;
        tmp.data[0]["MapRez"] = m.mapRez;
        tmp.data[0]["EntLife"] = m.entLife;
        tmp.data[0]["GridSize"] = m.gridSize;
        tmp.data[0]["Seed"] = m.seed;
    }
    tmp.data[0]["t_gridA"] = m.gridA;

    // Write and save tile data
    tmp.writeFile(mapPath);

}

// Load an existing set of tiles
bool Mapman::loadMapData(int id, Mapman* m) {
    Fileman tmp;
    std::ostringstream s, pathStream;
    std::string mapPath, rez;

    // Check if the entity ID exists in the index
    if (checkIndex(id)) {
        printf("No entry with that ID exists!\n");
        return true;
    }
    // Set path
    pathStream << datapath << "\\ent" << id << ".json";
    mapPath = pathStream.str();
    // Load
    tmp.readFile(mapPath);
    m->entID = tmp.data[0]["EntID"].asInt();
    m->entName = tmp.data[0]["EntName"].asString();
    m->entComp = tmp.data[0]["EntComp"].asString();
    m->entLife = tmp.data[0]["EntLife"].asInt();
    m->mapRez = tmp.data[0]["MapRez"];
    m->gridSize = tmp.data[0]["GridSize"].asInt();
    m->seed = tmp.data[0]["Seed"].asInt();
    m->gridA = tmp.data[0]["t_gridA"];

    return false;
}

// Generate and save tiles for an entity if none exist yet
bool Mapman::genMap(int id) {
    Fileman tmp;
    Mapman m;
    std::string entPath;
    int sd, val;
    double radius = EARTH_RADIUS, surfArea, tileLen;

    if (!checkIndex(id)) {
        printf("Entity already has tiles generated!\n");
        return true;
    }

    // Get type by ID, load entPath based on type of entity
    entPath = "database\\02_planets.json";
    tmp.readFile(entPath);
    sd = tmp.data.size();
    // Load planet entity
    for (int i = 0; i <= sd; i++) {
        val = tmp.data[i]["entityID"].asInt();
        if (val && val == id) {
            m.entName = tmp.data[i]["name"].asString();
            radius = tmp.data[i]["p_radius"].asDouble() * EARTH_RADIUS;
            m.entComp = tmp.data[i]["p_coreComp"].asString();
            m.entLife = tmp.data[i]["p_life"].asInt();
            break;
        }
        else if (i == sd){
            printf("Planet ID not found!\n");
            return true;
        }
    }
    // Determine number of tiles on planet based on size
    surfArea = 4 * PI * radius * radius;
    tileLen = 2.5e6;   // Length of each tile in meters
    m.gridSize = sqrt (surfArea / (tileLen * tileLen));   // Number of tiles
    m.seed = rng(0, 1000);
    m.entID = id;

    printf("Name: %s\n", m.entName.c_str());
    printf("SurfArea: %.2e KM^2   Len of tile: %.2f KM   Num of Tiles: %d   Seed: %d\n", surfArea / 1e6, tileLen / 1e3, m.gridSize, m.seed);

    // Make a set of entity specific resources
    // Create a set of resources based on planet parameters to populate the tile with
    chooseRez(&m, 0, 0, -1);    // Native Metal
    if (m.entComp == "Hydrous Silicate") {
        chooseRez(&m, 0, 8, -1);    // Silicate
    }
    else {
        chooseRez(&m, 0, -1, -1);    // Random Mineral
    }
    // Generate flora and fauna if life exists
    if (m.entLife == 1) {
        chooseRez(&m, 2, -1, -1);  // Random Flora
        chooseRez(&m, 3, -1, -1);  // Random Fauna
        chooseRez(&m, 1, 1, 0);    // Random Water
    }
    else {
        chooseRez(&m, 0, -1, -1);  // Random Mineral
        chooseRez(&m, 1, -1, -1);  // Random Chemical
    }
    //chooseRez(t.gridB, 1, -1, -1);  // Random Chemical
    // Save map to file
    saveMapData(m);

    return false;
}

// Populate tiles with resources
bool Mapman::genTile(int id) {
    Mapman m;
    Rezman r;
    int sizeA, sizeB, num;

    // Ensure that the ID exists
    if (checkIndex(id)) {
        printf("No entry with that ID exists!\n");
        return true;
    }
    // Load existing map data
    loadMapData(id, &m);

    // Get user input for what tile to scan
    printf("Enter height.\n");
    m.r = checkValue(0, 2);
    printf("Enter theta.\n");
    m.theta = checkValue(-m.gridSize, m.gridSize);
    printf("Enter phi.\n");
    m.phi = checkValue(-m.gridSize, m.gridSize);
    printf("Scanning GridA tile [%d, %d, %d]...\n", m.r, m.theta, m.phi);
    printf("Enter GridB x coord.\n");
    m.xB = checkValue(1, 10);
    printf("Enter GridB y coord.\n");
    m.yB = checkValue(1, 10);

    // Check if an entry already exists for the tile value
    sizeA = m.gridA.size();
    // First scan and check gridA for a match
    for (int i = 0; i <= sizeA; i++) {
        if (sizeA > 0 && m.gridA[i]["z"].asInt() == m.r && m.gridA[i]["x"].asInt() == m.theta && m.gridA[i]["y"].asInt() == m.phi) {
            m.gridB = m.gridA[i]["t_gridB"];
            sizeB = m.gridB.size();
            // Second scan and check gridB if there was a gridA match
            for (int j = 0; j <= sizeB; j++) {
                if (sizeB > 0 && m.gridB[j]["x"] == m.xB && m.gridB[j]["y"] == m.yB) {
                    printf("Entry already exists. (tile already scanned, get update)\n");
                    //update code
                    return false;
                }
                // Create gridB info if no sub-tile entry is found
                else if (j == sizeB) {
                    printf("Sub-tile entry was not found. Adding...\n\n");
                    m.gridB[sizeB]["x"] = m.xB;
                    m.gridB[sizeB]["y"] = m.yB;
                    // Load rez data by ID
                    num = m.mapRez.size();
                    for (int k = 0; k < num; k++) {
                        r.loadRezInfo(&r, m.mapRez[k].asInt());
                        m.mapRezValue[k] = m.seed * r.attr[0];
                    }
                    m.gridB[0]["MapRezValue"] = m.mapRezValue;
                    m.gridA[i]["t_gridB"] = m.gridB;
                }
            }
            break;
        }
        // Append to gridA if no tile entry found. Also create gridB info
        else if (i == sizeA) {
            printf("Tile entry was not found. Adding...\n\n");
            m.gridA[sizeA]["z"] = m.r;
            m.gridA[sizeA]["x"] = m.theta;
            m.gridA[sizeA]["y"] = m.phi;
            m.gridB[0]["x"] = m.xB;
            m.gridB[0]["y"] = m.yB;
            // Load rez data by ID
            num = m.mapRez.size();
            for (int k = 0; k < num; k++) {
                r.loadRezInfo(&r, m.mapRez[k].asInt());
                m.mapRezValue[k] = m.seed * r.attr[0];
            }
            m.gridB[0]["MapRezValue"] = m.mapRezValue;
            m.gridA[i]["t_gridB"] = m.gridB;
        }
    }
    // Clear temp gridB and save tile data to file
    gridB.clear();
    saveMapData(m);

    return false;
}

//
void Mapman::printTileInfo(int id) {

}
