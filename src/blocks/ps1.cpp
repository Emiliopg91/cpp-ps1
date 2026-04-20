#include "blocks/ps1.hpp"

#include <git2.h>
#include <pwd.h>
#include <unistd.h>

#include <climits>
#include <cstring>
#include <iostream>

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
	git_libgit2_init();

	gitPresent = true;

	git_repository* repo = nullptr;
	if (git_repository_open_ext(&repo, ".", 0, nullptr) != 0) {
		gitPresent = false;
		std::cout << "No encontrado";
		const git_error* err = git_error_last();
		if (err) {
			std::cerr << "libgit2 error: " << err->message << "\n";
		}
		return "";
	}

	git_reference* head = nullptr;
	if (git_repository_head(&head, repo) != 0) {
		git_repository_free(repo);
		gitPresent = false;
		return "";
	}

	const char* branch = git_reference_shorthand(head);

	// -------- STATUS (changes) --------
	git_status_options statusopt = GIT_STATUS_OPTIONS_INIT;
	statusopt.show				 = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
	// statusopt.flags				 = GIT_STATUS_OPT_INCLUDE_UNTRACKED;  // quítalo si quieres equivalente a -uno

	git_status_list* status = nullptr;
	git_status_list_new(&status, repo, &statusopt);

	bool has_changes = git_status_list_entrycount(status) > 0;

	// -------- AHEAD / BEHIND --------
	size_t ahead = 0, behind = 0;

	git_reference* upstream = nullptr;
	if (git_branch_upstream(&upstream, head) == 0) {
		const git_oid* local_oid	= git_reference_target(head);
		const git_oid* upstream_oid = git_reference_target(upstream);

		if (local_oid && upstream_oid) {
			git_graph_ahead_behind(&ahead, &behind, repo, local_oid, upstream_oid);
		}
	}

	// -------- BUILD OUTPUT --------
	std::string out;

	out += " ";
	out += RESET;
	out += FG_DIR;
	out += BG_GIT;
	out += ICON_CLOSE_BOX;
	out += RESET;
	out += BG_GIT;

	out += has_changes ? FG_GIT_CH : FG_GIT_OK;
	out += "  ";
	out += branch;

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

	// -------- CLEANUP --------
	if (upstream) {
		git_reference_free(upstream);
	}
	git_status_list_free(status);
	git_reference_free(head);
	git_repository_free(repo);
	git_libgit2_shutdown();

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