/*
World Manager - Planet Generator
Date: Jan-5-2015

Desc: The math and logic behind the generation of planets and (eventually) gas giants
*/

#include "..\include\planet.h"
#include "..\include\worldman.h"
#include <toolman.h>

#include <cstdio>
#include <cmath>

// Check if values are still within range
void Planet::valueCheck() {
    // Ensure surface water does not go below 0% or above 100%
    if (surfWater < 0.0)
        surfWater = 0.0;
    else if (surfWater > 1.0)
        surfWater = 1.0;
    // Ensure surface ice does not go below 0% or above 100%
    if (surfIce < 0.0)
        surfIce = 0.0;
    else if (surfIce > 1.0)
        surfIce = 1.0;
    // Limit cloud cover
    if (cloudCover < 0.0)
        cloudCover = 0.0;
    else if (cloudCover > 1.0)
        cloudCover = 1.0;
    // Limit GHG
    if (greenhouseEff < 0.0)
        greenhouseEff = 0.0;
    else if (greenhouseEff > 1.0)
        greenhouseEff = 1.0;

    // No pressure
    if (pressure <= 0.0) {
        pressure = 0.0;
        surfIce += surfWater;
        surfWater = 0.0;
        cloudCover = 0.0;
        greenhouseEff = 0.0;
        albedo = 0.068;
        if (surfIce > albedo)
            albedo = surfIce;
    }
}

// Events that occur during entity geneation
void Planet::events () {
    // These events only apply to planets with atmospheres
    if (pressure > 0.0) {
        if (j <= 0.3 && surfWater < 1.0)
            surfWater += 0.05;

        if (j <= 1.0) {
            // Reduce pressure if no magnetic field present during a young star (extreme UV)
            if (!magField)
                pressure -= 1e5 / gravity;
            // Early impactors cause some pressure decrease but volcanic events cause pressure increase. Random +/- pressure change
            //pressure -= 100 * gravity;
        }
        // NO LONG TERM ATMO LOSS: Rather minimal and ionosphere protects planet from solar winds. Temperture exiting atoms bigger reason for atm loss.

        // Generate a number, chance of life increases over time and with more surface water
        if (surfWater > 0.0 && temp < boilTemp && temp > freezeTemp - 50)
            hasLife++;//rand() % (int)(100 * surfWater / age + 1);
        else if ((temp >= boilTemp || temp <= freezeTemp - 50) && hasLife > 0)
            hasLife--;

        // Life results in increase in oxygen and decreses in GHG
        if (hasLife >= 36 && magField) {
            if (ageLife == 0.0)
                ageLife = j;
            if (pressure > 1e3)
                pressure -= 0.152 * pressure * exp(j - ageLife);
            // Increase surface water and decrease GHG. Increase in oxygen (from life/plants) allows bonding of hydrogen to form water
            greenhouseEff -= 0.032;
            surfWater += 0.024;
            if (debugLvl)
                printf("DEBUG: Planet is earth-like!\n");
        }
        // Basic life slightly reduces GHG and stabilizes environment
        else if (hasLife >= 5) {
            greenhouseEff -= 0.005;
            if (debugLvl)
                printf("DEBUG: Basic life evolved!\n");
        }
    }
    valueCheck();
}

// Generate properties for a new type 2 - Planet entity
void Planet::create (size_t subtype, const Star& parent) {
    int num;
    double lumTemp, radTemp, radiusInit;
    char endName = counter + 48;
    parentID = parent.entID;
    // Append the star name with a number for planet name ****NOTE: MAKE BETTER. Planet's are unordered***
    name = parent.name + " " + endName;

    /* Initialize planet properties */
    // Orbital parameters
    dist = rngLog(0.5, 0.85) * AU;   // Mercury 0.387, Venus 0.723, Earth 1.00, Mars 1.524, Titan 9.5
    /*
    if (counter == 1) {
        dist = 0.387 * AU;
        mass = 0.0553 * EARTH_MASS;
        radius = 0.383 * EARTH_RADIUS;
    }
    else if (counter == 2) {
        dist = 0.723 * AU;
        mass = 0.815 * EARTH_MASS;
        radius = 0.950 * EARTH_RADIUS;
    }
    else if (counter == 3) {
        dist = 1.0 * AU;
        mass = 1.0 * EARTH_MASS;
        radius = 1.0 * EARTH_RADIUS;
    }
    else if (counter == 4) {
        dist = 1.524 * AU;
        mass = 0.107 * EARTH_MASS;
        radius = 0.532 * EARTH_RADIUS;
    }
    */
    period = 2 * PI * sqrt(pow(dist, 3) / (G * parent.mass)) / 86400;

    // Physical parameters
    mass = 1.0 * EARTH_MASS;    // Mercury 0.0553, Venus 0.815, Mars 0.107, Titan 0.0225 EARTH_MASS
    radius = 1.0 * EARTH_RADIUS; // Mercury 0.383, Venus 0.950, Mars 0.532, Titan 0.404 EARTH_RADIUS
    gravity = G * mass / (radius * radius); // Surface gravity
    escVel = sqrt(2 * gravity * radius);  // Escape velocity

    // Planet core composition
    num = rng(1, 3);
    coreComp = coreType[num];

    // Other properties
    radiusInit = parent.radius / (round((0.042 * parent.age + 1) * 100) / 100);
    lumTemp = 4 * PI * radiusInit * radiusInit * SIGMA * pow(parent.temp, 4);
    solarConst = lumTemp / (4 * PI * dist * dist);
    if (solarConst <= 0) {
        printf("ERROR: Something probably went wrong. Solar const zero or negative!\n");
    }
    surfWater = 0.0;  // earth 0.71
    surfIce = 0.0;
    freezeTemp = 273.0;
    age = parent.age - 0.05;    // Planet age
    ageLife = 0.0;    // Age of first life on planet
    hasLife = 0;  // Planet has no life to begin
    if (counter == 3)
        magField = 1;
    else
        magField = 0;   // Magnetic field created after? Keeps solar badstuff away
    // Assuming early rocky planet in all cases. Initial atmosphere parameters
    albedo = 0.068;    // Bond albedo: Mercury 0.068 , Venus 0.90, Earth 0.306, Mars 0.25 [5]
    albedoCloud = 0.315; // Albedo for water vapour clouds
    emissivity = 0.96;
    numSats = rng(0, 3);

    /* Planet aging and atmosphere + climate development */
    // Planet too small and too close to star for atmo
    // printf("Min: %.2f Max: %.2f Grav: %.3f \n\n", parent.habitable_min / AU, parent.habitable_max / AU, gravity);
    if (dist < parent.habitable_min && gravity < 5) {
        pressure = 0.0;
        atmDens = atmType[1].c_str(); // Tenuous
    }
    // Planet can support an atmosphere
    else {
        pressure = gravity * gravity * 1e5;   // Pressure dependent on surface gravity + ADD random factor
        boilTemp = 135.84 * pow(pressure, 0.0879);  // Calculate boiling point of water at the pressure using: y = 135.84*x^0.0879 [8]
        surfWater = 0.75;  // All planets with atmo start with some oceans [9]
        // Update the initial cloud emissivity parameters. Assuming water vapour clouds form initially.
        greenhouseEff = 0.806; // Lots of CO2 and GHG in atmo
        cloudCover = 0.15;    // Earth 0.68
        cloudFactor = 0.33;
        albedo = cloudCover * albedoCloud + surfIce;
        // Total emissivity = groundCover * ground emiss + cloudCover * avg cloud emiss - GHG
        emissSurf = 0.96 * (1 - cloudCover);
        emissCloud = cloudCover * cloudFactor;
        emissivity = emissSurf + emissCloud - greenhouseEff;
    }


    temp = pow(solarConst * (1 - albedo) / (4 * SIGMA * emissivity), 0.25);    // 70% effective early star. Recalculate temperature
    cloudFactor = 0.65; // Increase the significance of clouds in cooling

    // Loop cycle generates history of planet. Starts 0.1 billion years after star formation. Age of sun 4.567 bya, age of earth 4.54 bya (wikipedia)
    for (j = 0.0; j <= age; j += 0.1) {
        // Print Debug info!
        if (debugLvl > 1) {
            printf("Emiss: %.3f = %.3f + %.3f - %.3f Albedo: %.3f\n", emissivity, emissSurf, emissCloud, greenhouseEff, albedo);
            printf("Pressure: %.4f bar\n", pressure / 1e5);
            printf("Temp: %.2f K (%.2f C) Boiling: %.2f K (%.2f C) \n", temp, temp - 273, boilTemp, boilTemp - 273);
            printf("CloudCover: %.3f Factor: %.3f\n", cloudCover, cloudFactor);
            printf("Age: %.2f SurfWater: %.2f%% SurfIce: %.2f%%\n\n", j, surfWater * 100, surfIce * 100);
        }
        // Star aging with planet for simulation
        if (parent.age > 0 && parent.age <= 8) {
            // Radius increases over time according to age and stellar class
            radTemp = (round((0.042 * j + 1) * 100) / 100) * radiusInit;  // u = 0.042 * j + 1, rounded to nearest hundredth
        }
        // Entering red giant phase for main sequence?
        else if (parent.age > 8) {
            radTemp = (0.1835 * j * j - 3.431 * j + 17.52) * radiusInit; //y = 0.1835*j^2 - 3.431*j + 17.52
        }

        // Introduce random events
        Planet::events();

        // First check if atmosphere exerts any pressure
        if (pressure > 0.0) {
            // Reduce the pressure due to CO2 absorption by ocean up to half initial pressure
            pressure -= 0.03 * surfWater * pressure / (j + 0.1);

            // Check if water boiling. Increase greenhouse effect and pressure, decrease surface water if water present.
            if (temp >= boilTemp) {

                // If planet still has some surface water, decrease it
                if (surfWater > 0.0) {
                    // Increase cloud cover as surface water turns into water vapour
                    if (surfWater - 0.25 > 0.0) {
                        cloudCover += 0.25;
                        surfWater -= 0.25;
                    }
                    else {
                        cloudCover += surfWater;
                        surfWater -= surfWater;
                    }

                    cloudFactor += exp (-4 * cloudFactor);
                    // More GHG over time as H2O/CO2 is released
                    greenhouseEff += 0.005;
                    // Increase pressure due to evaporating liquids, volcanoes and other effects
                    pressure += surfWater * 1.8e6;
                    if (debugLvl)
                        printf("DEBUG: Too hot, water boiling!\n");
                }
                else {
                    // If there is no mag field, atmospheric H2O seperates and reacts with sulfur to form sulfur dioxide
                    if (!magField) {
                        // SO2 clouds
                        pressure += pressure / 1.4e2;
                        greenhouseEff += (greenhouseEff * (1 - greenhouseEff / 0.996)) / 5;
                        cloudFactor += (cloudFactor * (1 - cloudFactor)) / 5;
                        albedoCloud += (albedoCloud * (1 - albedoCloud / 0.90)) / 5; // Albedo factor for SO2 clouds should approach 0.90
                    }
                    if (debugLvl)
                        printf("DEBUG: Water boiled away, no surface water!\n");
                }
            }
            // Check if water is frozen. Ice increases surface albedo and freezes water out of the atmosphere reducing clouds and pressure.
            else if (temp <= freezeTemp) {
                // If planet still has some surface water, decrease it
                if (surfWater > 0.0) {
                    if (debugLvl)
                        printf("DEBUG: Too cold, water freezing!\n");
                    surfWater -= 0.15;
                    surfIce += 0.005;

                    // Decrease cloud cover as H2O freezes out
                    cloudCover -= 0.045 / cloudCover;
                    cloudFactor -= 0.02;
                }
                else {
                    // Reduce pressure and GHG becasue reasons
                    pressure -= pressure / 1.7;
                    greenhouseEff -= greenhouseEff / 3;   // Reduced pressure on cold planets reduces density of GHG
                    if (debugLvl)
                        printf("DEBUG: Water frozen, no surface water!\n");
                }
            }
            // Conditions suitable for life (Not boiling or freezing)
            else {
                // Decrease water over time as more water evaporates into the hydrosphere to form clouds
                if (surfWater >= 0.0) {
                    cloudCover += 0.0047 / cloudCover;  // Increase cloud cover as oceans turn to water vapour
                    surfWater -= 0.007 / surfWater;
                    // pressure+?
                    greenhouseEff -= 0.0034;   // Less GHG over time as CO2 is absorbed by water, and oxygen populates air
                }
            }

            // Change amount of surface ice on planet based on temperature and cloud cover
            if (pressure < 1e4)
                surfIce -= surfIce / 7;    // Decrease if pressure is too low to sustain frozen ice (ice in atmosphere instead)
            else if (temp <= 320 && surfWater > 0.0)
                surfIce += 0.0233;    // Increase ice if below arbitrary temperture and surface still has water to freeze.

            valueCheck();

            // Update albedo as it scales with cloud cover and surface ice
            albedo = cloudCover * albedoCloud + surfIce;    // Earth ice ~9.2%, mars 15-18%

            // Update the emissivity parameters
            emissSurf = 0.96 * (1 - cloudCover);
            emissCloud = cloudCover * cloudFactor;    //OLD VAL: 0.68 * cloudCoverL + 0.55 * cloudCoverM + 0.33 * cloudCoverH;
            emissivity = emissSurf + emissCloud - greenhouseEff;    // groundCover * ground emiss + cloudCover * avg cloud emiss - greenhouse EARTH 0.615 apparent
            // Calculate boiling point of water at the pressure using: y = 135.84*x^0.0879 [8]
            boilTemp = 135.84 * pow(pressure, 0.0879);
            if (boilTemp <= freezeTemp)
                boilTemp = freezeTemp;
        }
        // Update luminosity and solar constant as star radius increases
        lumTemp = 4 * PI * radTemp * radTemp * SIGMA * pow(parent.temp, 4);
        solarConst = lumTemp / (4 * PI * dist * dist);
        // Recalculate the planet's current temperature
        temp = pow(solarConst * (1 - albedo) / (4 * SIGMA * emissivity), 0.25);    // Recalculate the planet temperature. Using 1/4 ratio for A_abs/A_rad

    }

    // Print Debug info
    if (debugLvl > 1) {
        printf("Emiss: %.3f = %.3f + %.3f - %.3f Albedo: %.3f\n", emissivity, emissSurf, emissCloud, greenhouseEff, albedo);
        printf("Pressure: %.4f bar\n", pressure / 1e5);
        printf("Temp: %.2f K (%.2f C) Boiling: %.2f K (%.2f C) \n", temp, temp - 273, boilTemp, boilTemp - 273);
        printf("CloudCover: %.3f Factor: %.3f\n", cloudCover, cloudFactor);
        printf("Age: %.2f SurfWater: %.2f%% SurfIce: %.2f%%\n\n", j, surfWater * 100, surfIce * 100);
    }

    // Determine atmosphere density category depending on pressure
    num = rng(2, 5);
    atmComp = atmCompType[num]; // Nitrogen, Oxygen, Carbon Dioxide, Methane
    if (pressure > 2e6)
        atmDens = atmType[4]; // Dense
    else if (pressure > 2e4)
        atmDens = atmType[3]; // Substantial
    else if (pressure > 100)
        atmDens = atmType[2]; // Minimal
    else {
        atmDens = atmType[1]; // Tenuous
        atmComp = atmCompType[0];   // None
    }

    /* OLD: Keep as an example of strcmp and normal_dist
    if (strcmp(atmDens, "Substantial") == 0) {
        std::normal_distribution<double> distribution(0.01, 0.25);
        pressure = abs(distribution(generator) * 1e6);
    }
    // Keep because these gave nice numbers when seed = 10, range of ~0.02 - 4.0 AU
    printf("Semi-Major Axis: %.2f AU\n", distribution(generator));
    printf("Semi-Major Axis: %.2f AU\n", distribution(generator));
    printf("Semi-Major Axis: %.2f AU\n", distribution(generator));
    printf("Semi-Major Axis: %.2f AU\n", distribution(generator));
    printf("Semi-Major Axis: %.2f AU\n", distribution(generator));
    */
}

