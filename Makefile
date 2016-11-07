NAME = gencc
CXXFLAGS = -g -O0 -std=c++11 -Wall
CXXFLAGS += -fsanitize=address
CORES = 1
UNAME = $(shell uname)

ifeq ($(UNAME), Darwin)
CORES = $(shell sysctl -n hw.ncpu)
else ifeq ($(UNAME), Linux)
CORES = $(shell nproc)
endif

all: $(NAME)

$(NAME): gencc.cpp
	$(CXX) -o $@ $(CXXFLAGS) $<

clean:
	rm -f $(NAME)

tests: $(NAME)
	env CXX=g++ $(GENCC) make -j$(CORES) -C tests
	diff -u tests/compile_commands.json compile_commands.json

dbg-%:
	@echo "Makefile: Value of $* = $($*)"

.PHONY: clean tests all
