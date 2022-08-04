#include "LEGOAniView.h"
#include "./misc/Color.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "\nUsage: LEGOAniView *.ani [options]\n\nOptions:\n--use-euler: Prints rotation information in Euler angles as opposed to quaternions\n--use-frames: Prints time value of keyframe as a frame value (relative to 60fps) as opposed to milliseconds\n--help: Shows these usage details\n\n";
		std::cout << Color::Modifier(Color::FG_RED) << "No file specified. Exiting...\n" << Color::Modifier(Color::FG_DEFAULT);
		return 0;
	}
	else {
		LEGOAniView av;
		if (argc > 2) {
			for (int i = 0; i < argc; i++) {
				if (!strcmp(argv[i], "--use-euler")) {
					av.useEuler = true;
				}
				if (!strcmp(argv[i], "--use-frames")) {
					av.useFrames = true;
				}
			}
		}
		if (!strcmp(argv[1], "--help")) {
			std::cout << "\nUsage: LEGOAniView *.ani [options]\n\nOptions:\n--use-euler: Prints rotation information in Euler angles as opposed to quaternions\n--use-frames: Prints time value of keyframe as a frame value (relative to 60fps) as opposed to milliseconds\n--help: Shows these usage details\n";
			return 0;
		}
		if (!av.ParseData(argv[1])) {
			std::cout << Color::Modifier(Color::FG_RED) << "Operation failed." << Color::Modifier(Color::FG_DEFAULT);
			return 0;
		}
		return 0;
	}
}