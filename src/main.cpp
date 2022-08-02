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
			if (!strcmp(argv[2], "--use-euler")) {
				av.useEuler = true;
			}
		}
		if (!av.ParseData(argv[1])) {
			std::cout << Color::Modifier(Color::FG_RED) << "Operation failed." << Color::Modifier(Color::FG_DEFAULT);
			return 0;
		}
		return 0;
	}
}