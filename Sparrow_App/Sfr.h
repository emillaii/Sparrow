#pragma once
#include "Sfr_entry.h"

class Sfr
{
public:
	static vector<Sfr_entry> calculateSfr(int idx, double currZ, std::string filename);
private:
	Sfr() {}
};