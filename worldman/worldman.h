#ifndef WORLDMAN_H
#define WORLDMAN_H

/* Bunch of constants */
#define LEN 32
#define PI 3.141592
#define G 6.6748e-11 				// N·(m/kg)2
#define SOL_RADIUS 6.955e+8 		    // m
#define SOL_TEMP 5778				// K
#define SOL_MASS 1.988e+30 			// x10^30 kg
#define SOL_LUMINOSITY 3.846e+26 	// W
#define EARTH_MASS 5.972e+24		// kg
#define EARTH_RADIUS 6.371e+6		// m
#define EARTH_GRAV 9.80 			// m/s^2
#define EARTH_MAG_FIELD 0.31		// gauss (31 micro Tesla)
#define SIGMA 5.670400e-8 			//x10^-8 J/s-m^2-K^4
#define AU 1.49597e+11 				// m

// JSON storage vars
extern size_t sizeIndex, sizeData;
// Planet variables
extern const char *p_atmComp, *p_atmDens, *p_coreComp;
extern double p_radius, p_dist, p_period, p_mass, p_solarConst, p_pressure, p_temp, p_gravity, p_escVel;
extern double p_albedo, p_emissivity, p_emissCloud, p_emissSurf, p_greenhouseEff, p_cloudCover, p_cloudFactor, p_surfWater, p_surfIce, p_boilTemp, p_age, p_ageLife;
extern size_t p_lifeCheck, p_hasLife, p_magField, p_starID;
// Star variables
extern double s_radius, s_mass, s_luminosity, s_intensity, s_temp, s_habitable_min, s_habitable_max, s_age;

class Worldman {
    private:

    public:
        Worldman(size_t) {}
        void init ();
        size_t checkTypebyID (size_t, size_t);
        size_t getLastID ();
        size_t createEntity (size_t, size_t, int);
};

#endif // WORLDMAN_H
