NAME = gencc
CXXFLAGS += -MMD -MP -std=c++11 -pedantic -Wall -Wextra -Werror -I$(ROOT)/src -I$(ROOT)/third_party -I$(ROOT)/third_party/cross-platform-cpp/sync

ifeq ($(DEBUG),1)
CXXFLAGS += -g -O0 --coverage
else
CXXFLAGS += -O2
endif

# AddressSanitizer is not available in Windows
ifeq ($(shell uname -s),Linux)
LDFLAGS += -pthread -lrt
CXXFLAGS += -fsanitize=address
else ifeq ($(shell uname -s),Darwin)
CXXFLAGS += -fsanitize=address
endif

OUTPUT_BIN = $(ROOT)/$(NAME)
TESTS_DIR = $(ROOT)/tests

BUILD_DIR = $(ROOT)/build
SRC_DIR = $(ROOT)/src
SRC = $(wildcard $(SRC_DIR)/*.cpp)
SRC += $(wildcard $(ROOT)/third_party/cross-platform-cpp/**/*.cpp)
OBJ = $(SRC:.cpp=.o)
OBJ := $(foreach obj,$(OBJ),$(subst $(ROOT)/,,$(obj)))
OBJ := $(foreach obj,$(OBJ),$(BUILD_DIR)/$(obj))
DEP = $(OBJ:.o=.d)

dbg-%:
	@echo "Makefile: Value of $* = $($*)"
