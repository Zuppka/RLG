/*
Useful functions and Tools Manager
Date: Jan-19-2014

Desc: Contains various functions used repeatedly throughout the project such
as random number generators, value checking and printing string lists.
*/

#include <toolman.h>

#include <cstdio>
#include <ctime>
#include <iostream>
#include <random>

// Random number generator related properties
//std::random_device rd;
std::mt19937 mt(time(0));   // Bit more advanced randomization

// Random Number Generator - Uniform
int rng(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(mt);
}

// Random Number Generator - Lognormal
double rngLog(double a, double b) {
    // Distribution of orbital distances for planets
    std::lognormal_distribution<double> dist(a, b);
    return dist(mt);
}

// Simple random name generator
std::string randName() {
    char tmp;
    int len = rng(3, 10);
    std::string result = "";

    // Add one random letter at a time until length reached
    for (int i = 0; i < len ; i++) {
        if (i == 0)
            tmp = rng(65, 90);   // Start with random uppercase letter
        else
            tmp = rng(97, 122);   // Randomize new lowercase letter
        result = result + tmp;  // Add letter to result
    }

    return result;
}

// Short function to round doubles to desired precision
double deciTrunc (double value, int precision) {
    double roundValue, mult;
    if (precision == 1)
         mult = 10;
    else
        mult = pow(10, precision);
    // Multiply value by a factor of 10, round to nearest integer, then divide by same factor
    roundValue = round(value * mult) / mult;
    return roundValue;
}

// Print a list from an array of strings
void printList (std::string str[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%i - %s\n", i, str[i].c_str());
    }
    printf("\n");
}

// Check if a selection option is between two values, min and max
int checkValue (int min, int max) {
    int value;

    while (true) {
        printf("Enter a value (%d-%d): ", min, max);
        std::cin >> value;
        if (std::cin && value >= min && value <= max) {
            std::cin.ignore();
            return value;
        }
        std::cin.clear();
        std::cin.ignore();
        printf("Invalid Input\n");
    }
}
