#pragma once

#include <iostream>
#include <fstream>

#include "./ani/Ani.h"

class LEGOAniView {
public:
	Ani ani;
	std::ifstream aniFile;
	bool ParseData(char* inputFile);
private:
	int actorsNum;
	bool ParseActors();
	void OutputData();
};
