// RLG Project 2015

#ifndef PLANET_H
#define PLANET_H

#include "worldman.h"
#include "star.h"

// Planet creation header
class Planet : public Worldman {
    private:

    public:
        // Planet variables
        double dist, period, solarConst, pressure, escVel, j;
        double albedo, albedoCloud, emissivity, emissCloud, emissSurf, greenhouseEff, cloudCover, cloudFactor, surfWater, surfIce, boilTemp, freezeTemp, ageLife;
        size_t lifeCheck, hasLife, magField;
        std::string atmComp, atmDens, coreComp;
        // Planet property strings
        std::string atmCompType[7] = {"None", "Hydrogen/Helium", "Nitrogen", "Oxygen", "Carbon Dioxide", "Methane", "Sodium Gas"};
        std::string atmType[5] = {"None", "Tenuous", "Minimal", "Substantial", "Dense"};
        std::string coreType[5] = {"None", "Small Nickel-Iron", "Large Nickel-Iron", "Hydrous Silicate", "Metallic Hydrogen"};

        void valueCheck();
        void events();
        void create(size_t, const Star&);
};

#endif // PLANET_H
