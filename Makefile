NAME = gencc
CXXFLAGS = -g -O0 -std=c++11 -Wall
CXXFLAGS += -fsanitize=address
UNAME = $(shell uname)

all: $(NAME)

$(NAME): gencc.cpp
	$(CXX) -o $@ $(CXXFLAGS) $<

clean:
	rm -f $(NAME)

tests: $(NAME)
	env CXX=g++ $(GENCC) $(MAKE) -C tests
	diff -u tests/compile_commands.json compile_commands.json

dbg-%:
	@echo "Makefile: Value of $* = $($*)"

.PHONY: clean tests all
