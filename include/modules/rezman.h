// RLG Project 2015

#ifndef REZMAN_H
#define REZMAN_H

#include <string>
#include <vector>

// Resource Manager class
class Rezman {
    private:

	public:
	    // File path variables
	    std::string datapath = "resources", indexPath = datapath + "\\index.json";
        // Resource classification
        std::vector <std::string> cat1 = {"Mineral", "Chemical", "Flora", "Fauna"};
        std::vector <std::vector <std::string>> cat2 = {
            {"Native", "Sulfides", "Halides", "Oxides", "Carbonates", "Borates", "Sulfates", "Phosphates", "Silicates", "Organic Minerals"},
            {"Petrochemical", "Compounds", "Monatomic Gas", "Diatomic Gas"},
            {"Cereals", "Fruit", "Vegetables", "Soft Wood", "Hard Wood"},
            {"Hide", "Bone", "Meat", "Fibre"}
        };
        std::vector <std::vector <std::string>> cat3B = {
            {"Alkane", "Cycloalkane", "Aromatic"},
            {"Water", "Acids", "Bases"}
        };
        std::vector <std::vector <std::string>> cat3C = {
            {"Wheat", "Millet", "Rice", "Maize"},
            {"Flowering Fruit", "Berry", "Nut", "Citrus"},
            {"Greens", "Root Vegetable", "Gourd", "Beans"}
        };
        // Resource properties
        std::string rezName, rezPath, type[5] = {"", "", "", "", ""};
        int rezID, attr[10], num[3];

        void init();
        void genRezPath(Rezman*);
        bool saveRezInfo(Rezman);
        bool loadRezInfo(Rezman*, int);
        bool printRezInfo(int);
        bool chekRezName(std::string);
        void genRezAuto(int, int, int);
        void genRezManual();
};

#endif // REZMAN_H
