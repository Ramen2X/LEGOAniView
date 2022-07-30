#include "LEGOAniView.h"

bool LEGOAniView::ParseData(char* inputFile)
{
	aniFile.open(inputFile);

	if (aniFile.is_open()) {
		char b;
		if (aniFile.read(&b, 1)) {
			// All ani files seem to start with 0x11
			if (b != 0x11) {
				std::cout << "Invalid file specified.\n";
				return false;
			}
			if (!ParseActors()) {
				std::cout << "An error occurred while parsing actors.\n";
				return false;
			}
		}
		aniFile.close();

		OutputData();
		return true;
	}
	else {
		std::cout << "Could not find the specified file.\n";
		return false;
	}
}

bool LEGOAniView::ParseActors()
{
	// Jump forward to actor index
	aniFile.seekg(28, std::ios::beg);
	char ba[4];
	int nameLength;

	// Number of actors in this scene
	aniFile.read(ba, 4);
	actorsNum = *(int*)ba;

	for (int i = 0; i < actorsNum; i++) {
		// Get length of actor name
		aniFile.read(ba, 4);
		nameLength = *(int*)ba;

		// Get actor name
		char* vba = new char[nameLength + 1];
		aniFile.read(vba, nameLength);

		// Not sure if there's a better way to add a null terminator here
		vba[nameLength] = '\0';

		// Append actors to vector in ani object
		Actor actor = { vba };
		ani.actors.push_back(actor);
		delete vba;

		// Continue to next entry
		aniFile.seekg(4, std::ios::cur);
	}

	// Scene name is stored directly after actors
	aniFile.seekg(4, std::ios::cur);
	aniFile.read(ba, 4);
	nameLength = *(int*)ba;

	char* vba = new char[nameLength + 1];
	aniFile.read(vba, nameLength);
	vba[nameLength] = '\0';

	ani.sceneName = vba;
	delete vba;

	return true;
}

void LEGOAniView::OutputData()
{
	std::cout << "\nScene name: " << ani.sceneName << "\n";
	std::cout << "Number of actors in this scene: " << actorsNum << "\n";
	std::cout << "Actors in this scene: ";

	for (int i = 0; i < actorsNum; i++) {
		if (i != actorsNum - 1) {
			std::cout << ani.actors[i].name << ", ";
		}
		else std::cout << ani.actors[i].name << "\n";
	}
}
