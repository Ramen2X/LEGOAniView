#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>

#include "./file/file.h"
#include "./ani/Ani.h"

#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923

class LEGOAniView {
public:
	Ani ani;
	f::File aniFile;
	bool useEuler = false;
	bool ParseData(char* inputFile);
private:
	int actorsNum;
	int actorsFound = 0;
	short keyframeNum;
	bool ParseActors();
	bool ParseKeyframes(Actor &actor);
	void ReadKeyframes(Type type, Actor& actor);
	void OutputData();
	void EulerFromQuaternion(float xp, float yp, float zp, float wp, float &pitch, float &yaw, float &roll) const;
};
