// RLG Project 2015

#ifndef TILEMAN_H
#define TILEMAN_H

#define PI 3.141592
#define EARTH_RADIUS 6.371e+6		// m

#include <string>

// Tile Manager class
class Tileman {
	private:

	public:
	    // Path management variables
        const char *datapath = "maps";
        std::string path = "", indexPath = "";
        // Tile properties
	    int xPos, yPos;
        int r, theta, phi;

        void init();
        bool genTiles(int);
        void popTiles();
        void printTileInfo(int id);
        void setTile(int, int);
};

#endif // TILEMAN_H
