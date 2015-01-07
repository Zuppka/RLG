#ifndef FILEMAN_H
#define FILEMAN_H

#include <json/json.h>

class Fileman {

    private:
        int debugLvl = 0;
    public:
        // JSON storage vars
        int sizeIndex, sizeData;
        Json::Value data, index;

        Fileman() {}
        size_t init ();
        size_t writeData (size_t);
        size_t readIndex ();
        size_t readData (size_t);
};

#endif // FILEMAN_H
