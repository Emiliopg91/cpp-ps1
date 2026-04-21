#include "blocks/previous.hpp"

#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

#include "constants.hpp"

constexpr const char* FG_TIMER = "\033[38;2;60;90;130m";
constexpr const char* BG_TIMER = "\033[48;2;60;90;130m";

constexpr const char* FG_EXIT_OK = "\033[38;2;0;110;0m";
constexpr const char* BG_EXIT_OK = "\033[48;2;0;110;0m";

constexpr const char* FG_EXIT_ERR = "\033[38;2;110;0;0m";
constexpr const char* BG_EXIT_ERR = "\033[48;2;110;0;0m";

constexpr const char* FG_CMD = "\033[38;2;60;60;60m";
constexpr const char* BG_CMD = "\033[48;2;60;60;60m";

std::string exit_block(char* argv1, std::string& exit_bg) {
	std::string out;
	int code = std::strtol(argv1, nullptr, 10);

	bool ok					   = (code == 0 || code == 130 || code == 148);
	exit_bg					   = ok ? BG_EXIT_OK : BG_EXIT_ERR;
	const std::string& exit_fg = ok ? FG_EXIT_OK : FG_EXIT_ERR;

	out.append(RESET).append(exit_fg).append(ICON_OPEN_BOX).append(RESET).append(exit_bg).append(" ");

	char code_buf[16];
	int code_len = snprintf(code_buf, sizeof(code_buf), "%d", code);
	out.append(code_buf, code_len);

	out.append(" ").append(RESET);

	return out;
}

std::string timer_block(char* argv2, const std::string& exit_bg) {
	std::string out;
	long ms	   = std::strtol(argv2, nullptr, 10);
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

	out.append(RESET)
		.append(exit_bg)
		.append(FG_TIMER)
		.append(ICON_OPEN_BOX)
		.append(RESET)
		.append(BG_TIMER)
		.append(" 🕒 ")
		.append(buf)
		.append(" ")
		.append(RESET)
		.append(BG_TIMER)
		.append(FG_CMD)
		.append(ICON_OPEN_BOX);

	return out;
}

int exitAndTimer(char* argv1, char* argv2, char* argv3) {
	std::string out;
	out.reserve(128 + strlen(argv3));

	std::string exit_bg;
	// EXIT CODE BLOCK
	out.append(exit_block(argv1, exit_bg));
	// END EXIT CODE BLOCK

	// TIMER BLOCK
	out.append(timer_block(argv2, exit_bg));
	// END TIMER BLOCK

	out.append(RESET)
		.append(BG_CMD)
		.append(" ")
		.append(argv3)
		.append(" ")

		.append(RESET)
		.append("\n");

	write(1, out.data(), out.size());

	return 0;
}