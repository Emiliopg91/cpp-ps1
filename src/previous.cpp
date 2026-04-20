#include "previous.hpp"

#include <iostream>
#include <string>

#include "constants.hpp"

int exitAndTimer(char* argv1, char* argv2, char* argv3) {
	std::string out;
	out.reserve(1024);

	// EXIT CODE BLOCK
	long ms	 = std::strtol(argv2, nullptr, 10);
	int code = std::strtol(argv1, nullptr, 10);

	bool ok					   = (code == 0 || code == 130 || code == 148);
	const std::string& exit_bg = ok ? BG_EXIT_OK : BG_EXIT_ERR;
	const std::string& exit_fg = ok ? FG_EXIT_OK : FG_EXIT_ERR;

	out += RESET;
	out += exit_fg;
	out += ICON_OPEN_BOX;
	out += RESET;
	out += exit_bg;
	out += " ";
	out += std::to_string(code);
	out += " ";
	out += RESET;
	// END EXIT CODE BLOCK

	// TIMER BLOCK

	long t	   = ms;
	long hours = t / 3600000;
	t %= 3600000;
	long minutes = t / 60000;
	t %= 60000;
	long seconds = t / 1000;
	long millis	 = t % 1000;

	char buf[64];
	if (hours > 0) {
		snprintf(buf, sizeof(buf), "%ld:%02ld:%02ld.%03ld", hours, minutes, seconds, millis);
	} else if (minutes > 0) {
		snprintf(buf, sizeof(buf), "%ld:%02ld.%03ld", minutes, seconds, millis);
	} else {
		snprintf(buf, sizeof(buf), "%ld.%03ld", seconds, millis);
	}

	out += RESET;
	out += exit_bg;
	out += FG_TIMER;
	out += ICON_OPEN_BOX;
	out += RESET;
	out += BG_TIMER;
	out += " 🕒 ";
	out += buf;
	out += " ";
	out += RESET;
	out += BG_TIMER;
	out += FG_CMD;
	out += ICON_OPEN_BOX;
	// END TIMER BLOCK

	out += RESET;
	out += BG_CMD;
	out += " ";
	out += argv3;
	out += " ";

	out += RESET;
	out += "\n";

	std::cout << out;

	return 0;
}