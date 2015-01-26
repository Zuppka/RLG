/*
Resource Manager Main
Date: Jan-7-2015

Desc: Generates resources and manages resource attributes

*/

#include <modules\rezman.h>
#include <modules\toolman.h>
#include <modules\fileman.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

Rezman r;

// Initialize necessary files and folders
void Rezman::init() {

    Fileman file;
    file.makeDir(datapath);
    // Index file location
    indexPath += datapath;
    indexPath += "\\index.json";
    file.makeFile(indexPath);
}

// Iterative function that creates the path name using entity types
void Rezman::pathName(int curr) {
    Fileman tmp;
    int next = curr + 1;

    path += r.type[curr];
    // Check if the following subtype is blank
    if (r.type[next].empty()) {
        path += ".json";
        tmp.makeFile(path); // Create resource file
    }
    // Append path, create sub folder and call self
    else {
        tmp.makeDir(path);
        path += "\\";
        pathName(next);
    }
}

// Copy resource data to JSON object and save both Index and Data file
bool Rezman::saveRezInfo() {
    Fileman tmp;
    int si, sd;

    tmp.readFile(indexPath); // Read existing index file
    si = tmp.sizeData;
    tmp.data[si]["RezID"] = si;
    tmp.data[si]["Name"] = r.name;
    tmp.data[si]["Path"] = path;

    tmp.writeFile(indexPath); // Read existing index file

    tmp.readFile(path); // Read existing data file
    sd = tmp.sizeData; // Get number of existing entities before appending

    tmp.data[sd]["RezID"] = si;
    tmp.data[sd]["Name"] = r.name;
    tmp.data[sd]["Type1"] = r.type[1];
    tmp.data[sd]["Type2"] = r.type[2];
    tmp.data[sd]["Type3"] = r.type[3];
    tmp.data[sd]["Attr0"] = r.attr[0];
    tmp.data[sd]["Attr1"] = r.attr[1];
    tmp.data[sd]["Attr2"] = r.attr[2];

    tmp.writeFile(path);
    path = "";  // Reset the path
    printf("\"%s\" successfully created!\n", r.name.c_str());
    return false;
}

// Load resource information from file by ID
bool Rezman::loadRezInfo(int id) {
    Fileman tmp;

    tmp.readFile(indexPath); // Read existing index file
    if (id < 0 || id > tmp.sizeData) {
        printf("ID not found!\n");
        return true;
    }
    path = tmp.data[id]["Path"].asString();

    // Read the data file using path from index
    tmp.readFile(path);

    for (int i = 0; i <= tmp.sizeData; i++) {
        if (tmp.data[i]["RezID"].asInt() == id) {
            r.rezID = tmp.data[i]["RezID"].asInt();
            r.name = tmp.data[i]["Name"].asString();
            r.type[1] = tmp.data[i]["Type1"].asString();
            r.type[2] = tmp.data[i]["Type2"].asString();
            r.type[3] = tmp.data[i]["Type3"].asString();

            r.attr[0] = tmp.data[i]["Attr0"].asInt();
            r.attr[1] = tmp.data[i]["Attr1"].asInt();
            r.attr[2] = tmp.data[i]["Attr2"].asInt();
            path = "";  // Reset the path

            return false;
        }
    }
    return true;
}

// Print resource info in a nice, organized way
bool Rezman::printRezInfo (int id) {
    if (loadRezInfo(id))
        return true;
    printf("\n");
    printf("|___%s\n", r.type[1].c_str());
    printf("     |___%s\n", r.type[2].c_str());
    // Print additional subtypes if resource uses them
    if (!r.type[3].empty()) {
        printf("          |___%s\n", r.type[3].c_str());
        printf("               |___%s\n\n", r.name.c_str());
    }
    else
        printf("          |___%s\n\n", r.name.c_str());
    // Print name and attribute values
    printf("Name:      %s (%d)\n", r.name.c_str(), r.rezID);
    printf("Attr1:     %d\n", r.attr[0]);
    printf("Attr2:     %d\n", r.attr[1]);
    printf("Attr3:     %d\n", r.attr[2]);

    return false;
}

// Check  names in index and compare to string provided to function
bool Rezman::chekRezName(std::string name) {
    Fileman tmp;
    int sd;
    std::string existName;

    tmp.readFile(indexPath); // Read existing index file
    sd = tmp.sizeData;

    // Convert the input string to lower case for comparison
    for (size_t j = 0; j < strlen(name.c_str()); j++) {
        name[j] = tolower(name[j]);
    }

    // Loop through all entries
    for (int i = 0; i < sd; i++) {
        existName = tmp.data[i]["Name"].asCString();
        // Convert the data string to lower case for comparison
        for (size_t j = 0; j < strlen(existName.c_str()); j++) {
            existName[j] = tolower(existName[j]);
        }
        if (existName == name)
            return true;
    }
    return false; // No match found

}

// Automatically create resource entities
void Rezman::generateResource() {
    int num, num2, num3, arraySize;
    // Reset resource types string array
    for(int i = 0; i <= 4; i++)
        r.type[i].clear();
    // Create a random name and check that it does not already exist
    r.name = randName();
    while (chekRezName(r.name))
        r.name = randName();

    // Type 0 - Datapath
    r.type[0] = datapath;
    // Type 1 - Mineral, Chemical, Flora, Fauna
    arraySize = sizeof(r.cat1) / sizeof(r.cat1[0]);
    num = rng(0, arraySize - 1);
    r.type[1] = r.cat1[num];    // Assign type 1
    // Type 2
    if (r.type[1] == "Mineral")
        num2 = rng(0, 9);
    else if (r.type[1] == "Chemical")
        num2 = rng(0, 3);
    else if (r.type[1] == "Flora")
        num2 = rng(0, 4);
    else if (r.type[1] == "Fauna")
        num2 = rng(0, 3);
    r.type[2] = r.cat2[num][num2];  // Assign type 2
    // Type 3
    if (r.type[2] == "Petrochemical" || r.type[2] == "Compounds") {
        arraySize = sizeof(r.cat3B[num2]) / sizeof(r.cat3B[num2][num2]);
        num3 = rng(0, arraySize - 1);
        r.type[3] = r.cat3B[num2][num3];    // Assign type 3
    }
    else if (r.type[2] == "Cereals" || r.type[2] == "Fruit" || r.type[2] == "Vegetables") {
        arraySize = sizeof(r.cat3C[num2]) / sizeof(r.cat3C[num2][num2]);
        num3 = rng(0, arraySize - 1);
        r.type[3] = r.cat3C[num2][num3];    // Assign type 3
    }
    // Assign attribute values
    r.attr[0] = rng(0, 1000);
    r.attr[1] = rng(0, 1000);
    r.attr[2] = rng(0, 1000);
    // Assign a path name and save resource
    pathName(0);
    saveRezInfo();
}

// Manually create resource entities
void Rezman::generateManual() {
    int num, num2, num3, arraySize;
    // Reset resource types string array
    for(int i = 0; i <= 4; i++)
        r.type[i].clear();

    printf("\nEntering manual generation.\n");
    // Obtain a resource name
    printf("Enter resource name: ");
    getline(std::cin, r.name);
    // Check if the name is empty or already in use
    while (chekRezName(r.name) || r.name.empty()) {
        printf("\nName already in use or blank name entered.\n");
        printf("Enter resource name: ");
        getline(std::cin, r.name);
    }

    // Type 0 - Datapath
    r.type[0] = datapath;
    // Type 1 - Mineral, Chemical, Flora, Fauna
    printf("\nSelect a type.\n");
    printList(r.cat1, 4);

    arraySize = sizeof(r.cat1) / sizeof(r.cat1[0]);
    num = checkValue(0, arraySize - 1);
    r.type[1] = r.cat1[num];    // Assign type 1
    // Type 2
    if (r.type[1] == "Mineral") {
        printf("\nSelect a subtype.\n");
        printList(r.cat2[num], 10);
        num2 = checkValue(0, 9);
    }
    else {
        printf("\nSelect a subtype.\n");
        printList(r.cat2[num], 4);
        num2 = checkValue(0, 3);
    }
    r.type[2] = r.cat2[num][num2];  // Assign type 2
    // Type 3
    if (r.type[2] == "Petrochemical" || r.type[2] == "Compounds") {
        printf("\nSelect a subtype.\n");
        printList(r.cat3B[num2], arraySize - 1);
        arraySize = sizeof(r.cat3B[num2]) / sizeof(r.cat3B[num2][num2]);
        num3 = checkValue(0, arraySize - 1);
        r.type[3] = r.cat3B[num2][num3];    // Assign type 3
    }
    else if (r.type[2] == "Cereals" || r.type[2] == "Fruit" || r.type[2] == "Vegetables") {
        printf("\nSelect a subtype.\n");
        printList(r.cat3C[num2], arraySize - 1);
        arraySize = sizeof(r.cat3C[num2]) / sizeof(r.cat3C[num2][num2]);
        num3 = checkValue(0, arraySize - 1);
        r.type[3] = r.cat3C[num2][num3];    // Assign type 3
    }

    printf("\nAttribute1\n");
    r.attr[0]  = checkValue(0, 1000);
    printf("\nAttribute2\n");
    r.attr[1]  = checkValue(0, 1000);
    printf("\nAttribute3\n");
    r.attr[2]  = checkValue(0, 1000);

    // Assign a path name and save resource
    pathName(0);
    saveRezInfo();
}
