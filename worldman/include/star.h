// RLG Project 2015

#ifndef STAR_H
#define STAR_H

#include "worldman.h"

// Star creation header
class Star : public Worldman {
    private:

    public:
        // Star variables
        double luminosity, habitable_min, habitable_max;
        int numStars;

        void events();
        void create(size_t, const Star&);
};

#endif // STAR_H
