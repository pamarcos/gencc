NAME = gencc
CXXFLAGS = -MMD -MP -std=c++11 -Wpedantic -Wall -Wextra -Werror -I$(ROOT)/src -I$(ROOT)/third_party

DEBUG = 1
ifneq ($(DEBUG),)
CXXFLAGS += -g -O0 -fsanitize=address
else
CXXFLAGS += -O2
endif

OUTPUT_BIN = $(ROOT)/$(NAME)
TESTS_DIR = $(ROOT)/tests

BUILD_DIR = $(ROOT)/build
SRC_DIR = $(ROOT)/src
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:.cpp=.o)
OBJ := $(foreach obj,$(OBJ),$(subst $(ROOT)/,,$(obj)))
OBJ := $(foreach obj,$(OBJ),$(BUILD_DIR)/$(obj))
DEP = $(OBJ:.o=.d)

dbg-%:
	@echo "Makefile: Value of $* = $($*)"
