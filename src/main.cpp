#include "LEGOAniView.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "No file specified. Exiting...";
		return 0;
	}
	else {
		LEGOAniView av;
		if (!av.ParseData(argv[1])) {
			std::cout << "Operation failed.";
			return 0;
		}
		return 0;
	}
}