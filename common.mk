NAME = gencc
CXXFLAGS = -std=c++11 -pedantic -Wall -Wextra -Ithird_party
COMPILE_COMMANDS = compile_commands.json
TESTS_DIR = $(ROOT)/tests

dbg-%:
	@echo "Makefile: Value of $* = $($*)"