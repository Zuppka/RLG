#ifndef FILEMAN_H
#define FILEMAN_H

#include <json/json.h>

class Fileman {

    private:
        int debugLvl = 0;
    public:
        // JSON storage vars
        int sizeData = 0;
        Json::Value data;

        Fileman() {}
        bool makeDir(std::string);
        bool makeFile(std::string);
        bool readFile(std::string);
        bool writeFile(std::string);
};

#endif // FILEMAN_H
