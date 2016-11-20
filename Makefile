ROOT = $(CURDIR)
include common.mk

ifeq ($(MAKECMDGOALS),tests)
$(OUTPUT_BIN): clean
CXXFLAGS += -fsanitize=address
endif

ifeq ($(MAKECMDGOALS),coverage)
$(OUTPUT_BIN): clean
CXXFLAGS += --coverage
endif

all: $(OUTPUT_BIN)

$(OUTPUT_BIN):
	cd $(SRC_DIR) && $(MAKE)

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.gcda *.gcno *.html*
	cd $(SRC_DIR) && $(MAKE) $@
	cd $(TESTS_DIR) && $(MAKE) $@

tests functional_tests unit_tests: $(OUTPUT_BIN)
	cd $(TESTS_DIR) && $(MAKE) $@

coverage: tests
	gcovr -r . --html --html-details -o coverage.html

CLANG_TIDY_CHECKS = *
CLANG_TIDY_CHECKS += ,-cert-err58-cpp
CLANG_TIDY_CHECKS += ,-google-runtime-references
CLANG_TIDY_CHECKS += ,-cppcoreguidelines-pro-type-reinterpret-cast
CLANG_TIDY_CHECKS += ,-cert-env33-c
#CLANG_TIDY_FILTER = '[{"name":"src/gencc.cpp", "lines":[[19,1000]]}]'

tidy:
	clang-tidy -checks='$(CLANG_TIDY_CHECKS)' src/gencc.cpp

.PHONY: clean tests unit_tests functional_tests coverage all
