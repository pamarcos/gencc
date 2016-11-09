NAME = gencc
CXXFLAGS = -std=c++11 -Wall -Ithird_party
CXXFLAGS += -fsanitize=address
UNAME = $(shell uname)
SRCS = $(wildcard src/*.cpp)

DEBUG = 1

ifneq ($(DEBUG),)
CXXFLAGS += -g -O0
else
CXXFLAGS += -02
endif

all: $(NAME)

$(NAME): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(NAME)

tests: $(NAME)
	rm -f compile_commands.json
	env CXX=g++ ./$(NAME) $(MAKE) -C tests
	diff -u tests/compile_commands.json compile_commands.json

dbg-%:
	@echo "Makefile: Value of $* = $($*)"

.PHONY: clean tests all
