#include <stdlib.h>
#include <stdio.h>

#include "tileman.h"

Tileman::Tileman();

void Tileman::setTile(int x, int y) {
	xPos = x;
	yPos = y;
}

int main(int argc, char *argv[]) {
	Tileman big;
	int x, y;

	printf ("Enter tile x, y");
	scanf ("%d, %d", &x, &y);

	big.setTile(x, y);
	printf ("xPos: %d, yPos: %d\n", big.xPos, big.yPos);

    return 0;
}
