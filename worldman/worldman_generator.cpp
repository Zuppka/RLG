/*
World Manager Generator
Date: Sept-20-2014

Desc: The math behind the generation of all entity properties including planets and stars.
*/

#include <math.h>
#include <random>
#include <ctime>
#include <stdio.h>
#include <string.h>

#include "worldman.h"
#include "fileman.h"
#include <json/json.h>

Fileman ent;    // Main list of entities

// Planet variables
double p_radius, p_dist, p_period, p_mass, p_solarConst, p_pressure, p_temp, p_gravity, p_escVel, j;
double p_albedo, p_albedoCloud, p_emissivity, p_emissCloud, p_emissSurf, p_greenhouseEff, p_cloudCover, p_cloudFactor, p_surfWater, p_surfIce, p_boilTemp, p_freezeTemp, p_age = 0.0, p_ageLife;
size_t p_numMoons, p_lifeCheck, p_hasLife, p_magField, p_starID, planetID;
const char *p_atmComp, *p_atmDens, *p_coreComp;
// Planet property strings
std::string p_name;
std::string atmCompType[7] = {"None", "Hydrogen/Helium", "Nitrogen", "Oxygen", "Carbon Dioxide", "Methane", "Sodium Gas"};
std::string atmType[5] = {"None", "Tenuous", "Minimal", "Substantial", "Dense"};
std::string coreType[5] = {"None", "Small Nickel-Iron", "Large Nickel-Iron", "Hydrous Silicate", "Metallic Hydrogen"};

// Star variables
size_t s_numPlanets;
double s_radius, s_mass, s_luminosity, s_temp, s_habitable_min, s_habitable_max, s_age;
std::string s_name;

// Random number generator related properties
unsigned randSeed = std::time(0);   // Dwarf Planets // Simple randomization of the seed
std::default_random_engine generator;
//std::lognormal_distribution<double> distribution(0.5, 0.85);  // Distribution of orbital distances for planets


// Dummy initialize for Fileman
void Worldman::init(){
    ent.init();
}

// Semi-dummy for checking if entity type matches a given ID
size_t Worldman::checkTypebyID (size_t id, size_t type) {
    ent.readIndex();   // Load index before checking
    if (ent.getTypeByID(id) == type) {  // Check if ID is an existing star
        printf("The entity %d is of type %d!\n", id, type);
        return 1;
    }
    else
        return 0;
}

// Get the last ID of the index
size_t Worldman::getLastID () {
    return ent.sizeIndex;
}

// Simple random name generator
std::string randName() {
    srand(std::time(NULL)); // Random seed based on time
    char tmp;
    int len = (rand() % 10) + 3;
    std::string result = "";

    // Add one random letter at a time until length reached
    for (int i = 0; i < len; i++) {
        if (i == 0)
            tmp = (rand() % 26) + 65;   // Start with random uppercase letter
        else
            tmp = (rand() % 26) + 97;   // Randomize new lowercase letter
        result = result + tmp;  // Add letter to result
    }

    return result;
}

// Small function to round doubles to desired precision
double deciTrunc (double value, int precision) {
    double roundValue;
    int mult;
    if (precision == 1)
         mult = 10;
    else
        mult = pow(10, precision);
    // Multiply value by a factor of 10, round to nearest integer, then divide by same factor
    roundValue = round(value * mult) / mult;
    return roundValue;
}

// Save data to latest entry position
size_t saveEntityInfo (size_t type) {
    int sd = ent.sizeData;

    switch(type) {
        case 0:     // Asteroid/comets
            break;
        case 1:     // Dwarf Planets
            ent.data[sd]["entityID"] = ent.sizeIndex;
            break;
        case 2:     // Planets
            // Assign values to the content of entry
            ent.data[sd]["entityID"] = ent.sizeIndex;
            ent.data[sd]["p_name"] = p_name;
            ent.data[sd]["p_radius"] = deciTrunc(p_radius / EARTH_RADIUS, 2);
            ent.data[sd]["p_dist"] = deciTrunc(p_dist / AU, 3);
            ent.data[sd]["p_period"] = deciTrunc(p_period, 1);
            ent.data[sd]["p_mass"] = deciTrunc(p_mass / EARTH_MASS, 3);
            ent.data[sd]["p_gravity"] = deciTrunc(p_gravity / EARTH_GRAV, 2);
            ent.data[sd]["p_solarConst"] = deciTrunc(p_solarConst, 1);
            ent.data[sd]["p_pressure"] = deciTrunc(p_pressure, 3);
            ent.data[sd]["p_temp"] = deciTrunc(p_temp, 1);
            ent.data[sd]["p_albedo"] = deciTrunc(p_albedo, 3);
            ent.data[sd]["p_emissivity"] = deciTrunc(p_emissivity, 3);
            ent.data[sd]["p_greenhouseEff"] = deciTrunc(p_greenhouseEff, 3);
            ent.data[sd]["p_coreComp"] = p_coreComp;
            ent.data[sd]["p_atmDens"] = p_atmDens;
            ent.data[sd]["p_atmComp"] = p_atmComp;
            ent.data[sd]["p_age"] = p_age;
            ent.data[sd]["p_starID"] = p_starID;
            break;
        case 3:     // Gas Giants
             ent.data[sd]["entityID"] = ent.sizeIndex;
             break;
        case 4:     // Stars
            ent.data[sd]["entityID"] = ent.sizeIndex;
            ent.data[sd]["s_name"] = s_name;
            ent.data[sd]["s_radius"] = deciTrunc(s_radius / SOL_RADIUS, 3);
            ent.data[sd]["s_mass"] = deciTrunc(s_mass / SOL_MASS, 3);
            ent.data[sd]["s_luminosity"] = deciTrunc(s_luminosity / SOL_LUMINOSITY, 3);
            ent.data[sd]["s_temp"] = s_temp;
            ent.data[sd]["s_habitable_min"] = deciTrunc(s_habitable_min / AU, 3);
            ent.data[sd]["s_habitable_max"] = deciTrunc(s_habitable_max / AU, 3);
            ent.data[sd]["s_age"] = s_age;
            break;
        case 5:     // Compact objects
            ent.data[sd]["entityID"] = ent.sizeIndex;
            break;
        default:
            printf("ERROR: No valid data to load!\n");
            return -1;
    }
	return 0;
}

// Load data by entID
size_t loadEntityByID (int id) {
    Fileman parEnt;
    // Parse entire type specific data file
    size_t type = parEnt.getTypeByID(id);
    parEnt.readData(type); // Read parent data

    // Scan through all entries of the particular data type until a matching ID is found
    for (size_t i = 0; i <= ent.sizeData; i++) {
        if (parEnt.data[i]["entityID"] == id) {
            // Load different data depending on entity type
            switch (type) {
                case 0:     // Asteroid/comets
                    // p_radius = data[i]["p_radius"] * 1000;
                    break;
                case 1:    // Dwarf Planets
                    // p_radius = data[i]["p_radius"] * 1000;
                    break;
                case 2:  // Planets
                    p_name = parEnt.data[i]["p_name"].asString();
                    p_radius = parEnt.data[i]["p_radius"].asDouble() * EARTH_RADIUS;
                    p_dist = parEnt.data[i]["p_dist"].asDouble() * AU;
                    p_period = parEnt.data[i]["p_period"].asDouble();
                    p_mass = parEnt.data[i]["p_mass"].asDouble() * EARTH_MASS;
                    p_gravity = parEnt.data[i]["p_gravity"].asDouble() * EARTH_GRAV;
                    p_solarConst = parEnt.data[i]["p_solarConst"].asDouble();
                    p_pressure = parEnt.data[i]["p_pressure"].asDouble();
                    p_temp = parEnt.data[i]["p_temp"].asDouble();
                    p_albedo = parEnt.data[i]["p_albedo"].asDouble();
                    p_emissivity = parEnt.data[i]["p_emissivity"].asDouble();
                    p_greenhouseEff = parEnt.data[i]["p_greenhouseEff"].asDouble();
                    p_coreComp = parEnt.data[i]["p_coreComp"].asCString();
                    p_atmDens = parEnt.data[i]["p_atmDens"].asCString();
                    p_atmComp = parEnt.data[i]["p_atmComp"].asCString();
                    p_age = parEnt.data[i]["p_age"].asDouble();
                    p_starID = parEnt.data[i]["p_starID"].asUInt();
                    break;
                case 3:  // Gas Giants
                    // p_radius = data[i]["p_radius"] * 1000;
                    break;
                case 4:   // Stars
                    s_name = parEnt.data[i]["s_name"].asString();
                    s_radius = parEnt.data[i]["s_radius"].asDouble() * SOL_RADIUS;
                    s_mass = parEnt.data[i]["s_mass"].asDouble() * SOL_MASS;
                    s_luminosity = parEnt.data[i]["s_luminosity"].asDouble() * SOL_LUMINOSITY;
                    s_temp = parEnt.data[i]["s_temp"].asDouble();
                    s_habitable_min = parEnt.data[i]["s_habitable_min"].asDouble() * AU;
                    s_habitable_max = parEnt.data[i]["s_habitable_max"].asDouble() * AU;
                    s_age = parEnt.data[i]["s_age"].asDouble();
                    break;
                case 5:  // Compact objects
                    // p_radius = data[i]["p_radius"] * 1000;
                    break;
                default:
                    printf("ERROR: No valid data to load!\n");
                    return -1;
            }
            break;  // Exit for loop
        }
        else if (i == ent.sizeData) {
            printf("DEBUG: ID not found. Likely it was not written.\n");
            return -1;
        }
    }

	return 0;
}

// Print out final generated attributes and properties of entity
size_t printEntityInfo (size_t type) {
    if (type == 1) {    // Dwarf Planets
        printf("Description: A rocky looking moon.\n");
    }
    else if (type == 2) {   // Planets
        printf("Description: The planet \"%s\" orbits a star of luminosity %.2f L_sol and mass of %.2f M_sol. ", p_name.c_str(), s_luminosity / SOL_LUMINOSITY, s_mass / SOL_MASS);
        printf("The planet's semi-major axis is %.3f AU and has a mass of %.2f M_earth with a gravitational acceleration of %.2f G. ",  p_dist / AU, p_mass / EARTH_MASS, p_gravity / EARTH_GRAV);
        printf("It takes the planet %.0f days to orbit its star. The solar constant at the plant is %.0f W / m^2. ", p_period, p_solarConst);
        printf("The plant has a %s core. The %s atmosphere composed primarily of %s and the pressure is about %.3f bar. ", p_coreComp, p_atmDens, p_atmComp, p_pressure / 1e5);
        printf("The average surface temperature of the planet is %.0f K (%.0f C).\n\n", p_temp, p_temp - 273);
    }
    else if (type == 3) {   // Gas Giants
        printf("Description: A large, colourful gas giant.\n");
    }
    else if (type == 4) {   // Stars
        printf("Description: The star \"%s\" has a luminosity of %.2f L_sol and mass of %.2f M_sol. ", s_name.c_str(), s_luminosity / SOL_LUMINOSITY, s_mass / SOL_MASS);
        printf("The star has a radius of %.2f R_sol and a surface temperature is %.0f.\n\n", s_radius / SOL_RADIUS, s_temp);
    }
    else if (type == 5) {   // Compact objects
        printf("Description: Some ball of degenerate matter.\n");
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
        if (j <= 0.5 && p_surfWater < 1.0)
            p_surfWater += 0.05;
        if (!p_magField) {
            //p_pressure -= (p_temp - 273) * 1e3;   // Reduce pressure if no magnetic field present. NOTE: Venus sim did not consider this
        }
        // Generate a number, chance of life increases over time and with more surface water
        if (p_surfWater > 0.0 && p_temp < p_boilTemp && p_temp > p_freezeTemp - 50)
            p_lifeCheck = 1;//rand() % (int)(100 * p_surfWater / p_age + 1);
        else if ((p_temp >= p_boilTemp || p_temp <= p_freezeTemp - 50) && p_hasLife > 0)
            p_hasLife--;

        if (p_lifeCheck <= 5) {
            p_hasLife++;
            p_greenhouseEff -= 0.005;
            //printf("Basic life evolved!\n");
        }
        if (p_hasLife >= 36) {
            if (p_ageLife == 0.0)
                p_ageLife = j;
            if (p_pressure > 1e3)
                p_pressure -= 0.15 * p_pressure * exp(j - p_ageLife);
            p_greenhouseEff -= p_greenhouseEff * 0.027;

            printf("Planet is earth-like!\n");
        }
    }
}

// Generate properties for a new type 0 - Asteroid/Comet entity
void createComet (size_t subtype) {

}

// Generate properties for a new type 1 - Dwarf Planet/Moon entity
void createMoon (size_t subtype) {

}

// Generate properties for a new type 2 - Planet entity
void createPlanet (size_t subtype) {

    int num;
    double s_lumTmp = s_luminosity, s_radTmp = s_radius, s_radiusInit;
    char letter;
    planetID = ent.sizeIndex;

    // Append the star name with a letter to set as the planet name ***Change to numbers/Roman numerals?***
    letter = (planetID - p_starID) + 97;
    p_name = s_name + " " + letter;

    generator.seed(randSeed);
    srand(randSeed);

    /* Initialize planet properties */
    // Orbital parameters
    //std::lognormal_distribution<double> distribution(0.5, 0.85);
    p_dist = 1.0 * AU; //distribution(generator) * AU; 	// Mercury 0.387, Venus 0.723, Earth 1.00, Mars 1.524, Titan 9.5
    p_period = 2 * PI * sqrt(pow(p_dist, 3) / (G * s_mass)) / 86400;

    // Physical parameters
    p_mass = 1.0 * EARTH_MASS;    // Mercury 0.0553, Venus 0.815, Mars 0.107, Titan 0.0225 EARTH_MASS
    p_radius = 1.0 * EARTH_RADIUS; // Mercury 0.383, Venus 0.950, Mars 0.532, Titan 0.404 EARTH_RADIUS
    p_gravity = G * p_mass / (p_radius * p_radius); // Surface gravity
    p_escVel = sqrt(2 * p_gravity * p_radius);  // Escape velocity

    // Planet core composition
    num = 1 + rand() % 3;
    p_coreComp = coreType[num].c_str();

    // Other properties
    s_radiusInit = s_radTmp / (round((0.042 * s_age + 1) * 100) / 100);
    s_lumTmp = 4 * PI * s_radiusInit * s_radiusInit * SIGMA * pow(s_temp, 4);
    p_solarConst = s_lumTmp / (4 * PI * p_dist * p_dist);
    if (p_solarConst <= 0) {
        printf("ERROR: Something probably went wrong. Solar const zero or negative!\n");
    }
    p_surfWater = 0.0;  // earth 0.71
    p_surfIce = 0.0;
    p_freezeTemp = 273.0;
    p_age = s_age - 0.05;    // Planet age
    p_ageLife = 0.0;    // Age of first life on planet
    p_hasLife = 0;  // Planet has no life to begin
    p_magField = 1;// rand() % 2;    // Magnetic field created after? Keeps solar badstuff away
    // Assuming early rocky planet in all cases. Initial atmosphere parameters
    p_albedo = 0.068;    // Bond albedo: Mercury 0.068 , Venus 0.90, Earth 0.306, Mars 0.25 [5]
    p_albedoCloud = 0.315; // Albedo for water vapour clouds
    p_emissivity = 0.96;

    /* Planet aging and atmosphere + climate development */
    // Planet too small and too close to star for atmo
    printf("Min: %.2f Max: %.2f Grav: %.3f \n\n", s_habitable_min / AU, s_habitable_max / AU, p_gravity);
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
        p_cloudCover = 0.15;    // Earth 0.68
        p_cloudFactor = 0.33;
        p_albedo = p_cloudCover * 0.90 + p_surfIce * 0.90;
        // Total emissivity = groundCover * ground emiss + cloudCover * avg cloud emiss - GHG
        p_emissSurf = 0.96 * (1 - p_cloudCover);
        p_emissCloud = p_cloudCover * p_cloudFactor;
        p_emissivity = p_emissSurf + p_emissCloud - p_greenhouseEff;
    }


    p_temp = pow(p_solarConst * (1 - p_albedo) / (4 * SIGMA * p_emissivity), 0.25);    // 70% effective early star. Recalculate temperature
    p_cloudFactor = 0.65; // Increase the significance of clouds in cooling

    // Loop cycle generates history of planet. Starts 0.1 billion years after star formation. Age of sun 4.567 bya, age of earth 4.54 bya (wikipedia)
    for (j = 0.0; j <= p_age; j += 0.1) {
        // Print Debug info!
        /*
        printf("Emiss: %.3f = %.3f + %.3f - %.3f Albedo: %.3f\n", p_emissivity, p_emissSurf, p_emissCloud, p_greenhouseEff, p_albedo);
        printf("Pressure: %.4f bar\n", p_pressure / 1e5);
        printf("Temp: %.2f K (%.2f C) Boiling: %.2f K (%.2f C) \n", p_temp, p_temp - 273, p_boilTemp, p_boilTemp - 273);
        printf("CloudCover: %.3f Factor: %.3f\n", p_cloudCover, p_cloudFactor);
        printf("Age: %.2f SurfWater: %.2f%% SurfIce: %.2f%%\n\n", j, p_surfWater * 100, p_surfIce * 100);
        */
        // Star aging with planet for simulation
        if (s_age > 0 && s_age <= 8) {
            // Radius increases over time according to age and stellar class
            s_radTmp = (round((0.042 * j + 1) * 100) / 100) * s_radiusInit;  // u = 0.042 * j + 1, rounded to nearest hundredth
        }
        // Entering red giant phase for main sequence?
        else if (s_age > 8) {
            s_radTmp = (0.1835 * j * j - 3.431 * j + 17.52) * s_radiusInit; //y = 0.1835*j^2 - 3.431*j + 17.52
        }


        // Introduce random events
        genEvents();

        // First check if atmosphere exerts any pressure
        if (p_pressure > 0.0) {
            // Reduce the p_pressure due to CO2 absorption by ocean up to half initial pressure
            p_pressure -= 0.025 * p_pressure / (j + 0.1);

            // Check if water boiling. Increase greenhouse effect and pressure, decrease surface water if water present.
            if (p_temp >= p_boilTemp) {
                printf("Too hot, water boiling!\n");
                // If planet still has some surface water, decrease it
                if (p_surfWater > 0.0) {
                    p_surfWater -= 0.15;
                    if (p_surfWater <= 0.0)
                        p_surfWater = 0.0;
                    // Increase cloud cover as surface water turns into water vapour
                    p_cloudCover += 0.035 / p_cloudCover;
                    p_cloudFactor += 0.033 * p_cloudFactor;
                    // More GHG over time as H2O/CO2 is released
                    p_greenhouseEff += 0.001;
                    // Increase pressure due to evaporating liquids, volcanoes and other effects
                    p_pressure += (p_temp - p_boilTemp) * 3e3;
                }
                else {
                    // If there is no mag field, atmospheric H2O seperates and reacts with sulfur to form sulfur dioxide
                    if (!p_magField) {
                        // SO2 clouds
                        p_pressure += (p_temp - p_boilTemp) * 100; // NOTE: should increase faster
                        p_cloudCover += 0.28 * p_cloudCover;
                        p_cloudFactor = p_greenhouseEff + 0.0039;
                        p_albedoCloud += 0.00552 / (p_albedoCloud * p_albedoCloud); // Albedo factor for SO2 clouds
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
                p_surfWater -= 0.0062 / p_surfWater;
                p_greenhouseEff -= 0.008;   // Less GHG over time as CO2 is absorbed by water
                // Depending if planet has a magnetic field or not
                if (p_magField) {
                    p_cloudCover += 0.0047 / p_cloudCover;  // Increase cloud cover as oceans turn to water vapour
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
            if (p_surfIce >= 0)
                p_surfIce +=  (p_freezeTemp - p_temp + 75) / 7.3e3;  //NOTE: incremental not static value! Shouldn't be a fn of cloudcover


            // Update albedo as it scales with cloud cover
            p_albedo = p_cloudCover * p_albedoCloud + p_surfIce * 0.99;
            // Constrain albedop_albedoCloud
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
        // Update luminosity and solar constant as star radius increases
        s_lumTmp = 4 * PI * s_radTmp * s_radTmp * SIGMA * pow(s_temp, 4);
        p_solarConst = s_lumTmp / (4 * PI * p_dist * p_dist);
        // Recalculate the planet's current temperature
        p_temp = pow(p_solarConst * (1 - p_albedo) / (4 * SIGMA * p_emissivity), 0.25);    // Recalculate the planet temperature. Using 1/4 ratio for A_abs/A_rad

    }
    // Print Debug info! Mercury: 440K (444K), Venus 737K (???K), Earth 288K (288K), Mars ~210K (???K)
    printf("Emiss: %.3f = %.3f + %.3f - %.3f Albedo: %.3f\n", p_emissivity, p_emissSurf, p_emissCloud, p_greenhouseEff, p_albedo);
    printf("Pressure: %.4f bar\n", p_pressure / 1e5);
    printf("Temp: %.2f K (%.2f C) Boiling: %.2f K (%.2f C) \n", p_temp, p_temp - 273, p_boilTemp, p_boilTemp - 273);
    printf("CloudCover: %.3f\n", p_cloudCover);
    printf("Age: %.2f SurfWater: %.2f%% SurfIce: %.2f%%\n\n", p_age, p_surfWater * 100, p_surfIce * 100);


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
}

// Generate properties for a new type 3 - Gas Giant entity
void createGasGiant (size_t subtype) {

}

// Generate properties for a new type 4 - Star entity
void createStar (size_t subtype) {
    p_starID = ent.sizeIndex;   // Set the starID to the latest free index entry. Planets will use this later
    s_name = randName();
    switch (subtype) {
        case 0:
            printf("RANDOM STAR (Not implemented)\n\n");
            break;
        case 1:
            printf("DEBUG: Main Sequence Star\n"); // Can range from (brown) red to blue in colour. See [10] for more info
            /* Initialize star properties */
            s_radius = SOL_RADIUS;
            s_temp = SOL_TEMP;
            s_mass = SOL_MASS;
            s_numPlanets = 3;
            s_age = 4.57;   // Sun age is ~4.567 billion years
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

    s_luminosity = 4 * PI * s_radius * s_radius * SIGMA * pow(s_temp, 4);
    s_habitable_min = sqrt((s_luminosity / SOL_LUMINOSITY) / 1.1) * AU;	// Minimum habitable distance [4]
    s_habitable_max = sqrt((s_luminosity / SOL_LUMINOSITY) / 0.53) * AU;	// Maximum habitable distance [4]

}

// Generate properties for a new type 5 - XX entity
// Generate properties for a new type 6 - XX entity

// Create a new entity entry
size_t createEntity (size_t type, size_t subtype, size_t parentID) {

    ent.readIndex();   // Parse existing index info
    ent.readData(type);    // Read updated entity data to make sure new data is appended to end

    printf("DEBUG: ID: %d Type: %d Subtype: %d\n", parentID, type, subtype);
    if (type != 4 && parentID >= 0) {
        loadEntityByID(parentID);   // Load information of parent entity if it exists
    }

    // Load the type specific data file for storing
    switch (type) {
        case 0:
            //createStar(subtype);
            break;
        case 1:
            createMoon(subtype);
            break;
        case 2:
            createPlanet(subtype);
            break;
        case 3:
            createGasGiant(subtype);
            break;
        case 4:
            createStar(subtype);
            break;
        case 5:
            //createStar(subtype);
            break;
        default:
            printf("ERROR: Invalid Type selected!\n\n");
            return -1;
    }

    // ***Message about keeping data here? Perhaps verification compared to other entities (planets with same orbital distances)***
    printEntityInfo(type);  // Output info about generated entity to console
    // Write the new data
    saveEntityInfo(type);   // Copy entity properties to JSON data file in memory
    ent.writeData(type);   // Save entity properties from memory to disk

    return 0;
}

// Create a star system including a star(s), orbiting planets, their moons and any other nearby celestial objects such as comets or asteroids
size_t Worldman::createStarSys () {

    // Generate the star
    createEntity(4, 1, -1);
    // Generate the planets surrounding the star
    for (size_t i = 0; i < s_numPlanets; i++) {
        createEntity(2, 0, p_starID);    // type = 1 for dwarf planets, type = 2 for terr planets, type = 3 for gas giants
        // Generate moons for the planets
        for (size_t j = 0; j < p_numMoons; j++) {
            //createEntity(1, 0, planetID);
        }
    }
    // Generate asteroid fields and other star system objects
    // createEntity(0);

    return 0;
}

