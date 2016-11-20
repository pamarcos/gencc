NAME = gencc
CXXFLAGS = -std=c++11 -pedantic -Wall -Wextra -I$(ROOT)/src -I$(ROOT)/third_party

DEBUG = 1
ifneq ($(DEBUG),)
CXXFLAGS += -g -O0 -fsanitize=address
else
CXXFLAGS += -02
endif

TESTS_DIR = $(ROOT)/tests

BUILD_DIR = $(ROOT)/build
SRC = $(wildcard $(ROOT)/src/*.cpp)
OBJ = $(SRC:.cpp=.o)
OBJ := $(foreach obj,$(OBJ),$(subst $(ROOT)/,,$(obj)))
OBJ := $(foreach obj,$(OBJ),$(BUILD_DIR)/$(obj))

dbg-%:
	@echo "Makefile: Value of $* = $($*)"
