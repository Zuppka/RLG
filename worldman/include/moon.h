// RLG Project 2015

#ifndef MOON_H
#define MOON_H

#include "worldman.h"
#include "planet.h"

// Moon and natural satellite creation header
class Moon : public Worldman {
    private:

    public:
        // Moon variables

        void events();
        void create(size_t, const Planet&);
};

#endif // MOON_H
