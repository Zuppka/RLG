/*
World Manager - Moon Generator
Date: Jan-5-2015

Desc: The math and logic behind the generation of moons and natural satellites around planets
*/

#include <modules\worldman.h>
#include <modules\toolman.h>

// Events that occur during entity geneation
void Moon::events () {
    // Random events during early age
    // Random events throughout life
    // Random events during late age
}

// Generate properties for a new type 3 - Moon/satellite entity
void Moon::create (size_t subtype, Planet* parent) {
    parentID = parent->entID;
    char endName = counter + 96;
    name = parent->name + endName;

    mass = 7.377e22; // Mass of the Moon
    radius = 1.737e6; // Radius of the Moon
    gravity = G * mass / (radius * radius); // Surface gravity

    temp = 40;
}
