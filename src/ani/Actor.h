#include "component.h"

struct Actor : public Component {
	std::vector<Component> components; // Components of the actor
};