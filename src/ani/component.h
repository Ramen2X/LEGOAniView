#include <string>
#include <vector>

#include "keyframe.h"

struct Component {
	std::string name; // Name of the component
	std::vector<Keyframe> keyframes; // Keyframes of the component
};