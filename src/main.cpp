#include "LEGOAniView.h"
#include "./misc/Color.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << Color::Modifier(Color::FG_RED) << "No file specified. Exiting..." << Color::Modifier(Color::FG_DEFAULT);
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
		if (!av.ParseData(argv[1])) {
			std::cout << Color::Modifier(Color::FG_RED) << "Operation failed." << Color::Modifier(Color::FG_DEFAULT);
			return 0;
		}
		return 0;
	}
}