ROOT = $(CURDIR)/../..
include $(ROOT)/common.mk

CXXFLAGS += -I$(CURDIR)/googletest
GTEST_CXXFLAGS := $(CXXFLAGS)
CXXFLAGS += -fsanitize=address --coverage

TESTS_BUILD_DIR = $(BUILD_DIR)/tests/unit_tests

TESTS_SRC = $(wildcard *.cpp)
TESTS_OBJ = $(TESTS_SRC:.cpp=.o)
TESTS_OBJ := $(foreach obj,$(TESTS_OBJ),$(TESTS_BUILD_DIR)/$(obj))
GTEST_SRC = googletest/gmock-gtest-all.cc
GTEST_OBJ = $(TESTS_BUILD_DIR)/googletest/gmock-gtest-all.o
TESTS_OBJ += $(GTEST_OBJ)

OBJ := $(filter-out $(BUILD_DIR)/src/main.o,$(OBJ))
OBJ += $(TESTS_OBJ)

UNIT_TESTS_BIN = $(ROOT)/unit_tests

all: run_unit_tests

$(GTEST_OBJ): $(GTEST_SRC)
	@mkdir -p $(shell dirname $@)
	$(CXX) $(GTEST_CXXFLAGS) -c -o $@ $<

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

