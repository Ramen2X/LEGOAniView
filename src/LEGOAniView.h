#pragma once

#include <iostream>
#include <fstream>

#include "./file/file.h"
#include "./ani/Ani.h"

class LEGOAniView {
public:
	Ani ani;
	f::File aniFile;
	bool ParseData(char* inputFile);
private:
	int actorsNum;
	int actorsFound = 0;
	bool ParseActors();
	bool ParseKeyframes(Actor &actor);
	void OutputData();
};
