#include "blocks/ps1.hpp"

#include <limits.h>
#include <pwd.h>
#include <unistd.h>

#include <cstring>
#include <sstream>

#include "constants.hpp"

constexpr const char* FG_HOST = "\033[38;2;42;78;180m";
constexpr const char* BG_HOST = "\033[48;2;42;78;180m";

constexpr const char* FG_DIR = "\033[38;2;92;92;92m";
constexpr const char* BG_DIR = "\033[48;2;92;92;92m";

constexpr const char* FG_GIT_OK = "\033[38;2;0;150;75m";
constexpr const char* FG_GIT_CH = "\033[38;2;205;133;63m";
constexpr const char* FG_GIT	= "\033[38;2;56;56;56m";
constexpr const char* BG_GIT	= "\033[48;2;56;56;56m";

std::string host_block(const std::string& cwd) {
	const char* user_env = getenv("USER");
	const char* ssh_env	 = getenv("SSH_CONNECTION");

	char host_buf[HOST_NAME_MAX];
	gethostname(host_buf, HOST_NAME_MAX);

	std::string out = RESET;
	out.append(BG_HOST);
	out.append(" 💻");
	if (ssh_env) {
		out.append(" 🔑");
	}
	out.append(" ");
	if (user_env) {
		out.append(user_env);
	}
	out.append("@").append(host_buf).append(" ").append(RESET).append(FG_HOST).append(BG_DIR).append(ICON_CLOSE_BOX).append(RESET).append(BG_DIR);

	return out;
}

std::string workspace_block(bool gitPresent) {
	std::string out = " ";
	out.append(RESET).append((gitPresent ? FG_GIT : FG_DIR)).append(ICON_CLOSE_BOX).append(RESET).append("\n╰──$");

	const char* venv_env = getenv("VIRTUAL_ENV");
	std::string venv;
	if (venv_env) {
		const char* slash = strrchr(venv_env, '/');
		venv			  = slash ? slash + 1 : venv_env;
	}

	if (!venv.empty()) {
		out.append("(🛠 venv:").append(venv).append(") ");
	} else {
		const char* box_env = getenv("CONTAINER_ID");
		if (box_env) {
			out.append("(🛠 box:").append(box_env).append(") ");
		}
	}

	return out;
}

std::string dir_block(const std::string& cwd) {
	std::string home_dir;

	const char* home = getenv("HOME");
	if (!home || !*home) {
		struct passwd* pw = getpwuid(getuid());
		if (pw && pw->pw_dir) {
			home = pw->pw_dir;
		}
	}

	std::string out = " 📁 ";

	if (home_dir.empty()) {
		return out + cwd;
	}

	if (cwd == home_dir) {
		return out + "~";
	}

	if (cwd.size() > home_dir.size() && cwd.compare(0, home_dir.size(), home_dir) == 0 && cwd[home_dir.size()] == '/') {
		return out + "~" + cwd.substr(home_dir.size());
	}

	return out + cwd;
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

std::string git_block(bool& gitPresent) {
	std::string out;
	out.reserve(128);

	gitPresent = true;

	std::string output = exec_cmd("git status --porcelain --branch -uno 2>/dev/null");

	if (output.empty()) {
		gitPresent = false;
		return "";
	}

	size_t newline = output.find('\n');

	std::string_view header_view = (newline == std::string::npos) ? std::string_view(output) : std::string_view(output.data(), newline);

	bool has_changes = (newline != std::string::npos && newline + 1 < output.size());

	out.append(" ").append(RESET).append(FG_DIR).append(BG_GIT).append(ICON_CLOSE_BOX).append(RESET).append(BG_GIT);

	if (header_view.rfind("## ", 0) == 0) {
		header_view.remove_prefix(3);
	}

	size_t dots					 = header_view.find("...");
	std::string_view branch_view = (dots == std::string_view::npos) ? header_view : header_view.substr(0, dots);

	out.append(has_changes ? FG_GIT_CH : FG_GIT_OK).append("  ").append(branch_view);

	int ahead = 0, behind = 0;

	size_t pos = header_view.find("ahead ");
	if (pos != std::string_view::npos) {
		ahead = std::atoi(header_view.data() + pos + 6);
	}

	pos = header_view.find("behind ");
	if (pos != std::string_view::npos) {
		behind = std::atoi(header_view.data() + pos + 7);
	}

	if (ahead || behind) {
		out.append(" ");
		if (behind) {
			out.append(to_superscript(behind)).append("⇣");
		}
		if (ahead) {
			out.append("⇡").append(to_subscript(ahead));
		}
	}

	return out;
}

int ps1() {
	std::string out;
	out.reserve(1024);

	char cwd_buf[1024];
	getcwd(cwd_buf, sizeof(cwd_buf));

	out.append("\n╭─");
	// HOST BLOCK
	out.append(host_block(cwd_buf));
	// END HOST BLOCK

	// DIR BLOCK
	out.append(dir_block(cwd_buf));
	// END DIR BLOCK

	// GIT BLOCK
	bool gitPresent = false;
	out.append(git_block(gitPresent));
	// END GIT BLOCK

	// WORKSPACE BLOCK
	out.append(workspace_block(gitPresent));
	// END WORKSPACE BLOCK

	write(1, out.data(), out.size());

	return 0;
}