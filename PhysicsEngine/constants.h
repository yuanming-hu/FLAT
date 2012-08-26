#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

const int W = 800;
const int H = 600;
const double eps = 1e-7;
const double pi = acos(-1.);
const double timeInterval = 0.1;

inline int RGB(int R, int G, int B) {
	return ((R & 255) << 16) + ((G & 255) << 8) + (B & 255);
}

inline double sqr(double a) {
	return a * a;
}

#endif