// RLG Project 2015

#ifndef FILEMAN_H
#define FILEMAN_H

#include <json/json.h>

#include <string>

bool makeDir(std::string);
bool makeFile(std::string);

class Fileman {
    public:
        Json::Value data;   // Store the JSON data
        bool readFile(std::string);
        bool writeFile(std::string);
    private:
        int debugLvl = 0;
};

#endif // FILEMAN_H
