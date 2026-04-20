#include "blocks/ps1.hpp"

#include <git2.h>
#include <pwd.h>
#include <unistd.h>

#include <climits>
#include <cstring>
#include <iostream>
#include <sstream>

#include "constants.hpp"

int ps1() {
	std::string out;
	out.reserve(1024);

	const char* user_env = getenv("USER");
	const char* ssh_env	 = getenv("SSH_CONNECTION");

	char host_buf[HOST_NAME_MAX];
	gethostname(host_buf, HOST_NAME_MAX);

	char cwd_buf[1024];
	getcwd(cwd_buf, sizeof(cwd_buf));

	out += "\n╭─";
	// HOST BLOCK
	out += RESET;
	out += BG_HOST;
	out += " 💻";
	if (ssh_env) {
		out += " 🔑";
	}
	out += " ";
	out += user_env ? user_env : "";
	out += "@";
	out += host_buf;
	out += " ";
	out += RESET;
	out += FG_HOST;
	out += BG_DIR;
	out += ICON_CLOSE_BOX;
	out += RESET;
	out += BG_DIR;
	// END HOST BLOCK

	// DIR BLOCK
	out += " 📁 ";
	out += collapse_home(cwd_buf);
	// END DIR BLOCK

	// GIT BLOCK
	bool gitPresent = false;
	out += get_git_prompt_segment(gitPresent);
	// END GIT BLOCK

	// WORKSPACE BLOCK
	out += " ";
	out += RESET;
	out += (gitPresent ? FG_GIT : FG_DIR);
	out += ICON_CLOSE_BOX;

	out += RESET;

	out += "\n╰──$";

	const char* venv_env = getenv("VIRTUAL_ENV");
	std::string venv;
	if (venv_env) {
		const char* slash = strrchr(venv_env, '/');
		venv			  = slash ? slash + 1 : venv_env;
	}

	if (!venv.empty()) {
		out += "(🛠 venv:";
		out += venv;
		out += ") ";
	} else {
		const char* box_env = getenv("CONTAINER_ID");
		if (box_env) {
			out += "(🛠 box:";
			out += box_env;
			out += ") ";
		}
	}
	// END WORKSPACE BLOCK

	out += " ";

	std::cout << out;
	return 0;
}

std::string collapse_home(const std::string& cwd) {
	std::string home_dir;

	const char* home = getenv("HOME");
	if (home && *home) {
		home_dir = home;
	}

	struct passwd* pw = getpwuid(getuid());
	if (pw && pw->pw_dir) {
		home_dir = pw->pw_dir;
	}

	if (home_dir.empty()) {
		return cwd;
	}

	if (cwd == home_dir) {
		return "~";
	}

	if (cwd.size() > home_dir.size() && cwd.compare(0, home_dir.size(), home_dir) == 0 && cwd[home_dir.size()] == '/') {
		return "~" + cwd.substr(home_dir.size());
	}

	return cwd;
}

std::string get_git_prompt_segment(bool& gitPresent) {
	std::string out = "";
	gitPresent		= true;
	std::istringstream iss(exec_cmd("git status --porcelain --branch -uno 2>/dev/null"));
	std::string header;
	if (std::getline(iss, header)) {
		bool has_changes = (iss.peek() != EOF);
		out += " ";
		out += RESET;
		out += FG_DIR;
		out += BG_GIT;
		out += ICON_CLOSE_BOX;
		out += RESET;
		out += BG_GIT;
		if (header.rfind("## ", 0) == 0) {
			header = header.substr(3);
		}
		std::string branch = header.substr(0, header.find("..."));
		out += has_changes ? FG_GIT_CH : FG_GIT_OK;
		out += "  ";
		out += branch;
		int ahead = 0, behind = 0;
		auto pos = header.find("ahead ");
		if (pos != std::string::npos) {
			ahead = std::atoi(header.c_str() + pos + 6);
		}
		pos = header.find("behind ");
		if (pos != std::string::npos) {
			behind = std::atoi(header.c_str() + pos + 7);
		}
		if (ahead || behind) {
			out += " ";
			if (behind) {
				out += to_superscript(behind);
				out += "⇣";
			}
			if (ahead) {
				out += "⇡";
				out += to_subscript(ahead);
			}
		}
	} else {
		gitPresent = false;
	}

	return out;
}

std::string to_subscript(int n) {
	static const char* sub[] = {"₀", "₁", "₂", "₃", "₄", "₅", "₆", "₇", "₈", "₉"};
	if (n == 0) {
		return "₀";
	}

	char buf[64];
	int pos = sizeof(buf);

	while (n > 0) {
		const char* s = sub[n % 10];
		int len		  = std::strlen(s);
		pos -= len;
		std::memcpy(buf + pos, s, len);
		n /= 10;
	}
	return std::string(buf + pos, sizeof(buf) - pos);
}

std::string to_superscript(int n) {
	static const char* sup[] = {"⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"};

	if (n == 0) {
		return "⁰";
	}

	char buf[64];
	int pos = sizeof(buf);

	while (n > 0) {
		const char* s = sup[n % 10];
		int len		  = std::strlen(s);
		pos -= len;
		std::memcpy(buf + pos, s, len);
		n /= 10;
	}
	return std::string(buf + pos, sizeof(buf) - pos);
}

std::string exec_cmd(const char* cmd) {
	std::array<char, 256> buffer;
	std::string result;
	result.reserve(512);
	FILE* pipe = popen(cmd, "r");
	if (!pipe) {
		return "";
	}
	while (fgets(buffer.data(), buffer.size(), pipe)) {
		result.append(buffer.data());
	}
	pclose(pipe);
	return result;
}