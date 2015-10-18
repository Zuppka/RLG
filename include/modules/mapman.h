// RLG Project 2015

#ifndef MAPMAN_H
#define MAPMAN_H

#define PI 3.141592
#define EARTH_RADIUS 6.371e+6	// m

#include <json/json.h>

#include <string>

// Map Tile Manager class
class Mapman {
	private:

	public:
	    // Path management variables
        std::string datapath = "maps", indexPath = datapath + "\\index.json";
        // Map tile properties
        std::string entName, entComp;
        Json::Value mapRez, mapRezValue, gridA, gridB;
        int entID, entLife, r = 0, theta = 0, phi = 0, xB = 0, yB = 0, seed, gridSize;

        void init();
        bool checkIndex (int);
        void chooseRez(Mapman*, int, int, int);
        void makeRezSet(Mapman);

        void saveMapData(Mapman);
        bool loadMapData(int, Mapman*);
        bool genMap(int);
        bool genTile(int);
        void printTileInfo(int);
};

#endif // MAPMAN_H
