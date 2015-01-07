/*
File Manager for file Input/Output
Date: Sept-20-2014

Desc: Controls creating necessary files, reading, writing and displaying data
*/

#include <fileman.h>
#include <json/json.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>

#include <cstdio>
#include <cstring>
#include <fstream>

const char *filepath[6], *datapath = "database";
const char *filename[6] = {"00_index.json", "01_stars.json", "02_planets.json", "03_moons.json", "04_minorPlanets.json", "05_other.json"};

// Initialize necessary files and folders
size_t Fileman::init () {
    int len = strlen(datapath) + 2, arraySize = sizeof(filename) / sizeof(filename[0]);
    struct stat info;
    char *tmpPath;

    // Check if database directory already exists
    if (stat (datapath, &info) != 0) {
        printf("Creating directory %s\n", datapath);
        CreateDirectory(datapath, NULL);
    }
    // Create database files
    for (int i = 0; i < arraySize; i++) {
        // Setup the name of the file path
        tmpPath = new char[len + strlen(filename[i])];
        strcpy(tmpPath, datapath);
        strcat(tmpPath, "\\");
        strcat(tmpPath, filename[i]);
        filepath[i] = tmpPath;

        // Check if file doesn't already exist
        if (!std::ifstream (filepath[i])) {
            printf("Creating file %s...\n", filepath[i]);
            std::ofstream tmpfile(filepath[i]);
        }
    }
    return 0;
}

// Read only the index database. Useful for looking up type of entity by ID
size_t Fileman::readIndex () {
    if (debugLvl)
        printf("DEBUG: Reading INDEX file.\n");
    std::ifstream indexFile;
    Json::Value parsedFromString;
    Json::Reader reader;
    bool parsingSuccessful;

    // Open an index file that stores all entities
    indexFile.open (filepath[0], std::ifstream::binary);
     // Check if file can be opened
    if (!indexFile.is_open()) {
       printf("ERROR: Read error opening %s!\n", filepath[0]);
       return 1;
    }

    // Read index
    parsingSuccessful = reader.parse(indexFile, parsedFromString);
    if (parsingSuccessful) {
        index = parsedFromString;
        sizeIndex = index.size();
    }
    else {
        if (debugLvl)
            printf("DEBUG: Blank INDEX file read.\n");
    }
    indexFile.close();
    return 0;
}

// Read data from any of the entity database files
size_t Fileman::readData (size_t type) {
    if (debugLvl)
        printf("DEBUG: Reading DATA of type: %d\n", type);
    std::ifstream dataFile;
    Json::Value parsedFromString;
    Json::Reader reader;
    bool parsingSuccessful;

    // Open file for reading values based on entity type
    dataFile.open (filepath[type], std::ifstream::binary);
    // Check if file can be opened
    if (!dataFile.is_open()) {
       printf("ERROR: Error opening %s of type %d!\n", filepath[type], type);
       return 1;
    }

	// Copy contents of file to memory if file has content
    parsingSuccessful = reader.parse(dataFile, parsedFromString);
    if (parsingSuccessful) {
        data = parsedFromString;
        sizeData = data.size();
    }
    else {
        if (debugLvl)
            printf("DEBUG: Blank DATA file read.\n");
    }

    dataFile.close();
	return 0;
}

// Append to end of JSON file
size_t Fileman::writeData (size_t type) {
    if (debugLvl)
        printf("DEBUG: Writing INDEX and DATA files.\n");
    // Create variables used in JSON I/O
    std::ofstream indexFile, dataFile;
    Json::StyledWriter styledWriter;

    // Open index
    indexFile.open (filepath[0], std::ofstream::out);
    if (!indexFile.is_open()) {
        printf("ERROR: Error opening %s!\n", filepath[0]);
        return 1;
    }

    // Open file for storing values based on entity type
    dataFile.open (filepath[type], std::ofstream::out);
    if (!dataFile.is_open()) {
        printf("ERROR: Error opening %s!\n", filepath[type]);
        return 1;
    }

    // Add entity entry and entity type to the index database
    index[sizeIndex]["entityID"] = sizeIndex;
    index[sizeIndex]["type"] = type;
    // Write to file in a readable way
    indexFile << styledWriter.write(index);
    dataFile << styledWriter.write(data);

    indexFile.close();
    dataFile.close();
    index.clear();
    data.clear();

    return 0;
}
