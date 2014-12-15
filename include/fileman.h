#ifndef FILEMAN_H
#define FILEMAN_H

#include <json/json.h>

class Fileman {

    private:

    public:
        // JSON storage vars
        size_t sizeIndex, sizeData;
        Json::Value data, index;

        Fileman() {}
        size_t init ();
        size_t getTypeByID (size_t);
        size_t writeData (size_t);
        size_t readIndex ();
        size_t readData (size_t);
        void displayData (size_t);
};

#endif // FILEMAN_H
