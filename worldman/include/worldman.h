#ifndef WORLDMAN_H
#define WORLDMAN_H

#include <cstdio>
#include <iostream>

/* Bunch of constants */
#define PI 3.141592
#define G 6.67384e-11 				// N·(m/kg)^2
#define SOL_RADIUS 6.96e+8 		    // m
#define SOL_TEMP 5778				// K
#define SOL_MASS 1.9885e+30 		// kg
#define SOL_LUMINOSITY 3.846e+26 	// W
#define EARTH_MASS 5.9726e+24		// kg
#define EARTH_RADIUS 6.371e+6		// m
#define EARTH_GRAV 9.807 			// m/s^2
#define EARTH_MAG_FIELD 0.31		// gauss (31 micro Tesla)
#define SIGMA 5.67040e-8 			// J/s-m^2-K^4
#define AU 1.49597e+11 				// m

// World Manager class
class Worldman {
    private:

    public:
        int debugLvl = 0;
        int entID, numSats, parentID, counter;
        double radius, mass, temp, gravity, age;
        std::string name;

        void init ();
        int rng (int, int);
        double rngLog(double, double);
        std::string randName();

        //virtual void events() = 0;
        //virtual void create(size_t, const);
};

#endif // WORLDMAN_H
