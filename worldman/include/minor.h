// RLG Project 2015

#ifndef MINOR_H
#define MINOR_H

#include "worldman.h"

// Minor planets and bodies creation header
class Minor : public Worldman {
    private:

    public:
        void events();
        void create(size_t);
};

#endif // MINOR_H
