#pragma once

#include <string>

// -------------------- COLORES --------------------

inline const std::string RESET = "\033[0m";

inline const std::string FG_HOST = "\033[38;2;42;78;180m";
inline const std::string BG_HOST = "\033[48;2;42;78;180m";

inline const std::string FG_DIR = "\033[38;2;92;92;92m";
inline const std::string BG_DIR = "\033[48;2;92;92;92m";

inline const std::string FG_GIT_OK = "\033[38;2;0;150;75m";
inline const std::string FG_GIT_CH = "\033[38;2;205;133;63m";
inline const std::string FG_GIT	   = "\033[38;2;56;56;56m";
inline const std::string BG_GIT	   = "\033[48;2;56;56;56m";

inline const std::string FG_TIMER = "\033[38;2;60;90;130m";
inline const std::string BG_TIMER = "\033[48;2;60;90;130m";

inline const std::string FG_EXIT_OK = "\033[38;2;0;110;0m";
inline const std::string BG_EXIT_OK = "\033[48;2;0;110;0m";

inline const std::string FG_EXIT_ERR = "\033[38;2;110;0;0m";
inline const std::string BG_EXIT_ERR = "\033[48;2;110;0;0m";

inline const std::string FG_CMD = "\033[38;2;60;60;60m";
inline const std::string BG_CMD = "\033[48;2;60;60;60m";

// --------------------- ICONS ---------------------

inline const std::string ICON_OPEN_BOX	= "";
inline const std::string ICON_CLOSE_BOX = "";