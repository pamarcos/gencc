ROOT = $(realpath $(CURDIR)/../..)
include $(ROOT)/common.mk

COMPILE_COMMANDS = compile_commands.json
EXPECTED_COMPILE_COMMANDS = $(CURDIR)/$(COMPILE_COMMANDS).exp
FUNC_TESTS_DIR = test

define compare_json
	@bash -c "diff -u <(sort $(EXPECTED_COMPILE_COMMANDS)) <(sort $(COMPILE_COMMANDS))"
endef

GENCC = $(ROOT)/$(NAME)

all: functional_tests

functional_tests:
	cd $(ROOT)

	# No arguments
	$(GENCC)

	# Wrong arguments
	$(GENCC) -asd -esd | echo "Fail expected :)"

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

clean:
	cd $(FUNC_TESTS_DIR) && $(MAKE) $@

.PHONY: functional_tests
