#include "LEGOAniView.h"
#include "Color.h"

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
	int nameLength;

	// Number of actors in this scene
	aniFile.read((char*)&actorsNum, sizeof(actorsNum));

	for (int i = 0; i < actorsNum; i++) {
		// Get length of actor name
		aniFile.read((char*)&nameLength, sizeof(nameLength));

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
	aniFile.read((char*)&nameLength, sizeof(nameLength));

	char* vba = new char[nameLength + 1];
	aniFile.read(vba, nameLength);
	vba[nameLength] = '\0';

	ani.sceneName = vba;
	delete vba;

	aniFile.seekg(12, std::ios::cur);

	// We currently don't know which components belong to which actors, so
	// only process keyframes that affect the entire actor
	for (int i = 0; i < actorsNum; i++) {
		ParseKeyframes(ani.actors[i]);
	}
	return true;
}

bool LEGOAniView::ParseKeyframes(Actor &actor)
{
	int nameLength;
	short keyframeNum;

	// Get length of actor/component name
	aniFile.read((char*)&nameLength, sizeof(nameLength));

	// Get actor/component name
	char* vba = new char[nameLength + 1];
	aniFile.read(vba, nameLength);
	vba[nameLength] = '\0';

	// Amount of keyframes
	aniFile.read((char*)&keyframeNum, sizeof(keyframeNum));

	// Does this actor/component match the one that was passed?
	if (actor.name == vba) {
		for (int i = 0; i < keyframeNum; i++) {
			Keyframe kf{};
			
			// We do not yet support handling rotation, scale, or morph keyframes
			kf.type = POSITION;

			// Position of this keyframe
			aniFile.read((char*)&kf.ms, sizeof(kf.ms));

			// Skip unknown 0x01 value
			aniFile.seekg(2, std::ios::cur);

			// X Position
			aniFile.read((char*)&kf.x_pos, sizeof(kf.x_pos));

			// Y Position
			aniFile.read((char*)&kf.y_pos, sizeof(kf.y_pos));

			// Z Position
			aniFile.read((char*)&kf.z_pos, sizeof(kf.z_pos));

			// Push this keyframe to vector in actor object
			actor.keyframes.push_back(kf);
		}
		// Skip the unimplemented types for now; skip rotation
		aniFile.read((char*)&keyframeNum, sizeof(keyframeNum));
		aniFile.seekg(keyframeNum * 20, std::ios::cur);

		// Skip scale
		aniFile.read((char*)&keyframeNum, sizeof(keyframeNum));
		aniFile.seekg(keyframeNum * 16, std::ios::cur);

		// Skip morph
		aniFile.read((char*)&keyframeNum, sizeof(keyframeNum));
		aniFile.seekg(keyframeNum * 5, std::ios::cur);

		// Skip ahead to next entry
		aniFile.seekg(4, std::ios::cur);

		delete vba;
		return true;
	}
	else {
		// If this is not the actor/component we want, skip to the next one
		aniFile.seekg(keyframeNum * 16, std::ios::cur);
		aniFile.read((char*)&keyframeNum, sizeof(keyframeNum));

		// Skip rotation
		aniFile.seekg(keyframeNum * 20, std::ios::cur);
		aniFile.read((char*)&keyframeNum, sizeof(keyframeNum));

		// Skip scale
		aniFile.seekg(keyframeNum * 16, std::ios::cur);
		aniFile.read((char*)&keyframeNum, sizeof(keyframeNum));

		// Skip morph
		aniFile.seekg(keyframeNum * 5, std::ios::cur);

		// Skip ahead to next entry
		aniFile.seekg(4, std::ios::cur);
	}
	delete vba;

	// Make sure we don't reach the end of the file
	if (!aniFile.good()) {
		return false;
	}
	ParseKeyframes(actor);
	return false;
}

void LEGOAniView::OutputData()
{
	// For output purposes
	static const char* typeNames[] = { "Position", "Rotation", "Scale", "Morph" };

	std::cout << "\nScene name: " << Color::Modifier(Color::FG_YELLOW) << ani.sceneName << "\n" << Color::Modifier(Color::FG_DEFAULT);
	std::cout << "Number of actors in this scene: " << Color::Modifier(Color::FG_YELLOW) << actorsNum << "\n" << Color::Modifier(Color::FG_DEFAULT);
	std::cout << "Actors in this scene: " << Color::Modifier(Color::FG_YELLOW);

	for (int i = 0; i < actorsNum; i++) {
		if (i != actorsNum - 1) {
			std::cout << ani.actors[i].name << ", ";
		}
		else std::cout << ani.actors[i].name << "\n" << Color::Modifier(Color::FG_DEFAULT);
	}

	for (int i = 0; i < actorsNum; i++) {
		std::cout << Color::Modifier(Color::FG_CYAN) << "\nAnimation data for actor " << ani.actors[i].name << ":" << Color::Modifier(Color::FG_DEFAULT);
		for (int i2 = 0; i2 < ani.actors[i].keyframes.size(); i2++) {
			std::cout << Color::Modifier(Color::FG_GREEN) << "\nKeyframe " << i2 + 1 << " (" << typeNames[ani.actors[i].keyframes[i2].type] << "):\n" << Color::Modifier(Color::FG_DEFAULT);
			std::cout << "Time: " << Color::Modifier(Color::FG_YELLOW) << ani.actors[i].keyframes[i2].ms << " ms\n" << Color::Modifier(Color::FG_DEFAULT);
			std::cout << "X: " << Color::Modifier(Color::FG_YELLOW) << ani.actors[i].keyframes[i2].x_pos << "\n" << Color::Modifier(Color::FG_DEFAULT);
			std::cout << "Y: " << Color::Modifier(Color::FG_YELLOW) << ani.actors[i].keyframes[i2].y_pos << "\n" << Color::Modifier(Color::FG_DEFAULT);
			std::cout << "Z: " << Color::Modifier(Color::FG_YELLOW) << ani.actors[i].keyframes[i2].z_pos << "\n" << Color::Modifier(Color::FG_DEFAULT);
		}
	}
}
