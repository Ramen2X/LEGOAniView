#include "LEGOAniView.h"
#include "Color.h"

bool LEGOAniView::ParseData(char* inputFile)
{
	if (aniFile.Open(inputFile, f::File::Read)) {
		// All ani files seem to start with 0x11
		if (aniFile.ReadU8() != 17) {
			std::cout << "Invalid file specified.\n";
			return false;
		}
		if (!ParseActors()) {
			std::cout << "An error occurred while parsing actors.\n";
			return false;
		}
		aniFile.Close();

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
	aniFile.seek(28);
	int nameLength;
	std::string name;

	// Number of actors in this scene
	actorsNum = aniFile.ReadU32();

	for (int i = 0; i < actorsNum; i++) {
		// Get length of actor name
		nameLength = aniFile.ReadU32();

		// Get actor name
		name = std::string(aniFile.ReadBytes(nameLength).data(), nameLength);

		// Append actors to vector in ani object
		Actor actor = { name };
		ani.actors.push_back(actor);

		// Continue to next entry
		aniFile.seek(4, f::File::SeekCurrent);
	}

	// Scene name is stored directly after actors
	aniFile.seek(4, f::File::SeekCurrent);
	nameLength = aniFile.ReadU32();

	name = std::string(aniFile.ReadBytes(nameLength).data(), nameLength);
	ani.sceneName = name;

	aniFile.seek(12, f::File::SeekCurrent);

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
	std::string name;

	// Get length of actor/component name
	nameLength = aniFile.ReadU32();

	// Get actor/component name
	name = std::string(aniFile.ReadBytes(nameLength).data(), nameLength);

	// Amount of keyframes
	keyframeNum = aniFile.ReadU16();

	// Does this actor/component match the one that was passed?
	if (actor.name == name) {
		for (int i = 0; i < keyframeNum; i++) {
			Keyframe kf{};
			
			// We do not yet support handling rotation, scale, or morph keyframes
			kf.type = POSITION;

			// Position of this keyframe
			kf.ms = aniFile.ReadU16();

			// Skip unknown 0x01 value
			aniFile.seek(2, f::File::SeekCurrent);

			// X Position
			kf.x_pos = aniFile.ReadFloat();

			// Y Position
			kf.y_pos = aniFile.ReadFloat();

			// Z Position
			kf.z_pos = aniFile.ReadFloat();

			// Push this keyframe to vector in actor object
			actor.keyframes.push_back(kf);
		}
		// Skip the unimplemented types for now; skip rotation
		keyframeNum = aniFile.ReadU16();
		aniFile.seek(keyframeNum * 20, f::File::SeekCurrent);

		// Skip scale
		keyframeNum = aniFile.ReadU16();
		aniFile.seek(keyframeNum * 16, f::File::SeekCurrent);

		// Skip morph
		keyframeNum = aniFile.ReadU16();
		aniFile.seek(keyframeNum * 5, f::File::SeekCurrent);

		// Skip ahead to next entry
		aniFile.seek(4, f::File::SeekCurrent);

		return true;
	}
	else {
		// If this is not the actor/component we want, skip to the next one
		aniFile.seek(keyframeNum * 16, f::File::SeekCurrent);
		keyframeNum = aniFile.ReadU16();

		// Skip rotation
		aniFile.seek(keyframeNum * 20, f::File::SeekCurrent);
		keyframeNum = aniFile.ReadU16();

		// Skip scale
		aniFile.seek(keyframeNum * 16, f::File::SeekCurrent);
		keyframeNum = aniFile.ReadU16();

		// Skip morph
		aniFile.seek(keyframeNum * 5, f::File::SeekCurrent);

		// Skip ahead to next entry
		aniFile.seek(4, f::File::SeekCurrent);
	}

	// Make sure we don't reach the end of the file
	if (aniFile.atEnd()) {
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
