/*
File Manager for file Input/Output
Date: Sept-20-2014

Desc: Controls creating necessary files, reading, writing and displaying data
*/

#include <fstream>

#include <json/json.h>
#include <fileman.h>
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>

const char *filepath[6], *datapath = "database";
const char *filename[6] = {"db_index.json", "db_dwarfPlanets.json", "db_planets.json", "db_gasGiants.json", "db_stars.json", "db_compactObj.json"};

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
    else
        printf("DEBUG: Blank INDEX file read.\n");

    indexFile.close();
    return 0;
}

// Read data from any of the entity database files
size_t Fileman::readData (size_t type) {
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
        data.clear();   // Clear and reset data to default if the file is still blank
        sizeData = 0;
        printf("DEBUG: Blank DATA file read.\n");
    }

    dataFile.close();
	return 0;
}

// Append to end of JSON file
size_t Fileman::writeData (size_t type) {
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

    return 0;
}

// Search the index array for matching ID and return the type
size_t Fileman::getTypeByID (size_t id) {
    //readIndex(); // Assuming index is already loaded?
    // Return if requested ID is out of bounds of index
    if (id < 0 || id > sizeIndex)
        return -1;

    for (size_t i = 0; i <= sizeIndex; i++) {
        if (index[i]["entityID"].asUInt() == id)
            return index[i]["type"].asInt();
    }
    return -1;
}

// Print data to screen
void Fileman::displayData (size_t entID) {
    //getTypeByID(); // Read index file only, scan for ID, and grab type
    size_t type = getTypeByID(entID);
    if (type > 0)
        readData(type);
   /*
        p_dist = data[entID]["p_dist"].asDouble();
        p_mass = data[entID]["p_mass"].asDouble();
        p_gravity = data[entID]["p_gravity"].asDouble();
        p_period = data[entID]["p_period"].asDouble();
        p_solarConst = data[entID]["p_solarConst"].asDouble();
        p_coreComp = data[entID]["p_coreComp"].asCString();
        p_atmDens = data[entID]["p_atmDens"].asCString();
        p_atmComp = data[entID]["p_atmComp"].asCString();
        p_pressure = data[entID]["p_pressure"].asDouble();
        p_temp = data[entID]["p_temp"].asDouble();

        std::ofstream output;
        output.open ("output.txt", std::ofstream::app);
        output << "Description: The planet orbits a star of luminosity " << s_luminosity / SOL_LUMINOSITY << " L_sol and mass of " << s_mass / SOL_MASS << " M_sol. ";
        output << "The planet's semi-major axis is " << p_dist / AU << " AU and has a mass of " << p_mass / EARTH_MASS << " M_earth with a gravitational acceleration of " << p_gravity / EARTH_GRAV << " G. ";
        output << "It takes the planet " << p_period << " days to orbit its star. The solar constant at the plant is " << p_solarConst << " W / m^2. ";
        output << "The plant has a " << p_coreComp << "  core. The " << p_atmif (type == 2) {
        p_dist = data[entID]["p_dist"].asDouble();Dens << " atmosphere composed primarily of " << p_atmComp << " and the pressure is about " << p_pressure / 1e5 << " bar. ";
        output << "The average surface temperature of the planet is " << p_temp << " K (" << p_temp - 273 << " C).\n\n";
        output.close();
   */
    else
        printf("Description not set!\n\n");
}
