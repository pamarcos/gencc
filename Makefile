NAME = gencc
CXXFLAGS = -g -O0 -std=c++11 -Wall
CXXFLAGS += -fsanitize=address

all: $(NAME)

$(NAME): gencc.cpp
	$(CXX) -o $@ $(CXXFLAGS) $<

clean:
	rm -f $(NAME)

tests: $(NAME)
	env CXX=g++ $(GENCC) make -C tests
	diff -u tests/compile_commands.json compile_commands.json

dbg-%:
	@echo "Makefile: Value of $* = $($*)"

.PHONY: clean tests all
