ROOT = $(realpath $(CURDIR)/../..)
include $(ROOT)/common.mk

CXXFLAGS += -I$(CURDIR)/googletest

# Do not measure coverage for unit tests themselves
CXXFLAGS := $(filter-out --coverage,$(CXXFLAGS))
LDFLAGS += --coverage

TESTS_BUILD_DIR = $(BUILD_DIR)/tests/unit_tests

TESTS_SRC = $(wildcard *.cpp)
TESTS_OBJ = $(TESTS_SRC:.cpp=.o)
TESTS_OBJ := $(foreach obj,$(TESTS_OBJ),$(TESTS_BUILD_DIR)/$(obj))
GTEST_SRC = googletest/gmock-gtest-all.cc
GTEST_OBJ = $(TESTS_BUILD_DIR)/googletest/gmock-gtest-all.o
TESTS_OBJ += $(GTEST_OBJ)

OBJ := $(filter-out $(BUILD_DIR)/src/main.o,$(OBJ))
OBJ += $(TESTS_OBJ)
DEP = $(subst .o,.d,$(filter-out $(GTEST_OBJ),$(OBJ)))

UNIT_TESTS_BIN = $(ROOT)/unit_tests

all: run_unit_tests

$(GTEST_OBJ): $(GTEST_SRC)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(TESTS_BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@

unit_tests: $(UNIT_TESTS_BIN)

$(UNIT_TESTS_BIN): $(TESTS_OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LDFLAGS) 

run_unit_tests: $(ROOT)/unit_tests
	$(UNIT_TESTS_BIN)

clean:
	rm -rf $(TESTS_BUILD_DIR)
	rm -f $(DEP)
	rm -f $(UNIT_TESTS_BIN)

.PHONY: clean run_unit_tests unit_tests

-include $(DEP)
