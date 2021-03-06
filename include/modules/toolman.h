// RLG Project 2015

#ifndef TOOLMAN_H
#define TOOLMAN_H

#include <string>
#include <vector>

// Random number and name generators
int rng(int, int);
double rngLog(double, double);
std::string randName();

// Useful tools
double deciTrunc (double, int);
void printList (std::vector<std::string>, int);
int checkValue (int, int);

#endif // TOOLMAN_H
