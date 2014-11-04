#ifndef FILEMAN_H
#define FILEMAN_H

#include <json/json.h>

// JSON storage vars
extern Json::Value data, index;
extern int sizeIndex, sizeData;
extern const char *filepath[6], *datapath, *filename[6];

class Fileman {

private:

public:


    Fileman(int) {}
    size_t init ();
    size_t getTypeByID (size_t);
    size_t writeData (size_t);
    size_t readIndex ();
    size_t readData (size_t);
    void displayData (size_t);
};

#endif // FILEMAN_H
