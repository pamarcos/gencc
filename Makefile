ROOT = $(shell dirname $PWD)
include common.mk

SRCS = $(wildcard src/*.cpp)
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
	rm -f *.gcda *.gcno *.html*
	rm -f $(NAME)
	cd $(TESTS_DIR) && $(MAKE) $@

tests functional_tests: $(NAME)
	cd $(TESTS_DIR) && $(MAKE) $@

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

.PHONY: clean tests functional_tests coverage all
