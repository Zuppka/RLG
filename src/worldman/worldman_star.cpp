/*
World Manager - Star Generator
Date: Jan-5-2015

Desc: The math and logic behind the generation of star and stellar mass entities
*/

#include <modules\worldman.h>
#include <modules\toolman.h>

#include <cstdio>
#include <cmath>

// Events that occur during entity geneation
void Star::events () {
    // Random events during early age
    // Random events throughout life
    // Random events during late age
}

// Generate properties for a new type 1 - Star entity
void Star::create (size_t subtype, const Star& parent) {
    char endName = 64 + counter;

    if (counter == 1)
        name = randName(); // Generate a random star name
    else
        name = parent.name + " " + endName;
    //if (numStars > 1)
        //name = name + " " + endName;

    switch (subtype) {
        case 0:
            printf("RANDOM STAR (Not implemented)\n\n");
            break;
        case 1:
            if (debugLvl)
                printf("DEBUG: Main Sequence Star\n"); // Can range from (brown) red to blue in colour. See [10] for more info
            /* Initialize star properties */
            numSats = rng(0,4);
            radius = SOL_RADIUS;
            mass = SOL_MASS;
            temp = SOL_TEMP;
            age = 4.57;   // Sun age is ~4.567 billion years
            break;
        case 2:
            printf("Red Giant\n\n");
            break;
        case 3:
            printf("Red Supergiant\n\n");
            break;
        case 4:
            printf("Red Dwarf (Not implemented)\\n\n");
            break;
        case 5:
            printf("Brown dwarf (Not implemented)\\n\n");
            break;

    }
    // Properties
    gravity = G * mass / (radius * radius); // Surface gravity
    luminosity = 4 * PI * radius * radius * SIGMA * pow(temp, 4);
    habitable_min = sqrt((luminosity / SOL_LUMINOSITY) / 1.1) * AU;	// Minimum habitable distance [4]
    habitable_max = sqrt((luminosity / SOL_LUMINOSITY) / 0.53) * AU;	// Maximum habitable distance [4]
}
