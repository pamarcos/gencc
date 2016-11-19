ROOT = $(CURDIR)/../..
include $(ROOT)/common.mk

CXXFLAGS += -I$(CURDIR)/googletest

SRC = $(wildcard *.cpp) $(wildcard googletest/*.cpp)

OBJ = $(SRC:.cpp=.o)
OBJ := $(foreach obj,$(OBJ),$(BUILD_DIR)/$(obj))

all: run_unit_tests

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

unit_tests: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(ROOT)/$@ $(OBJ)

run_unit_tests: unit_tests
	$(ROOT)/unit_tests

clean:
	rm -rf $(BUILD_DIR)/unit_tests

.PHONY: clean run_unit_tests

