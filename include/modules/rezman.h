// RLG Project 2015

#ifndef REZMAN_H
#define REZMAN_H

#include <string>

// Resource Manager class
class Rezman {
    private:

	public:
	    // File path variables
	    const char *datapath = "resources";
        std::string path = "", indexPath = "";
        // Resource classification
        std::string cat1[4] = {"Mineral", "Chemical", "Flora", "Fauna"};
        std::string cat2[4][10] = {{"Native", "Sulfides", "Halides", "Oxides", "Carbonates", "Borates", "Sulfates", "Phosphates", "Silicates", "Organic Minerals"},
                                    {"Petrochemical", "Compounds", "Monatomic Gas", "Diatomic Gas"},
                                    {"Cereals", "Fruit", "Vegetables", "Soft Wood", "Hard Wood"},
                                    {"Hide", "Bone", "Meat", "Fibre"}};
        std::string cat3B[2][3] = {{"Alkane", "Cycloalkane", "Aromatic"},
                                    {"Water", "Acids", "Bases"}};
        std::string cat3C[3][4] = {{"Wheat", "Millet", "Rice", "Maize"},
                                    {"Flowering Fruit", "Berry", "Nut", "Citrus"},
                                    {"Greens", "Root Tuber", "Gourd", "Beans"}};
        // Resource properties
        std::string type[5] = {"", "", "", "", ""};
        std::string name;
	    int rezID, attr[10];

        void init();
        void pathName(int);
        bool saveRezInfo();
        bool loadRezInfo(int);
        bool printRezInfo(int);
        bool chekRezName(std::string);
        void generateResource();
        void generateManual();
};

#endif // REZMAN_H
