/*
Tile Manager Main
Date: Jan-7-2015

Desc: Creates and manages a coordinate system for entities with surfaces and various parameters associated
with each location (resource quantities, structures, physical characteristics etc).

*/

#include <modules\tileman.h>
#include <modules\worldman.h>
#include <modules\fileman.h>
#include <modules\toolman.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>

// Initialize necessary files and folders
void Tileman::init() {

    Fileman file;
    file.makeDir(datapath);
    // Index file location
    indexPath += datapath;
    indexPath += "\\index.json";
    file.makeFile(indexPath);

}

// Generate and save tiles for an entity if none exist yet
bool Tileman::genTiles(int id) {
    Worldman w;
    Fileman tmp;
    int si, sd;
    double radius, length, sections;
    std::string entPath = "database\\02_planets.json";

    tmp.readFile(indexPath);
    si = tmp.sizeData;
    // Make sure tiles have not already been generated for the entity
    for (int i = 0; i < si; i++) {
        if (tmp.data[i]["entID"].asInt() == id) {
            printf("Entity already has tiles generated!\n");
            return true;
        }
    }

    tmp.readFile(entPath);
    sd = tmp.sizeData;
    // Load planet entity
    for (int i = 0; i < sd; i++) {
        if (tmp.data[i]["entityID"].asInt() == id) {
            printf("NAME: %s\n", tmp.data[i]["name"].asCString());
            radius = tmp.data[i]["p_radius"].asDouble() * EARTH_RADIUS;
        }
    }
    sections = 36;
    length = radius * 2 * PI / sections / 1000;
    printf ("length of segment: %.2f Sections: %.2f\n", length, sections);

    return false;
}

// Populate tiles with resources
void Tileman::popTiles() {

}

void Tileman::printTileInfo(int id) {

}
