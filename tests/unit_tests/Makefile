ROOT = $(CURDIR)/../..
include $(ROOT)/common.mk

CXXFLAGS += -g -fsanitize=address -I$(CURDIR)/googletest

TESTS_BUILD_DIR = $(BUILD_DIR)/unit_tests

TESTS_SRC = $(wildcard *.cpp) $(wildcard googletest/*.cpp)
TESTS_OBJ = $(TESTS_SRC:.cpp=.o)
TESTS_OBJ := $(foreach obj,$(TESTS_OBJ),$(TESTS_BUILD_DIR)/$(obj))

OBJ := $(filter-out $(BUILD_DIR)/src/main.o,$(OBJ))
OBJ += $(TESTS_OBJ)

UNIT_TESTS_BIN = $(ROOT)/unit_tests

all: run_unit_tests

$(TESTS_BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(UNIT_TESTS_BIN): $(TESTS_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

run_unit_tests: $(ROOT)/unit_tests
	$(UNIT_TESTS_BIN)

clean:
	rm -rf $(TESTS_BUILD_DIR)
	rm -f $(UNIT_TESTS_BIN)

.PHONY: clean run_unit_tests

