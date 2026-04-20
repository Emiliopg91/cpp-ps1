// clang++ -O3 -lgit2 -ffast-math -march=native -flto -fno-exceptions -fno-rtti
// -pipe -s -std=c++17 cpp-ps1.cpp -o cpp-ps1

#include <limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "blocks/previous.hpp"
#include "blocks/ps1.hpp"
#include "blocks/source.hpp"

// -------------------- MAIN --------------------
int main(int argc, char* argv[]) {
	switch (argc) {
		case 1:
			return ps1();
		case 2: {
			std::string arg = argv[1];
			if (arg == "--source") {
				return printSource(argv[0]);
			}
			return 1;
		}
		case 4:
			return exitAndTimer(argv[1], argv[2], argv[3]);
	}
}
