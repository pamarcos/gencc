ROOT = $(CURDIR)/../..
include $(ROOT)/common.mk

CXXFLAGS += -g -fsanitize=address -I$(CURDIR)/googletest

TESTS_BUILD_DIR = $(BUILD_DIR)/unit_tests

TESTS_SRC = $(wildcard *.cpp) $(wildcard googletest/*.cpp)
TESTS_OBJ = $(TESTS_SRC:.cpp=.o)
TESTS_OBJ := $(foreach obj,$(TESTS_OBJ),$(TESTS_BUILD_DIR)/$(obj))

OBJ := $(filter-out $(BUILD_DIR)/src/main.o,$(OBJ))
OBJ += $(TESTS_OBJ)

all: run_unit_tests

$(TESTS_BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

unit_tests: $(TESTS_OBJ)
	$(CXX) $(CXXFLAGS) -o $(ROOT)/$@ $(OBJ)

run_unit_tests: unit_tests
	$(ROOT)/unit_tests

clean:
	rm -rf $(TESTS_BUILD_DIR)

.PHONY: clean run_unit_tests unit_tests

