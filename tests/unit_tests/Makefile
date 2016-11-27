ROOT = $(realpath $(CURDIR)/../..)
include $(ROOT)/common.mk

CXXFLAGS += -g -I$(CURDIR)/googletest -fsanitize=address
GTEST_CXXFLAGS := $(CXXFLAGS)
LDFLAGS = --coverage

ifneq ($(shell uname), "Darwin")
LDFLAGS += -pthread
endif

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
	$(CXX) $(GTEST_CXXFLAGS) -c -o $@ $<

$(TESTS_BUILD_DIR)/%.d: %.cpp
	@mkdir -p $(dir $@)
	@echo "Generating dependency for $<..."
	@$(CXX) -MM -MP -MF $@ $(CXXFLAGS) $<
	@sed -i.bak 's:$(subst .d,.o,$(notdir $@)):$(dir $@)$(subst .d,.o,$(notdir $@)):' $@
	@rm -f "$(@:.d=.d.bak)"
	@cat $@ | sed 's:$(subst .d,.o,$(notdir $@)):$(notdir $@):' >> $@

$(TESTS_BUILD_DIR)/%.o: %.cpp $(TESTS_BUILD_DIR)/%.d
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

unit_tests: $(UNIT_TESTS_BIN)

$(UNIT_TESTS_BIN): $(TESTS_OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ)

run_unit_tests: $(ROOT)/unit_tests
	$(UNIT_TESTS_BIN)

clean:
	rm -rf $(TESTS_BUILD_DIR)
	rm -f $(DEP)
	rm -f $(UNIT_TESTS_BIN)

.PHONY: clean run_unit_tests unit_tests

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif

