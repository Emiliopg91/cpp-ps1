#include "bash.hpp"

#include <iostream>
#include <string>

int printSource(char* argv0) {
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