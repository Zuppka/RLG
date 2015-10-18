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

// Initialize necessary files and folders
void Rezman::init() {
    makeDir(datapath);    // Datapath directory "resources"
    makeFile(indexPath);  // Index file location "index.json"
    // Do a file check here to ensure files listed in index exist
}

// Read existing resources and choose one based on input parameters
void Rezman::genRezPath(Rezman* r) {
    std::string path;
    r->type[0] = datapath;

    // Randomize the first resource subtype if not set
    if (r->num[0] == -1)
        r->num[0] = rng(0, cat1.size() - 1);
    r->type[1] = cat1[r->num[0]];                // Store name of first type
    path = r->type[0] + "\\" + r->type[1];  // Append datapath and num1
    // Randomize the second resource subtype if not set
    if (r->num[1] == -1)
        r->num[1] = rng(0, cat2[r->num[0]].size() - 1);
    makeDir(path);
    r->type[2] = cat2[r->num[0]][r->num[1]];  // Store name of second type
    path += "\\" + r->type[2];      // Append num2
    // Condition if "Chemical" type and either "Petrochemical" or "Compound" subtype
    if (r->num[0] == 1 && (r->num[1] >= 0 && r->num[1] <= 1)) {
        // Randomize third resource subtype if not set (Chemical)
        if (r->num[2] == -1)
            r->num[2] = rng(0, cat3B[r->num[1]].size() - 1);
        makeDir(path);
        r->type[3] = cat3B[r->num[1]][r->num[2]]; // Store name of third type
        path += "\\" + r->type[3];      // Append num3
    }
    // Condition if "Flora" type and either "Cereal", "Fruit" or "Vegetable" subtype
    else if (r->num[0] == 2 && (r->num[1] >= 0 && r->num[1] <= 2)) {
        // Randomize third resource subtype if not set (Flora)
        if (r->num[2] == -1)
            r->num[2] = rng(0, cat3C[r->num[1]].size() - 1);
        makeDir(path);
        r->type[3] = cat3C[r->num[1]][r->num[2]]; // Store name of third type
        path += "\\" + r->type[3];      // Append num3
    }
    path += ".json";

    // Create the path if it does not exist
    makeFile(path);
    r->rezPath = path;
}

// Copy resource data to JSON object and save both Index and Data file
bool Rezman::saveRezInfo(Rezman r) {
    Fileman tmp;
    int si, sd;

    // Read existing index file
    tmp.readFile(indexPath);
    si = tmp.data.size();
    tmp.data[si]["RezID"] = si;
    tmp.data[si]["Name"] = r.rezName;
    tmp.data[si]["Path"] = r.rezPath;
    // Write index file
    tmp.writeFile(indexPath);

    // Read existing data file
    tmp.readFile(r.rezPath);
    sd = tmp.data.size();
    // Append to end of list
    tmp.data[sd]["RezID"] = si;
    tmp.data[sd]["Name"] = r.rezName;
    tmp.data[sd]["Type1"] = r.type[1];
    tmp.data[sd]["Type2"] = r.type[2];
    tmp.data[sd]["Type3"] = r.type[3];
    tmp.data[sd]["Attr0"] = r.attr[0];
    tmp.data[sd]["Attr1"] = r.attr[1];
    tmp.data[sd]["Attr2"] = r.attr[2];
    // Write data file
    tmp.writeFile(r.rezPath);
    printf("\"%s\" successfully created!\n", r.rezName.c_str());
    return false;
}

// Load resource information from file by ID
bool Rezman::loadRezInfo(Rezman* r, int id) {
    Fileman tmp;
    int si, sd;

    // Read existing index file
    tmp.readFile(indexPath);
    si = tmp.data.size();
    // Check if provided ID is not less than 0 or greater than the number of entries
    if (id < 0 || id >= si) {
        printf("ID not found!\n");
        return true;
    }
    r->rezPath = tmp.data[id]["Path"].asString();   // Read resource path from index

    // Read the data file using path from index
    tmp.readFile(r->rezPath);
    sd = tmp.data.size();
    // Parse the resource file searching for the specified ID
    for (int i = 0; i <= sd; i++) {
        if (tmp.data[i]["RezID"].asInt() == id) {
            // Save resource properties on match
            r->rezID = tmp.data[i]["RezID"].asInt();
            r->rezName = tmp.data[i]["Name"].asString();
            r->type[1] = tmp.data[i]["Type1"].asString();
            r->type[2] = tmp.data[i]["Type2"].asString();
            r->type[3] = tmp.data[i]["Type3"].asString();
            r->attr[0] = tmp.data[i]["Attr0"].asInt();
            r->attr[1] = tmp.data[i]["Attr1"].asInt();
            r->attr[2] = tmp.data[i]["Attr2"].asInt();
            return false;
        }
    }
    printf("ERROR: Something unexpected happened reading resource!\n");
    return true;
}

// Print resource info in a nice, organized way
bool Rezman::printRezInfo (int id) {
    Rezman r;

    // Load the resource by ID
    if (loadRezInfo(&r, id))
        return true;
    // Print resource type in a graphical tree structure
    printf("\n");
    printf("|___%s\n", r.type[1].c_str());
    printf("     |___%s\n", r.type[2].c_str());
    // Print additional subtypes if resource uses them
    if (!r.type[3].empty()) {
        printf("          |___%s\n", r.type[3].c_str());
        printf("               |___%s\n\n", r.rezName.c_str());
    }
    else
        printf("          |___%s\n\n", r.rezName.c_str());
    // Print name and attribute values
    printf("Name:      %s (%d)\n", r.rezName.c_str(), r.rezID);
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
    sd = tmp.data.size();

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
        // Return if matching name is found
        if (existName == name)
            return true;
    }
    return false; // No match found
}

// Automatically create resource entities
void Rezman::genRezAuto(int num1, int num2, int num3) {
    Rezman r;

    // Create a random name and check that it does not already exist
    r.rezName = randName();
    while (chekRezName(r.rezName))
        r.rezName = randName();
    // Save resource path
    r.num[0] = num1;
    r.num[1] = num2;
    r.num[2] = num3;
    r.genRezPath(&r);

    // Assign attribute values
    r.attr[0] = rng(0, 1000);
    r.attr[1] = rng(0, 1000);
    r.attr[2] = rng(0, 1000);

    // Save resource
    saveRezInfo(r);
}

// Manually create resource entities
void Rezman::genRezManual() {
    Rezman r;
    int num1, num2, num3, arraySize;

    printf("\nEntering manual generation.\n");
    // Obtain a resource name
    printf("Enter resource name: ");
    getline(std::cin, r.rezName);
    // Check if the name is empty or already in use
    while (chekRezName(r.rezName) || r.rezName.empty()) {
        printf("\nName already in use or blank name entered.\n");
        printf("Enter resource name: ");
        getline(std::cin, r.rezName);
    }

    // Type 1 - Mineral, Chemical, Flora, Fauna
    printf("\nSelect a type.\n");
    arraySize = cat1.size();
    printList(cat1, arraySize);

    num1 = checkValue(0, arraySize - 1);
    r.num[0] = num1;
    // Type 2
    printf("\nSelect a subtype.\n");
    arraySize = cat2[num1].size();
    printList(cat2[num1], arraySize);
    num2 = checkValue(0, arraySize - 1);
    r.num[1] = num2;
    // Type 3
    if (num1 == 1 && (num2 >= 0 && num2 <= 1)) {
        printf("\nSelect a subtype.\n");
        arraySize = cat3B[num2].size();
        printList(cat3B[num2], arraySize);
        num3 = checkValue(0, arraySize - 1);
        r.num[2] = num3;
    }
    else if (num1 == 2 && (num2 >= 0 && num2 <= 2)) {
        printf("\nSelect a subtype.\n");
        arraySize = cat3C[num2].size();
        printList(cat3C[num2], arraySize);
        num3 = checkValue(0, arraySize - 1);
        r.num[2] = num3;
    }

    genRezPath(&r);

    // Assign values of attributes
    printf("\nAttribute1\n");
    r.attr[0]  = checkValue(0, 1000);
    printf("\nAttribute2\n");
    r.attr[1]  = checkValue(0, 1000);
    printf("\nAttribute3\n");
    r.attr[2]  = checkValue(0, 1000);

    // Save resource
    saveRezInfo(r);
}
