ROOT = $(CURDIR)
include common.mk

ifeq ($(MAKECMDGOALS),coverage)
$(OUTPUT_BIN) $(OBJ): clean
CXXFLAGS += --coverage
endif

all: $(OUTPUT_BIN)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(OUTPUT_BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.html*
	rm -f $(OUTPUT_BIN)
	rm -f $(DEP)
	cd $(TESTS_DIR) && $(MAKE) $@

tests functional_tests unit_tests: $(OUTPUT_BIN)
	cd $(TESTS_DIR) && $(MAKE) $@

clean_coverage:
	find . -iname "*.gcda" | xargs rm -f

coverage: clean_coverage unit_tests functional_tests
	gcovr -r . --html --html-details -o coverage.html -e "third_party.*" -e ".*unit_tests.*"

CLANG_TIDY_CHECKS = *
CLANG_TIDY_CHECKS += ,-cert-err58-cpp
CLANG_TIDY_CHECKS += ,-google-runtime-references
CLANG_TIDY_CHECKS += ,-cppcoreguidelines-pro-type-reinterpret-cast
CLANG_TIDY_CHECKS += ,-cert-env33-c

tidy:
	clang-tidy -checks='$(CLANG_TIDY_CHECKS)' src/gencc.cpp

.PHONY: clean tests unit_tests functional_tests coverage all

-include $(DEP)
