/*
World Manager Generator
Date: Sept-20-2014

Desc: The math behind the generation of all entity properties including planets and stars.
*/

#include <math.h>
#include <random>
#include <ctime>

#include "worldman.h"
#include <json/json.h>

// Shared data storage variables
extern Json::Value data;
extern int sizeIndex, sizeData;


// Random number seed
unsigned seed;

// Planet variables
const char *p_atmComp, *p_atmDens, *p_coreComp;
double p_radius, p_dist, p_period, p_mass, p_solarConst, p_pressure, p_temp, p_gravity, p_escVel;
double p_albedo, p_albedoCloud, p_emissivity, p_emissCloud, p_emissSurf, p_greenhouseEff, p_cloudCover, p_cloudFactor, p_surfWater, p_surfIce, p_boilTemp, p_freezeTemp, p_age = 0.0, p_ageLife;
size_t p_lifeCheck, p_hasLife, p_magField;
// Star variables
double s_radius, s_mass, s_luminosity, s_intensity, s_temp, s_habitable_min, s_habitable_max, s_age;

// Save data
size_t saveEntityInfo (size_t type) {

    if (type == 1) {    // Dwarf Planets
        data[sizeData]["entityID"] = sizeIndex;
    }
	else if (type == 2) {   // Planets
        // Assign values to the content of entry
        data[sizeData]["entityID"] = sizeIndex;
        data[sizeData]["p_radius"] = p_radius;
        data[sizeData]["p_dist"] = p_dist;
        data[sizeData]["p_period"] = p_period;
        data[sizeData]["p_mass"] = p_mass;
        data[sizeData]["p_gravity"] = p_gravity;
        data[sizeData]["p_solarConst"] = p_solarConst;
        data[sizeData]["p_pressure"] = p_pressure;
        data[sizeData]["p_temp"] = p_temp;
        data[sizeData]["p_albedo"] = p_albedo;
        data[sizeData]["p_emissivity"] = p_emissivity;
        data[sizeData]["p_greenhouseEff"] = p_greenhouseEff;
        data[sizeData]["p_coreComp"] = p_coreComp;
        data[sizeData]["p_atmDens"] = p_atmDens;
        data[sizeData]["p_atmComp"] = p_atmComp;
        data[sizeData]["p_age"] = p_age;

	}
    else if (type == 3) {   // Gas Giants
        data[sizeData]["entityID"] = sizeIndex;
	}
    else if (type == 4) {   // Stars
        data[sizeData]["entityID"] = sizeIndex;
        data[sizeData]["s_radius"] = s_radius;
        data[sizeData]["s_mass"] = s_mass;
        data[sizeData]["s_luminosity"] = s_luminosity;
        data[sizeData]["s_temp"] = s_temp;
        data[sizeData]["s_habitable_min"] = s_habitable_min;
        data[sizeData]["s_habitable_max"] = s_habitable_max;
        data[sizeData]["s_age"] = s_age;
	}
    else if (type == 5) {   // Compact objects
        data[sizeData]["entityID"] = sizeIndex;
	}
	else {
        printf("ERROR: No valid data to write!\n");
        return -1;
	}

	return 0;
}

// Print out final generated attributes and properties of entity
size_t printEntityInfo (size_t type) {
    if (type == 1) {    // Dwarf Planets

    }
    else if (type == 2) {   // Planets
        printf("Description: The planet orbits a star of luminosity %.2f L_sol and mass of %.2f M_sol. ", s_luminosity / SOL_LUMINOSITY, s_mass / SOL_MASS);
        printf("The planet's semi-major axis is %.3f AU and has a mass of %.2f M_earth with a gravitational acceleration of %.2f G. ",  p_dist / AU, p_mass / EARTH_MASS, p_gravity / EARTH_GRAV);
        printf("It takes the planet %.0f days to orbit its star. The solar constant at the plant is %.0f W / m^2. ", p_period, p_solarConst);
        printf("The plant has a %s core. The %s atmosphere composed primarily of %s and the pressure is about %.3f bar. ", p_coreComp, p_atmDens, p_atmComp, p_pressure / 1e5);
        printf("The average surface temperature of the planet is %.0f K (%.0f C).\n\n", p_temp, p_temp - 273);
    }
    else if (type == 3) {   // Gas Giants

    }
    else if (type == 4) {   // Stars

    }
    else if (type == 5) {   // Compact objects

    }
    else {
        printf("ERROR: No valid data to display!\n");
        return -1;
	}
    return 0;
}

// Various random events that can occur during generation
void genEvents () {
    // These events only apply to planets with atmospheres
    if (p_pressure > 0.0) {
        if (p_age <= 0.5 && p_surfWater < 1.0)
            p_surfWater += 0.05;
        if (!p_magField) {
            //p_pressure -= (p_temp - 273) * 1e3;
        }
        // Generate a number, chance of life increases over time and with more surface water
        if (p_surfWater > 0.0 && p_temp < p_boilTemp && p_temp > p_freezeTemp - 50)
            p_lifeCheck = rand() % (int)(100 * p_surfWater / p_age + 1);
        else if ((p_temp >= p_boilTemp || p_temp <= p_freezeTemp - 50) && p_hasLife > 0)
            p_hasLife--;

        if (p_lifeCheck <= 5) {
            //p_hasLife++;
            //p_greenhouseEff -= 0.005;
            printf("Basic life evolved!\n");
        }
        if (p_hasLife >= 5) {
            if (p_ageLife == 0.0)
                p_ageLife = p_age;
            if (p_pressure > 1e3)
                p_pressure -= 0.131 * p_pressure * exp(p_age - p_ageLife);
            p_greenhouseEff -= p_greenhouseEff * 0.075;
            printf("Planet is earth-like!\n");
        }
    }
}

// Create a new entity entry
size_t Worldman::createEntity (size_t type, size_t subtype) {

    int num;
    // Planet property strings
    std::string atmCompType[7] = {"None", "Hydrogen/Helium", "Nitrogen", "Oxygen", "Carbon Dioxide", "Methane", "Sodium Gas"};
    std::string atmType[5] = {"None", "Tenuous", "Minimal", "Substantial", "Dense"};
    std::string coreType[5] = {"None", "Small Nickel-Iron", "Large Nickel-Iron", "Hydrous Silicate", "Metallic Hydrogen"};

    // Random number generator related properties
    std::default_random_engine generator;
    std::lognormal_distribution<double> distribution(0.5, 0.85);
    seed = std::time(0);    // Simple randomization of the seed
    generator.seed(seed);
    srand(seed);

    /* Initialize star properties */ // NOTE: Should read them from existing star in the future
    s_radius = SOL_RADIUS;
    s_temp = SOL_TEMP;
    s_mass = SOL_MASS;
    s_luminosity = 4 * PI * s_radius * s_radius * SIGMA * pow(s_temp, 4);
    s_habitable_min = sqrt((s_luminosity / SOL_LUMINOSITY) / 1.1) * AU;	// Minimum habitable distance [4]
    s_habitable_max = sqrt((s_luminosity / SOL_LUMINOSITY) / 0.53) * AU;	// Maximum habitable distance [4]
    s_age = 4.6; // Age of the sun ~ 4.6 billion years
    s_intensity = 0.7 + 0.3 * p_age / (s_age - 0.1);  // Initial solar intensity ~+10% per billion years

    /* Initialize planet properties */
    // Orbital parameters
    //std::lognormal_distribution<double> distribution(0.5, 0.85);
    p_dist = 1.524 * AU; //distribution(generator) * AU; 	// Mercury 0.387, Venus 0.723, Earth 1.00, Mars 1.524, Titan 9.5
    p_period = 2 * PI * sqrt(pow(p_dist, 3) / (G * s_mass)) / 86400;

    // Physical parameters
    p_mass = 0.107 * EARTH_MASS;    // Mercury 0.0553, Venus 0.815, Mars 0.107, Titan 0.0225 EARTH_MASS
    p_radius = 0.532 * EARTH_RADIUS; // Mercury 0.383, Venus 0.950, Mars 0.532, Titan 0.404 EARTH_RADIUS
    p_gravity = G * p_mass / (p_radius * p_radius); // Surface gravity
    p_escVel = sqrt(2 * p_gravity * p_radius);  // Escape velocity

    // Planet core composition
    num = 1 + rand() % 3;
    p_coreComp = coreType[num].c_str();

    // Other properties
    p_solarConst = s_luminosity * s_intensity / (4 * PI * p_dist * p_dist);
    p_surfWater = 0.0;
    p_surfIce = 0.0;
    p_freezeTemp = 273.0;
    p_age = 0.0;    // Planet age
    p_ageLife = 0.0;    // Age of first life on planet
    p_hasLife = 0;  // Planet has no life to begin
    p_magField = 0;// rand() % 2;    // Magnetic field created after? Keeps solar badstuff away
    // Assuming early rocky planet in all cases. Initial atmosphere parameters
    p_albedo = 0.068;    // Bond albedo: Mercury 0.068 , Venus 0.90, Earth 0.306, Mars 0.25 [5]
    p_albedoCloud = 0.315; // Albedo for water vapour clouds
    p_emissivity = 0.96;

    /* Planet aging and atmosphere + climate development */
    // Planet too small and too close to star for atmo
    printf("Min: %.3e Max: %.3e Grav: %.3f \n\n", s_habitable_min / AU, s_habitable_max / AU, p_gravity);
    if (p_dist < s_habitable_min && p_gravity < 5) {
        p_pressure = 0.0;
        p_atmDens = atmType[1].c_str(); // Tenuous
    }
    // Planet can support an atmosphere
    else {
        p_pressure = p_gravity * p_gravity * 1e5;   // Pressure dependent on surface gravity + ADD random factor
        p_boilTemp = 135.84 * pow(p_pressure, 0.0879);  // Calculate boiling point of water at the pressure using: y = 135.84*x^0.0879 [8]
        p_surfWater = 0.75;  // All planets with atmo start with some oceans [9]
        // Update the initial cloud emissivity parameters. Assuming water vapour clouds form initially.
        p_greenhouseEff = 0.806; // Lots of CO2 and GHG in atmo
        p_cloudCover = 0.15;
        p_cloudFactor = 0.33;
        p_albedo = p_cloudCover * 0.90 + p_surfIce * 0.90;
        // Total emissivity = groundCover * ground emiss + cloudCover * avg cloud emiss - GHG
        p_emissSurf = 0.96 * (1 - p_cloudCover);
        p_emissCloud = p_cloudCover * p_cloudFactor;
        p_emissivity = p_emissSurf + p_emissCloud - p_greenhouseEff;
    }


    p_temp = pow(p_solarConst * (1 - p_albedo) / (4 * SIGMA * p_emissivity), 0.25);    // 70% effective early star. Recalculate temperature
    p_cloudFactor = 0.65; // Increase the significance of clouds in cooling

    // Loop cycle generates history of planet. Starts 0.1 billion years after star formation.
    while (p_age <= s_age - 0.1) {
        // Print Debug info!
        printf("Emiss: %.3f = %.3f + %.3f - %.3f Albedo: %.3f\n", p_emissivity, p_emissSurf, p_emissCloud, p_greenhouseEff, p_albedo);
        printf("Pressure: %.2f bar\n", p_pressure / 1e5);
        printf("Temp: %.2f K (%.2f C) Boiling: %.2f K (%.2f C) \n", p_temp, p_temp - 273, p_boilTemp, p_boilTemp - 273);
        printf("CloudCover: %.3f\n", p_cloudCover);
        printf("Age: %.2f Intensity: %.2f SurfWater: %.2f%% SurfIce: %.2f%%\n\n", p_age, s_intensity, p_surfWater * 100, p_surfIce * 100);

        // Increase planet age
        p_age += 0.1;
        // Introduce random events
        genEvents();

        // First check if atmosphere exerts any pressure
        if (p_pressure > 0.0) {
            // Reduce the p_pressure due to CO2 absorption by ocean up to half initial pressure
            p_pressure -= 0.025 * p_pressure / p_age;

            // Check if water boiling. Increase greenhouse effect and pressure, decrease surface water if water present.
            if (p_temp >= p_boilTemp) {
                printf("Too hot, water boiling!\n");
                // If planet still has some surface water, decrease it
                if (p_surfWater > 0.0) {
                    p_surfWater -= 0.15;
                    if (p_surfWater <= 0.0)
                        p_surfWater = 0.0;
                    // Increase cloud cover as surface water turns into water vapour
                    p_cloudCover += 0.03 / p_cloudCover;
                    p_cloudFactor += 0.03 * p_cloudFactor;
                    // More GHG over time as H2O/CO2 is released
                    p_greenhouseEff += 0.001;
                    // Increase pressure due to evaporating liquids, volcanoes and other effects
                    p_pressure += (p_temp - p_boilTemp) * 3e3;
                }
                else {
                    // If there is no mag field, atmospheric H2O seperates and reacts with sulfur to form sulfur dioxide
                    if (!p_magField) {
                        // SO2 clouds
                        p_pressure += (p_temp - p_boilTemp) * 1000; // NOTE: should increase faster
                        p_cloudCover += 0.28 * p_cloudCover;
                        p_cloudFactor = p_greenhouseEff + 0.0039;
                        p_albedoCloud = 0.90; // Albedo factor for SO2 clouds
                    }
                    printf("Water boiled away, no surface water!\n");
                }
            }
            // Check if water is frozen. Ice increases surface albedo and freezes water out of the atmosphere reducing clouds and pressure.
            else if (p_temp <= p_freezeTemp) {
                printf("Too cold, water freezing!\n");

                // If planet still has some surface water, decrease it
                if (p_surfWater > 0.0) {// Calculate boiling point of water at the pressure using: y = 135.84*x^0.0879 [8]
                    p_surfWater -= 0.10;
                    p_surfIce += 0.10;

                    // Decrease cloud cover
                    p_cloudCover -= 0.0043 / p_cloudCover;
                    // If no magnetic field, atmospheric escape
                    if (!p_magField) {
                        p_pressure -= (p_freezeTemp - p_temp) * 2e9 / p_pressure;
                    }
                }
                else {
                    printf("Water frozen, no surface water!\n");
                }
            }
            // Generate life if conditions are suitable (Not boiling or freezing)
            else {
                // Decrease water over time as more water evaporates into the hydrosphere to form clouds
                p_surfWater -= 0.0055 / p_surfWater;
                p_greenhouseEff -= 0.008;   // Less GHG over time as CO2 is absorbed by water
                // Depending if planet has a magnetic field or not
                if (p_magField) {
                    p_cloudCover += 0.0042 / p_cloudCover;  // Increase cloud cover as oceans turn to water vapour
                }
                else {
                    p_cloudCover += 0.0021 / p_cloudCover;
                }
            }

            // Ensure surface water does not go below 0% or above 100%
            if (p_surfWater < 0.0)
                p_surfWater = 0.0;
            else if (p_surfWater > 1.0)
                p_surfWater = 1.0;
            // Ensure surface ice does not go below 0% or above 100%
            if (p_surfIce < 0.0)
                p_surfIce = 0.0;
            else if (p_surfIce > 1.0)
                p_surfIce = 1.0;
            // Limit cloud cover
            if (p_cloudCover > 1.0)
                p_cloudCover = 1.0;
             else if (p_cloudCover > 1.0)
                p_cloudCover = 1.0;
            // Limit GHG
            if (p_greenhouseEff < 0.0)
                p_greenhouseEff = 0.0;
            // No pressure
            if (p_pressure <= 0.0) {
                p_pressure = 0.0;
                p_cloudCover = 0.0;
                p_greenhouseEff = 0.0;
            }

            // Calculate amount of surface ice on planet based on temperature and cloud cover
            if (p_temp < p_freezeTemp + 50) {
                p_surfIce +=  2 * (1 - p_cloudCover) / p_temp;  //NOTE: incremental not static value! Shouldn't be a fn of cloudcover
            }
            else {
                p_surfIce = 0.0;
            }

            // Update albedo as it scales with cloud cover
            p_albedo = p_cloudCover * p_albedoCloud + p_surfIce * 0.99;
            // Constrain albedo
            if (p_albedo > 1.0)
                p_albedo = 1.0;
            else if (p_albedo <= 0.0)
                p_albedo = 0.068;

            // Update the emissivity parameters
            p_emissSurf = 0.96 * (1 - p_cloudCover);
            p_emissCloud = p_cloudCover * p_cloudFactor;    //OLD VAL: 0.68 * p_cloudCoverL + 0.55 * p_cloudCoverM + 0.33 * p_cloudCoverH;
            p_emissivity = p_emissSurf + p_emissCloud - p_greenhouseEff;    // groundCover * ground emiss + cloudCover * avg cloud emiss - greenhouse EARTH 0.615 apparent
            // Calculate boiling point of water at the pressure using: y = 135.84*x^0.0879 [8]
            p_boilTemp = 135.84 * pow(p_pressure, 0.0879);
        }
        // Increase the solar intensity over time as the star ages
        s_intensity = 0.7 + 0.3 * p_age / (s_age - 0.1);
        p_solarConst = s_luminosity * s_intensity / (4 * PI * p_dist * p_dist);
        // Recalculate the planet's current temperature
        p_temp = pow(p_solarConst * (1 - p_albedo) / (4 * SIGMA * p_emissivity), 0.25);    // Recalculate the planet temperature. Using 1/4 ratio for A_abs/A_rad

    }
    // Print Debug info!
    printf("Emiss: %.3f = %.3f + %.3f - %.3f Albedo: %.3f\n", p_emissivity, p_emissSurf, p_emissCloud, p_greenhouseEff, p_albedo);
    printf("Pressure: %.4f bar\n", p_pressure / 1e5);
    printf("Temp: %.2f K (%.2f C) Boiling: %.2f K (%.2f C) \n", p_temp, p_temp - 273, p_boilTemp, p_boilTemp - 273);
    printf("CloudCover: %.3f\n", p_cloudCover);
    printf("Age: %.2f Intensity: %.2f SurfWater: %.2f%% SurfIce: %.2f%%\n\n", p_age, s_intensity, p_surfWater * 100, p_surfIce * 100);


    // Determine atmosphere density category depending on pressure
    num = 2 + rand() % 4;
    p_atmComp = atmCompType[num].c_str(); // Nitrogen, Oxygen, Carbon Dioxide, Methane
    if (p_pressure > 2e6)
        p_atmDens = atmType[4].c_str(); // Dense
    else if (p_pressure > 2e4)
        p_atmDens = atmType[3].c_str(); // Substantial
    else if (p_pressure > 100)
        p_atmDens = atmType[2].c_str(); // Minimal
    else {
        p_atmDens = atmType[1].c_str(); // Tenuous
        p_atmComp = atmCompType[0].c_str();   // None
    }

    /* OLD: Keep as an example of strcmp and normal_dist
    if (strcmp(p_atmDens, "Substantial") == 0) {
        std::normal_distribution<double> distribution(0.01, 0.25);
        p_pressure = abs(distribution(generator) * 1e6);
    }
    // Keep because these gave nice numbers when seed = 10, range of ~0.02 - 4.0 AU
    printf("Semi-Major Axis: %.2f AU\n", distribution(generator));
    printf("Semi-Major Axis: %.2f AU\n", distribution(generator));
    printf("Semi-Major Axis: %.2f AU\n", distribution(generator));
    printf("Semi-Major Axis: %.2f AU\n", distribution(generator));
    printf("Semi-Major Axis: %.2f AU\n", distribution(generator));
    */

    printEntityInfo(type);
    // Message about keeping data here?
    saveEntityInfo(type);

    return 0;
}
