#include "LEGOAniView.h"
#include "./misc/Color.h"

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
		std::cout << "\nCould not find the specified file.\n";
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
	std::string name;

	// Get length of actor/component name
	nameLength = aniFile.ReadU32();

	if (aniFile.atEnd()) {
		return false;
	}

	// Get actor/component name
	name = std::string(aniFile.ReadBytes(nameLength).data(), nameLength);

	// Does this actor/component match the one that was passed?
	if (actor.name == name) {
		actorsFound++;

		ReadKeyframes(POSITION, actor);
		ReadKeyframes(ROTATION, actor);
		ReadKeyframes(SCALE, actor);

		// Skip the unimplemented morph for now
		keyframeNum = aniFile.ReadU16();
		aniFile.seek(keyframeNum * 5, f::File::SeekCurrent);

		// Skip ahead to next entry
		aniFile.seek(4, f::File::SeekCurrent);

		return true;
	}
	else {
		// If this is not the actor/component we want, skip to the next one
		keyframeNum = aniFile.ReadU16();
		aniFile.seek(keyframeNum * 16, f::File::SeekCurrent);

		// Skip rotation
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
	}

	// Make sure we don't reach the end of the file
	if (aniFile.atEnd()) {
		return false;
	}

	// Recurse only if we have not found all actors
	if (actorsFound != actorsNum) {
		ParseKeyframes(actor);
		return false;
	}
	else return true;
}

void LEGOAniView::ReadKeyframes(Type type, Actor& actor)
{
	// Amount of keyframes of this type
	keyframeNum = aniFile.ReadU16();

	for (int i = 0; i < keyframeNum; i++) {
		Keyframe kf{};

		kf.type = type;

		kf.num = i + 1;

		// Position of this keyframe
		kf.ms = aniFile.ReadU16();

		// Skip unknown 0x01 value
		aniFile.seek(2, f::File::SeekCurrent);

		if (type == ROTATION) {
			// W (Angle)
			kf.w = aniFile.ReadFloat();
		}

		// X
		kf.x = aniFile.ReadFloat();

		// Y
		kf.y = aniFile.ReadFloat();

		// Z
		kf.z = aniFile.ReadFloat();

		// Convert from quaternions to euler
		if (type == ROTATION && useEuler) {
			EulerFromQuaternion(kf.x, kf.y, kf.z, kf.w, kf.x, kf.y, kf.z);
			// We don't need the angle anymore
			kf.w = 0;
		}

		// Push this keyframe to vector in actor object
		actor.keyframes.push_back(kf);
	}
}

void LEGOAniView::EulerFromQuaternion(float xp, float yp, float zp, float wp, float &pitch, float &yaw, float &roll) const
{
	// Algorithm from:
	// http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q37
	float xx = xp * xp;
	float xy = xp * yp;
	float xz = xp * zp;
	float xw = xp * wp;
	float yy = yp * yp;
	float yz = yp * zp;
	float yw = yp * wp;
	float zz = zp * zp;
	float zw = zp * wp;
	const float lengthSquared = xx + yy + zz + wp * wp;
	if ((lengthSquared - 1.0f) != 0.0 && (lengthSquared != 0.0)) {
		xx /= lengthSquared;
		xy /= lengthSquared; // same as (xp / length) * (yp / length)
		xz /= lengthSquared;
		xw /= lengthSquared;
		yy /= lengthSquared;
		yz /= lengthSquared;
		yw /= lengthSquared;
		zz /= lengthSquared;
		zw /= lengthSquared;
	}
	pitch = std::asin(-2.0f * (yz - xw));
	if (pitch < M_PI_2) {
		if (pitch > -M_PI_2) {
			yaw = std::atan2(2.0f * (xz + yw), 1.0f - 2.0f * (xx + yy));
			roll = std::atan2(2.0f * (xy + zw), 1.0f - 2.0f * (xx + zz));
		}
		else {
			// not a unique solution
			roll = 0.0f;
			yaw = -std::atan2(-2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
		}
	}
	else {
		// not a unique solution
		roll = 0.0f;
		yaw = std::atan2(-2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
	}
	pitch *= 180 / M_PI;
	yaw *= 180 / M_PI;
	roll *= 180 / M_PI;
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
		if (ani.actors[i].keyframes.size() != 0) {
			for (int i2 = 0; i2 < ani.actors[i].keyframes.size(); i2++) {
				std::cout << Color::Modifier(Color::FG_GREEN) << "\nKeyframe " << ani.actors[i].keyframes[i2].num << " (" << typeNames[ani.actors[i].keyframes[i2].type] << "):\n" << Color::Modifier(Color::FG_DEFAULT);

				if (useFrames) {
					std::cout << "Frame: " << Color::Modifier(Color::FG_YELLOW) << (ani.actors[i].keyframes[i2].ms * 0.001) * 60 << "\n" << Color::Modifier(Color::FG_DEFAULT);
				}
				else std::cout << "Time: " << Color::Modifier(Color::FG_YELLOW) << ani.actors[i].keyframes[i2].ms << " ms\n" << Color::Modifier(Color::FG_DEFAULT);

				std::cout << "X: " << Color::Modifier(Color::FG_YELLOW) << std::fixed << std::setprecision(8) << ani.actors[i].keyframes[i2].x << "\n" << Color::Modifier(Color::FG_DEFAULT);
				std::cout << "Y: " << Color::Modifier(Color::FG_YELLOW) << ani.actors[i].keyframes[i2].y << "\n" << Color::Modifier(Color::FG_DEFAULT);
				std::cout << "Z: " << Color::Modifier(Color::FG_YELLOW) << ani.actors[i].keyframes[i2].z << "\n" << Color::Modifier(Color::FG_DEFAULT);
				if (ani.actors[i].keyframes[i2].type == ROTATION && !useEuler) {
					std::cout << "W (Angle): " << Color::Modifier(Color::FG_YELLOW) << ani.actors[i].keyframes[i2].w << "\n" << Color::Modifier(Color::FG_DEFAULT);
				}
				// To prevent the time/frame count from being ultra-precise
				std::cout.precision(0);
			}
		}
		else std::cout << Color::Modifier(Color::FG_YELLOW) << " None!\n" << Color::Modifier(Color::FG_DEFAULT);
	}
}
