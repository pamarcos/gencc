ROOT = $(CURDIR)

ifeq ($(MAKECMDGOALS),coverage)
DEBUG = 1
endif

include common.mk

ifeq ($(MAKECMDGOALS),coverage)
$(OBJ): clean
endif

all: $(OUTPUT_BIN)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(OUTPUT_BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LDFLAGS)

clean: clean_coverage
	rm -rf $(BUILD_DIR)
	rm -rf coverage
	rm -f $(OUTPUT_BIN)
	rm -f $(DEP)
	cd $(TESTS_DIR) && $(MAKE) $@

check functional_tests unit_tests: clean_coverage $(OUTPUT_BIN)
	cd $(TESTS_DIR) && $(MAKE) DEBUG=1 $@

clean_coverage:
	find . -iname "*.gcda" | xargs rm -f

coverage: unit_tests functional_tests
	mkdir -p coverage
	gcovr -r . -d --html --html-details -o coverage/coverage.html -e "third_party.*" -e ".*unit_tests.*"

CLANG_TIDY_CHECKS = *
CLANG_TIDY_CHECKS += ,-cert-err58-cpp
CLANG_TIDY_CHECKS += ,-google-runtime-references
CLANG_TIDY_CHECKS += ,-cppcoreguidelines-pro-type-reinterpret-cast
CLANG_TIDY_CHECKS += ,-cert-env33-c

tidy:
	clang-tidy -checks='$(CLANG_TIDY_CHECKS)' src/gencc.cpp

cppcheck:
	cppcheck --enable=all --inconclusive src tests/unit_tests -igoogletest

.PHONY: clean check unit_tests functional_tests coverage all

-include $(DEP)
