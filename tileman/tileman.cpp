/*
Tile Manager Main
Date: Jan-7-2015

Desc: Creates and manages a coordinate system for entities with surfaces and various parameters associated
with each location (resource quantities, structures, physical characteristics etc).

*/

#include "include\tileman.h"
#include <fileman.h>

#include <cstdio>

Fileman file;

void Tileman::populateTile() {
    int type = 2;
	double radius, length, sections;

    file.readData(type);
	radius = file.data[0]["p_radius"].asDouble() * EARTH_RADIUS / 1000;
	length = radius * 10 / 360 * 2 * PI;
	sections = (radius * 2 * PI / length);

	printf ("length: %.2f Sections: %.4f\n", length, sections);

}

void Tileman::setTile(int x, int y) {


}


int main() {
    Tileman big;
    int x, y;

	file.init();
    big.populateTile();

	printf ("Enter tile x, y");
	scanf ("%d, %d", &x, &y);

	big.setTile(x, y);
	printf ("xPos: %d, yPos: %d\n", big.xPos, big.yPos);

    return 0;
}
