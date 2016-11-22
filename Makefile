ROOT = $(CURDIR)
include common.mk

ifeq ($(MAKECMDGOALS),coverage)
$(OUTPUT_BIN) $(OBJ): clean
CXXFLAGS += --coverage
endif

all: $(OUTPUT_BIN)

$(BUILD_DIR)/%.d: %.cpp
	@mkdir -p $(dir $@)
	@echo "Generating dependency for $<..."
	@$(CXX) -MM -MP -MF $@ $(CXXFLAGS) $<
	@sed -i.bak 's:$(subst .d,.o,$(notdir $@)):$(dir $@)$(subst .d,.o,$(notdir $@)):' $@
	@rm -f "$(@:.d=.d.bak)"
	@cat $@ | sed 's:$(subst .d,.o,$(notdir $@)):$(notdir $@):' >> $@

$(BUILD_DIR)/%.o: %.cpp $(BUILD_DIR)/%.d
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OUTPUT_BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.gcda *.gcno *.html*
	rm -f $(OUTPUT_BIN)
	rm -f $(DEP)
	cd $(TESTS_DIR) && $(MAKE) $@

tests functional_tests unit_tests: $(OUTPUT_BIN)
	cd $(TESTS_DIR) && $(MAKE) $@

coverage: unit_tests
	# Workaround for gcovr failing if source code is not in the build folder
	cp tests/unit_tests/*.cpp $(BUILD_DIR)/tests/unit_tests
	gcovr -r . --html --html-details -o coverage.html -e "third_party.*" -e ".*unit_tests.*"

CLANG_TIDY_CHECKS = *
CLANG_TIDY_CHECKS += ,-cert-err58-cpp
CLANG_TIDY_CHECKS += ,-google-runtime-references
CLANG_TIDY_CHECKS += ,-cppcoreguidelines-pro-type-reinterpret-cast
CLANG_TIDY_CHECKS += ,-cert-env33-c

tidy:
	clang-tidy -checks='$(CLANG_TIDY_CHECKS)' src/gencc.cpp

.PHONY: clean tests unit_tests functional_tests coverage all

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif
