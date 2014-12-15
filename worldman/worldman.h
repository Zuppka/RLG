#ifndef WORLDMAN_H
#define WORLDMAN_H

/* Bunch of constants */
#define LEN 32
#define PI 3.141592
#define G 6.67384e-11 				// N·(m/kg)^2
#define SOL_RADIUS 6.96e+8 		    // m
#define SOL_TEMP 5778				// K
#define SOL_MASS 1.9885e+30 		// kg
#define SOL_LUMINOSITY 3.846e+26 	// W
#define EARTH_MASS 5.9726e+24		// kg
#define EARTH_RADIUS 6.371e+6		// m
#define EARTH_GRAV 9.80665 			// m/s^2
#define EARTH_MAG_FIELD 0.31		// gauss (31 micro Tesla)
#define SIGMA 5.67040e-8 			// J/s-m^2-K^4
#define AU 1.49597e+11 				// m

// World Manager class
class Worldman {
    private:

    public:
        Worldman () {}
        void init ();
        size_t checkTypebyID (size_t, size_t);
        size_t getLastID ();
        size_t createStarSys ();
};

#endif // WORLDMAN_H
