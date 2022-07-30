#include "Component.h"

struct Actor {
	std::string name; // Name of the actor
	std::vector<Component> components; // Components of the actor
	std::vector<Keyframe> keyframes; // Keyframes of the actor
};