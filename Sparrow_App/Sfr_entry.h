#pragma once
#include <cmath>
#include <vector>

using std::vector;

class Sfr_entry {
public:

	Sfr_entry(void) : group(-1), px(-1), py(-1), pz(0), sfr(64, 0.0) {
	}

	Sfr_entry(const int group, const double px, const double py, const vector<double>& sfr) : group(group), px(px), py(py), sfr(sfr) {
	}

	double distance(const double nx, const double ny) {
		return sqrt((px - nx)*(px - nx) + (py - ny)*(py - ny));
	}

	void clear(void) {
		for (size_t i = 0; i < sfr.size(); i++) {
			sfr[i] = 0.0;
		}
	}

	int group;
	double px;
	double py;
	double pz;
	vector<double> sfr;
	double sfr_025;
};

// least-squares fit (inverse of design matrix) of a cubic polynomial through 4 points [-1..2]/64.0
const double sfr_cubic_weights[4][4] = {
	{ 0.00000000000000e+00,   1.00000000000000e+00,   0.00000000000000e+00,   0.00000000000000e+00 },
{ -2.13333333333333e+01,  -3.20000000000000e+01,   6.40000000000000e+01,  -1.06666666666667e+01 },
{ 2.04800000000000e+03,  -4.09600000000000e+03,   2.04800000000000e+03,   0.00000000000000e+00 },
{ -4.36906666666667e+04,   1.31072000000000e+05,  -1.31072000000000e+05,   4.36906666666667e+04 }
};
