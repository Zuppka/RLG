/*
World Manager File Input/Output
Date: Sept-20-2014

Desc: Controls creating necessary files, reading, writing and displaying data
*/

#include <fstream>

#include <json/json.h>
#include <worldman.h>
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>

// Arrays for storing file data and data position pointers
Json::Value data, index;
int sizeIndex, sizeData;

const char *filename[6] = {"database\\db_index.json", "database\\db_dwarfPlanets.json", "database\\db_planets.json", "database\\db_gasGiants.json", "database\\db_stars.json", "database\\db_compactObj.json"};

// Initialize
size_t Worldman::init () {
    int arraySize = sizeof(filename) / sizeof(filename[0]);
    struct stat info;
    const char *pathname = "database";

    // Check if database directory already exists
    if (stat (pathname, &info) != 0) {
        printf("Creating directory %s\n", pathname);
        CreateDirectory("database", NULL);
    }
    // Create database files if the do not exist
    for (int i = 0; i < arraySize; i++) {
        if (!std::ifstream (filename[i])) {
            printf("Creating file %s...\n", filename[i]);
            std::ofstream tmpfile(filename[i]);
        }
    }
    return 0;
}

// Read only the index database. Useful for looking up type of entity by ID
size_t Worldman::readIndex () {
    std::ifstream indexFile;
    Json::Value parsedFromString;
    Json::Reader reader;
    bool parsingSuccessful;

    // Open an index file that stores all entities
    indexFile.open (filename[0], std::ifstream::binary);
     // Check if file can be opened
    if (!indexFile.is_open()) {
       printf("Error opening %s!\n", filename[0]);
       return 1;
    }

    // Read index
    parsingSuccessful = reader.parse(indexFile, parsedFromString);
    if (parsingSuccessful) {
        index = parsedFromString;
        sizeIndex = index.size();
    }
    else
        printf("DEBUG: Blank Index file read.\n\n");

    indexFile.close();
    return 0;
}

// Read data from any of the entity database files
size_t Worldman::readData (size_t type) {
    std::ifstream dataFile;
    Json::Value parsedFromString;
    Json::Reader reader;
    bool parsingSuccessful;

    // Open file for reading values based on entity type
    dataFile.open (filename[type], std::ifstream::binary);
    // Check if file can be opened
    if (!dataFile.is_open()) {
       printf("Error opening %s!\n", filename[type]);
       return 1;
    }

	// Copy contents of file to memory if file has content
    parsingSuccessful = reader.parse(dataFile, parsedFromString);
    if (parsingSuccessful) {
        data = parsedFromString;
        sizeData = data.size();
    }
    else
        printf("DEBUG: Blank Data file read.\n\n");

    dataFile.close();
	return 0;
}

// Append to end of JSON file
size_t Worldman::writeData (size_t type) {
    // Create variables used in JSON I/O
    std::ofstream indexFile, dataFile;
    Json::StyledWriter styledWriter;

    // Open index
    indexFile.open (filename[0], std::ofstream::out);
    if (!indexFile.is_open()) {
        printf("Error opening %s!\n\n", filename[0]);
        return 1;
    }

    // Open file for storing values based on entity type
    dataFile.open (filename[type], std::ofstream::out);
    if (!dataFile.is_open()) {
        printf("Error opening %s!\n\n\n", filename[type]);
        return 1;
    }

    if (type == 1) {    // Dwarf Planets
        data[sizeData]["entityID"] = sizeIndex;
    }
	else if (type == 2) {   // Planets
        // Assign values to the content of entry
        data[sizeData]["entityID"] = sizeIndex;
        data[sizeData]["p_radius"] = p_radius;
        data[sizeData]["p_dist"] = p_dist;
        data[sizeData]["p_period"] = p_period;
        data[sizeData]["p_mass"] = p_mass;
        data[sizeData]["p_gravity"] = p_gravity;
        data[sizeData]["p_solarConst"] = p_solarConst;
        data[sizeData]["p_pressure"] = p_pressure;
        data[sizeData]["p_temp"] = p_temp;
        data[sizeData]["p_albedo"] = p_albedo;
        data[sizeData]["p_emissivity"] = p_emissivity;
        data[sizeData]["p_greenhouseEff"] = p_greenhouseEff;
        data[sizeData]["p_coreComp"] = p_coreComp;
        data[sizeData]["p_atmDens"] = p_atmDens;
        data[sizeData]["p_atmComp"] = p_atmComp;
        data[sizeData]["p_age"] = p_age;

	}
    else if (type == 3) {   // Gas Giants
        data[sizeData]["entityID"] = sizeIndex;
	}
    else if (type == 4) {   // Stars
        data[sizeData]["entityID"] = sizeIndex;
        data[sizeData]["s_radius"] = s_radius;
        data[sizeData]["s_mass"] = s_mass;
        data[sizeData]["s_luminosity"] = s_luminosity;
        data[sizeData]["s_temp"] = s_temp;
        data[sizeData]["s_habitable_min"] = s_habitable_min;
        data[sizeData]["s_habitable_max"] = s_habitable_max;
        data[sizeData]["s_age"] = s_age;
	}
    else if (type == 5) {   // Compact objects
        data[sizeData]["entityID"] = sizeIndex;
	}
	else {
        printf("No valid data to write!\n");
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

// Print data to screen
void Worldman::displayData (size_t entID) {
    //getTypeByID(); // Read index file only, scan for ID, and grab type
    size_t type = getTypeByID(entID);
    if (type > 0)
        readData(type);

    if (type == 2) {
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

        printf("Description: The planet orbits a star of luminosity %.2f L_sol and mass of %.2f M_sol. ", s_luminosity / SOL_LUMINOSITY, s_mass / SOL_MASS);
        printf("The planet's semi-major axis is %.3f AU and has a mass of %.2f M_earth with a gravitational acceleration of %.2f G. ",  p_dist / AU, p_mass / EARTH_MASS, p_gravity / EARTH_GRAV);
        printf("It takes the planet %.0f days to orbit its star. The solar constant at the plant is %.0f W / m^2. ", p_period, p_solarConst);
        printf("The plant has a %s core. The %s atmosphere composed primarily of %s and the pressure is about %.3f bar. ", p_coreComp, p_atmDens, p_atmComp, p_pressure / 1e5);
        printf("The average surface temperature of the planet is %.0f K (%.0f C).\n\n", p_temp, p_temp - 273);

        std::ofstream output;
        output.open ("output.txt", std::ofstream::app);
        output << "Description: The planet orbits a star of luminosity " << s_luminosity / SOL_LUMINOSITY << " L_sol and mass of " << s_mass / SOL_MASS << " M_sol. ";
        output << "The planet's semi-major axis is " << p_dist / AU << " AU and has a mass of " << p_mass / EARTH_MASS << " M_earth with a gravitational acceleration of " << p_gravity / EARTH_GRAV << " G. ";
        output << "It takes the planet " << p_period << " days to orbit its star. The solar constant at the plant is " << p_solarConst << " W / m^2. ";
        output << "The plant has a " << p_coreComp << "  core. The " << p_atmDens << " atmosphere composed primarily of " << p_atmComp << " and the pressure is about " << p_pressure / 1e5 << " bar. ";
        output << "The average surface temperature of the planet is " << p_temp << " K (" << p_temp - 273 << " C).\n\n";
        output.close();
    }
    else
        printf("Description not set!\n\n");
}

// Search the index array for matching ID and return the type
size_t Worldman::getTypeByID (size_t id) {
    for (int i = 0; i <= sizeIndex; i++) {
        if (index[i]["entityID"].asUInt() == id)
            return index[i]["type"].asInt();
    }
    return -1;
}
