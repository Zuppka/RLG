// RLG Project 2015

#ifndef TILEMAN_H
#define TILEMAN_H

#define PI 3.141592
#define EARTH_RADIUS 6.371e+6		// m

// Tile Manager class
class Tileman {
	private:

	public:
	    int xPos, yPos;
        int r, theta, phi;

        void populateTile();
        void setTile(int, int);
};

#endif // TILEMAN_H
