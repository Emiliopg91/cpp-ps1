// clang++ -O3 -lgit2 -ffast-math -march=native -flto -fno-exceptions -fno-rtti
// -pipe -s -std=c++17 cpp-ps1.cpp -o cpp-ps1

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits.h>
#include <pwd.h>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>

// -------------------- UTILIDADES --------------------

inline std::string to_subscript(int n) {
  static const char *sub[] = {"₀", "₁", "₂", "₃", "₄", "₅", "₆", "₇", "₈", "₉"};
  if (n == 0)
    return "₀";

  char buf[64];
  int pos = sizeof(buf);

  while (n > 0) {
    const char *s = sub[n % 10];
    int len = std::strlen(s);
    pos -= len;
    std::memcpy(buf + pos, s, len);
    n /= 10;
  }
  return std::string(buf + pos, sizeof(buf) - pos);
}

inline std::string to_superscript(int n) {
  static const char *sup[] = {"⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"};

  if (n == 0)
    return "⁰";

  char buf[64];
  int pos = sizeof(buf);

  while (n > 0) {
    const char *s = sup[n % 10];
    int len = std::strlen(s);
    pos -= len;
    std::memcpy(buf + pos, s, len);
    n /= 10;
  }
  return std::string(buf + pos, sizeof(buf) - pos);
}

inline std::string exec_cmd(const char *cmd) {
  std::array<char, 256> buffer;
  std::string result;
  result.reserve(512);

  FILE *pipe = popen(cmd, "r");
  if (!pipe)
    return "";

  while (fgets(buffer.data(), buffer.size(), pipe))
    result.append(buffer.data());

  pclose(pipe);
  return result;
}

// -------------------- COLORES --------------------

inline const std::string RESET = "\033[0m";

inline const std::string FG_HOST = "\033[38;2;42;78;180m";
inline const std::string BG_HOST = "\033[48;2;42;78;180m";

inline const std::string FG_DIR = "\033[38;2;92;92;92m";
inline const std::string BG_DIR = "\033[48;2;92;92;92m";

inline const std::string FG_GIT_OK = "\033[38;2;0;150;75m";
inline const std::string FG_GIT_CH = "\033[38;2;205;133;63m";
inline const std::string FG_GIT = "\033[38;2;56;56;56m";
inline const std::string BG_GIT = "\033[48;2;56;56;56m";

inline const std::string FG_TIMER = "\033[38;2;60;90;130m";
inline const std::string BG_TIMER = "\033[48;2;60;90;130m";

inline const std::string FG_EXIT_OK = "\033[38;2;0;110;0m";
inline const std::string BG_EXIT_OK = "\033[48;2;0;110;0m";

inline const std::string FG_EXIT_ERR = "\033[38;2;110;0;0m";
inline const std::string BG_EXIT_ERR = "\033[48;2;110;0;0m";

inline const std::string FG_CMD = "\033[38;2;60;60;60m";
inline const std::string BG_CMD = "\033[48;2;60;60;60m";

// --------------------- ICONS ---------------------

inline const std::string ICON_OPEN_BOX = "";
inline const std::string ICON_CLOSE_BOX = "";

// -------------------- BLOQUES --------------------

inline std::string collapse_home(const std::string &cwd) {
  std::string home_dir;

  const char *home = getenv("HOME");
  if (home && *home) {
    home_dir = home;
  }

  struct passwd *pw = getpwuid(getuid());
  if (pw && pw->pw_dir) {
    home_dir = pw->pw_dir;
  }

  if (home_dir.empty()) {
    return cwd;
  }

  if (cwd == home_dir)
    return "~";

  if (cwd.size() > home_dir.size() &&
      cwd.compare(0, home_dir.size(), home_dir) == 0 &&
      cwd[home_dir.size()] == '/') {
    return "~" + cwd.substr(home_dir.size());
  }

  return cwd;
}

inline int printSource(char *argv0) {
  std::string self = argv0;

  std::cout
      << R"___(trap '[[ -z "$t0" && -n "$BASH_COMMAND" && $BASH_COMMAND != "pl_ps1"* && $BASH_COMMAND != "clear" ]] && export t0=$(date +%s%N) && export LAST_CMD=$(history 1 | sed "s/^ *[0-9]\+ *//")' DEBUG

pl_ps1() {
  local exit_code=$?
  local t1=$(date +%s%N)

  if [[ -z "$__FIRST_PROMPT_DONE" ]]; then
    PS1="$()___"
      << self <<
      R"___()"
    __FIRST_PROMPT_DONE=1
    unset t0
    unset LAST_CMD
    export INSERT_NEW_LINE=1
    return
  fi

  if [[ -n "$t0" ]]; then
    local duration_ms=$(((t1 - t0) / 1000000))
    )___"
      << self << R"___( $exit_code $duration_ms "$LAST_CMD"
  fi

  PS1="$()___"
      << self << R"___()"

  unset t0
}

PROMPT_COMMAND=pl_ps1
)___";

  return 0;
}

inline int exitAndTimer(char *argv1, char *argv2, char *argv3) {
  std::string out;
  out.reserve(1024);

  // EXIT CODE BLOCK
  long ms = std::strtol(argv2, nullptr, 10);
  int code = std::strtol(argv1, nullptr, 10);

  bool ok = (code == 0 || code == 130 || code == 148);
  const std::string &exit_bg = ok ? BG_EXIT_OK : BG_EXIT_ERR;
  const std::string &exit_fg = ok ? FG_EXIT_OK : FG_EXIT_ERR;

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

  long t = ms;
  long hours = t / 3600000;
  t %= 3600000;
  long minutes = t / 60000;
  t %= 60000;
  long seconds = t / 1000;
  long millis = t % 1000;

  char buf[64];
  if (hours > 0)
    snprintf(buf, sizeof(buf), "%ld:%02ld:%02ld.%03ld", hours, minutes, seconds,
             millis);
  else if (minutes > 0)
    snprintf(buf, sizeof(buf), "%ld:%02ld.%03ld", minutes, seconds, millis);
  else
    snprintf(buf, sizeof(buf), "%ld.%03ld", seconds, millis);

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

inline int ps1() {
  std::string out;
  out.reserve(1024);

  const char *user_env = getenv("USER");
  const char *ssh_env = getenv("SSH_CONNECTION");

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
  bool gitPresent = true;
  std::istringstream iss(
      exec_cmd("git status --porcelain --branch -uno 2>/dev/null"));
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

    if (header.rfind("## ", 0) == 0)
      header = header.substr(3);

    std::string branch = header.substr(0, header.find("..."));

    out += has_changes ? FG_GIT_CH : FG_GIT_OK;
    out += "  ";
    out += branch;

    int ahead = 0, behind = 0;

    auto pos = header.find("ahead ");
    if (pos != std::string::npos)
      ahead = std::atoi(header.c_str() + pos + 6);

    pos = header.find("behind ");
    if (pos != std::string::npos)
      behind = std::atoi(header.c_str() + pos + 7);

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
  // END GIT BLOCK

  // WORKSPACE BLOCK
  out += " ";
  out += RESET;
  out += (gitPresent ? FG_GIT : FG_DIR);
  out += ICON_CLOSE_BOX;

  out += RESET;

  out += "\n╰──$";

  const char *venv_env = getenv("VIRTUAL_ENV");
  std::string venv;
  if (venv_env) {
    const char *slash = strrchr(venv_env, '/');
    venv = slash ? slash + 1 : venv_env;
  }

  if (!venv.empty()) {
    out += "(🛠 venv:";
    out += venv;
    out += ") ";
  } else {
    const char *box_env = getenv("CONTAINER_ID");
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

// -------------------- MAIN --------------------
int main(int argc, char *argv[]) {
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
