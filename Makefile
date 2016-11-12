NAME = gencc
CXXFLAGS = -std=c++11 -Wpedantic -Ithird_party
SRCS = $(wildcard src/*.cpp)
TESTS_DIR = tests

DEBUG = 1

ifneq ($(DEBUG),)
CXXFLAGS += -g -O0
else
CXXFLAGS += -02
endif

ifeq ($(MAKECMDGOALS),verify)
$(NAME): clean
CXXFLAGS += -g -fsanitize=address
endif

ifeq ($(MAKECMDGOALS),coverage)
$(NAME): clean
CXXFLAGS += -g -fsanitize=address --coverage
endif

all: $(NAME)

$(NAME): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f compile_commands.json
	rm -f *.gcda *.gcno *.html*
	rm -f $(NAME)

define compare_json
	@bash -c "diff -u <(sort $(TESTS_DIR)/compile_commands.json) <(sort compile_commands.json)"
endef

tests: clean $(NAME)
	# No arguments
	./$(NAME)

	# Wrong arguments
	./$(NAME) -asd -esd || echo "Fail expected :)"

	# Simple case
	./$(NAME) $(MAKE) -C $(TESTS_DIR)

	# Wrong target
	./$(NAME) $(MAKE) -C $(TESTS_DIR) asdasd

	# Passing CXX and CC through env var
	rm -f compile_commands.json
	env CXX=g++ CC=gcc ./$(NAME) $(MAKE) -C $(TESTS_DIR)
	$(call compare_json)

	# Passing CXX and CC through arguments
	rm -f compile_commands.json
	./$(NAME) -cxx g++ -cc gcc $(MAKE) -C $(TESTS_DIR)
	$(call compare_json)

	# Passing CXX and CC through both env var and arguments
	rm -f compile_commands.json
	env CXX=clang++ CC=clang ./$(NAME) -cxx g++ -cc gcc $(MAKE) -C $(TESTS_DIR)
	$(call compare_json)

	# Passing CXX and CC through both env var and arguments
	rm -f compile_commands.json
	env CXX=clang++ CC=clang ./$(NAME) -cxx g++ -cc gcc -o compile_commands.json $(MAKE) -C $(TESTS_DIR)
	$(call compare_json)

	# Using multiple jobs to force collisions
	rm -f compile_commands.json
	./$(NAME) -cxx g++ -cc gcc make -j8 -C $(TESTS_DIR)
	$(call compare_json)

	# Retries, fallback and build arguments
	rm -f compile_commands.json
	$(MAKE) -C $(TESTS_DIR) clean
	./$(NAME) -cxx g++ -cc gcc -build -r 10 -f 100 $(MAKE) -C $(TESTS_DIR)
	$(call compare_json)
	test -f $(TESTS_DIR)/test_source1.o
	test -f $(TESTS_DIR)/test_source2.o
	test -f $(TESTS_DIR)/test_source3.o
	test -f $(TESTS_DIR)/test_source4.o
	test -f $(TESTS_DIR)/test_source5.o
	test -f $(TESTS_DIR)/test_source6.o
	test -f $(TESTS_DIR)/test_source7.o
	test -f $(TESTS_DIR)/test_source8.o

coverage: tests
	gcovr -r . --html --html-details -o coverage.html

CLANG_TIDY_CHECKS = *
CLANG_TIDY_CHECKS += ,-cert-err58-cpp
CLANG_TIDY_CHECKS += ,-google-runtime-references
CLANG_TIDY_CHECKS += ,-cppcoreguidelines-pro-type-reinterpret-cast
CLANG_TIDY_CHECKS += ,-cert-env33-c
#CLANG_TIDY_FILTER = '[{"name":"src/gencc.cpp", "lines":[[19,1000]]}]'

tidy:
	clang-tidy -checks='$(CLANG_TIDY_CHECKS)' src/gencc.cpp

dbg-%:
	@echo "Makefile: Value of $* = $($*)"

.PHONY: clean tests coverage all
