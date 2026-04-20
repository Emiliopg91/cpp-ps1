#pragma once

#include <string>

int ps1();

std::string collapse_home(const std::string& cwd);

std::string get_git_prompt_segment(bool& gitPresent);

std::string to_subscript(int n);

std::string to_superscript(int n);

std::string exec_cmd(const char* cmd);