ROOT = $(CURDIR)/..
include $(ROOT)/common.mk

all: $(OUTPUT_BIN)

$(BUILD_DIR)/src/%.o: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OUTPUT_BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ)
	rm -f $(OUTPUT_BIN)

.PHONY: clean run_unit_tests unit_tests

