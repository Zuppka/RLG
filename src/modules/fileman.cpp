/*
File Manager for file Input/Output
Date: Sept-20-2014

Desc: Controls creating necessary files, reading, writing and displaying data
*/

#include <modules/fileman.h>
#include <json/json.h>
#include <sys/stat.h>
#include <windows.h>

#include <cstdio>
#include <fstream>

// Function for checking if a directory exists and creating it
bool makeDir(std::string path) {
    struct stat info;

    // Check if directory already exists
    if (stat (path.c_str(), &info) != 0) {
        printf("Creating directory \"%s\"...\n", path.c_str());
        CreateDirectory(path.c_str(), NULL);
        return true;
    }
    return false;
}

// Function for checking if a file exists and creating it
bool makeFile(std::string name) {
    // Check if file doesn't already exist
    if (!std::ifstream (name)) {
        printf("Creating file \"%s\"...\n", name.c_str());
        std::ofstream tmpfile(name);
        return true;
    }
    return false;
}

// Read from a specified filepath instead of a predefined JSON file
bool Fileman::readFile(std::string path) {
    if (debugLvl)
        printf("DEBUG: Reading from file: %s\n", path.c_str());
    std::ifstream file;
    Json::Value parsedFromString;
    Json::Reader reader;
    bool parsingSuccessful;

    // Open file specified in path
    file.open (path, std::ifstream::binary);
    // Check if file was opened successfully
    if (!file.is_open()) {
       printf("ERROR: Read error opening %s!\n", path.c_str());
       return true;
    }

    // Copy contents of file to memory if file has content
    parsingSuccessful = reader.parse(file, parsedFromString);
    if (parsingSuccessful) {
        data = parsedFromString;
    }
    // If file was opened but no data was read, set size to zero
    else {
        if (debugLvl)
            printf("DEBUG: Blank file read.\n");
    }

    file.close();
    return false;
}

// Write to a specified filepath instead of a predefined JSON file
bool Fileman::writeFile(std::string path) {
    if (debugLvl)
        printf("DEBUG: Writing to file: %s.\n", path.c_str());
    // Create variables used in JSON I/O
    std::ofstream file;
    Json::StyledWriter styledWriter;

    // Open file specified in path
    file.open (path, std::ofstream::out);
    // Check if file was opened successfully
    if (!file.is_open()) {
        printf("ERROR: Error opening %s!\n", path.c_str());
        return true;
    }

    // Write to file in a readable way
    file << styledWriter.write(data);

    file.close();
    data.clear();

    return false;
}
