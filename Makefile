NAME = gencc
CXXFLAGS = -std=c++11 -Wall -Ithird_party
CXXFLAGS += -fsanitize=address
SRCS = $(wildcard src/*.cpp)
TESTS_DIR = tests

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
	env CXX=g++ CC=gcc ./$(NAME) $(MAKE) -C $(TESTS_DIR)
	diff -u $(TESTS_DIR)/compile_commands.json compile_commands.json
	./$(NAME) -cxx g++ -cc gcc $(MAKE) -C $(TESTS_DIR)
	diff -u $(TESTS_DIR)/compile_commands.json compile_commands.json
	$(MAKE) -C $(TESTS_DIR) clean
	./$(NAME) -cxx g++ -cc gcc -build $(MAKE) -C $(TESTS_DIR)
	test -f $(TESTS_DIR)/test_source1.o
	test -f $(TESTS_DIR)/test_source2.o
	test -f $(TESTS_DIR)/test_source3.o
	test -f $(TESTS_DIR)/test_source4.o
	test -f $(TESTS_DIR)/test_source5.o
	test -f $(TESTS_DIR)/test_source6.o
	test -f $(TESTS_DIR)/test_source7.o
	test -f $(TESTS_DIR)/test_source8.o

dbg-%:
	@echo "Makefile: Value of $* = $($*)"

.PHONY: clean tests all
