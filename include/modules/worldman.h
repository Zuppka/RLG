// RLG Project 2015

#ifndef WORLDMAN_H
#define WORLDMAN_H

#include <string>

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
    public:
        Worldman() {
            debugLvl = 0;
            counter = 0;
            entID = 0;
        }
        // Functions common to all Worldman entities
        void init();
        int getTypeByID (int);
        void getIndex();
        bool saveEntity(Worldman*);
        bool loadEntity(Worldman*, int);
        void printEntTree(int, int, int);
        bool printEntityInfo(int);
        void generateStarSystem();
        int generateEntity(int, int);
        void generateStellarObject();
        void generateManual();
        // Variables common to all Worldman entities
        int type, subtype, entID, numSats, parentID, counter, debugLvl;
        double radius, mass, temp, gravity, age;
        std::string name;
        // Star variables
        double luminosity, habitable_min, habitable_max;
        int numStars;
        // Planet variables
        double dist, period, solarConst, pressure, escVel, j;
        double albedo, albedoCloud, emissivity, emissCloud, emissSurf, greenhouseEff, cloudCover, cloudFactor, surfWater, surfIce, boilTemp, freezeTemp, ageLife;
        size_t lifeCheck, hasLife, magField;
        std::string atmComp, atmDens, coreComp;
        // Planet property strings
        std::string atmCompType[7] = {"None", "Hydrogen/Helium", "Nitrogen", "Oxygen", "Carbon Dioxide", "Methane", "Sodium Gas"};
        std::string atmType[5] = {"None", "Tenuous", "Minimal", "Substantial", "Dense"};
        std::string coreType[5] = {"None", "Small Nickel-Iron", "Large Nickel-Iron", "Hydrous Silicate", "Metallic Hydrogen"};

    private:
};

// Star creation class
class Star : public Worldman {
    public:
        Star() {
            type = 1;
            subtype = 1;
            numStars = 1;
        }
        // Star functions
        void events();
        void create(size_t, Star*);

    private:
};

// Planet creation class
class Planet : public Worldman {
    public:
        Planet() {
            type = 2;
            subtype = 0;
        }
        // Planet functions
        void Init(size_t, Star*);
        void valueCheck();
        void events();
        void create(size_t, Star*);

    private:
};

// Moon and natural satellite class
class Moon : public Worldman {
    public:
        Moon() {
            type = 3;
            subtype = 0;
        }
        // Moon functions
        void events();
        void create(size_t, Planet*);
        // Moon variables

    private:
};

// Minor planets and bodies class
class Minor : public Worldman {
    public:
        // Minor planet functions
        void events();
        void create(size_t);
        // Minor planet variables
    private:
};

#endif // WORLDMAN_H
