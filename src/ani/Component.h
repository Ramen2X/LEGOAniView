#include <string>
#include <vector>

#include "Keyframe.h"

struct Component {
	std::string name; // Name of the component
	std::vector<Keyframe> keyframes; // Keyframes of the component
};