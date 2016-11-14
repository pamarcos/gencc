ROOT = $(shell dirname $(PWD))
include $(ROOT)/common.mk

GENCC = $(ROOT)/$(NAME)
FUNC_TESTS_DIR = $(TESTS_DIR)/functional_tests

all: tests

clean:
	rm -f $(COMPILE_COMMANDS)
	rm -Rf *.o

define compare_json
	@bash -c "diff -u <(sort $(FUNC_TESTS_DIR)/$(COMPILE_COMMANDS)) <(sort $(COMPILE_COMMANDS))"
endef

tests: functional_tests

functional_tests: clean $(GENCC)
	cd $(ROOT)

	# No arguments
	$(GENCC)

	# Wrong arguments
	$(GENCC) -asd -esd || echo "Fail expected :)"

	# Simple case
	$(GENCC) $(MAKE) -C $(FUNC_TESTS_DIR)

	# Wrong target
	$(GENCC) $(MAKE) -C $(FUNC_TESTS_DIR) asdasd

	# Passing CXX and CC through env var
	rm -f $(COMPILE_COMMANDS)
	env CXX=g++ CC=gcc $(GENCC) $(MAKE) -C $(FUNC_TESTS_DIR)
	$(call compare_json)

	# Passing CXX and CC through arguments
	rm -f $(COMPILE_COMMANDS)
	$(GENCC) -cxx g++ -cc gcc $(MAKE) -C $(FUNC_TESTS_DIR)
	$(call compare_json)

	# Passing CXX and CC through both env var and arguments
	rm -f $(COMPILE_COMMANDS)
	env CXX=clang++ CC=clang $(GENCC) -cxx g++ -cc gcc $(MAKE) -C $(FUNC_TESTS_DIR)
	$(call compare_json)

	# Passing CXX and CC through both env var and arguments
	rm -f $(COMPILE_COMMANDS)
	env CXX=clang++ CC=clang $(GENCC) -cxx g++ -cc gcc -o $(COMPILE_COMMANDS) $(MAKE) -C $(FUNC_TESTS_DIR)
	$(call compare_json)

	# Using multiple jobs to force collisions
	rm -f $(COMPILE_COMMANDS)
	$(GENCC) -cxx g++ -cc gcc make -j8 -C $(FUNC_TESTS_DIR)
	$(call compare_json)

	# Retries, fallback and build arguments
	rm -f $(COMPILE_COMMANDS)
	$(MAKE) -C $(FUNC_TESTS_DIR) clean
	$(GENCC) -cxx g++ -cc gcc -build -r 10 -f 100 $(MAKE) -C $(FUNC_TESTS_DIR)
	$(call compare_json)
	test -f $(FUNC_TESTS_DIR)/test_source1.o
	test -f $(FUNC_TESTS_DIR)/test_source2.o
	test -f $(FUNC_TESTS_DIR)/test_source3.o
	test -f $(FUNC_TESTS_DIR)/test_source4.o
	test -f $(FUNC_TESTS_DIR)/test_source5.o
	test -f $(FUNC_TESTS_DIR)/test_source6.o
	test -f $(FUNC_TESTS_DIR)/test_source7.o
	test -f $(FUNC_TESTS_DIR)/test_source8.o

.PHONY: tests functional_tests