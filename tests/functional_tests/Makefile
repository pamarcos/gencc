ROOT = $(realpath $(CURDIR)/../..)
include $(ROOT)/common.mk

COMPILE_COMMANDS = compile_commands.json
EXPECTED_COMPILE_COMMANDS = $(CURDIR)/$(COMPILE_COMMANDS).exp
EXPECTED_COMPILE_COMMANDS_REAL = $(EXPECTED_COMPILE_COMMANDS).real
EXPECTED_COMPILE_COMMANDS_SC = $(CURDIR)/$(COMPILE_COMMANDS).sc.exp
EXPECTED_COMPILE_COMMANDS_SC_REAL = $(EXPECTED_COMPILE_COMMANDS).sc.real
SRC_DIR = src

define compare_json
	@bash -c "diff -u <(sort $(EXPECTED_COMPILE_COMMANDS_REAL)) <(sort $(COMPILE_COMMANDS))"
endef

define compare_json_sc
	@bash -c "diff -u <(sort $(EXPECTED_COMPILE_COMMANDS_SC_REAL)) <(sort $(COMPILE_COMMANDS))"
endef

GENCC = $(ROOT)/$(NAME)

all: functional_tests

functional_tests:
	cd $(ROOT)

	# Generate expected output independently of the project folder
	sed 's:$$PWD:$(CURDIR):g' $(EXPECTED_COMPILE_COMMANDS) > $(EXPECTED_COMPILE_COMMANDS_REAL)
	sed 's:$$PWD:$(CURDIR):g' $(EXPECTED_COMPILE_COMMANDS_SC) > $(EXPECTED_COMPILE_COMMANDS_SC_REAL)

	# No arguments
	$(GENCC) | echo "Fail expected :)"

	# Wrong arguments
	$(GENCC) -asd -esd | echo "Fail expected :)"

	# Simple project
		# Simple case
		$(GENCC) $(MAKE) -C $(SRC_DIR) -f Makefile.simple | echo "Fail expected :)"

		# Wrong target
		$(GENCC) $(MAKE) -C $(SRC_DIR) -f Makefile.simple asdasd | echo "Fail expected :)"

		# Wrong GENCC_OPTIONS
		env GENCC_OPTIONS=asdasd $(GENCC) $(MAKE) -C $(SRC_DIR) -f Makefile.simple | echo "Fail expected :)"

		# Passing CXX and CC through env var
		rm -f $(COMPILE_COMMANDS)
		env CXX=g++ CC=gcc $(GENCC) $(MAKE) -C $(SRC_DIR) -f Makefile.simple
		$(call compare_json)

		# Passing CXX and CC through arguments
		rm -f $(COMPILE_COMMANDS)
		$(GENCC) -cxx g++ -cc gcc $(MAKE) -C $(SRC_DIR) -f Makefile.simple
		$(call compare_json)

		# Passing CXX and CC through both env var and arguments
		rm -f $(COMPILE_COMMANDS)
		env CXX=clang++ CC=clang $(GENCC) -cxx g++ -cc gcc $(MAKE) -C $(SRC_DIR) -f Makefile.simple
		$(call compare_json)

		# Passing CXX and CC through both env var and arguments
		rm -f $(COMPILE_COMMANDS)
		env CXX=clang++ CC=clang $(GENCC) -cxx g++ -cc gcc -o $(COMPILE_COMMANDS) $(MAKE) -C $(SRC_DIR) -f Makefile.simple
		$(call compare_json)

		# Using multiple jobs to force collisions
		rm -f $(COMPILE_COMMANDS)
		$(GENCC) -cxx g++ -cc gcc make -j4 -C $(SRC_DIR) -f Makefile.simple
		$(call compare_json)

		# Not enough shared memory
		rm -f $(COMPILE_COMMANDS)
		$(GENCC) -cxx g++ -cc gcc -m 1024 make -j4 -C $(SRC_DIR) -f Makefile.simple | echo "Fail expected :)"

		# Build argument
		rm -f $(COMPILE_COMMANDS)
		$(MAKE) -C $(SRC_DIR) -f Makefile.simple clean
		$(GENCC) -cxx g++ -cc gcc -m 4096 -build $(MAKE) -C $(SRC_DIR) -f Makefile.simple
		$(call compare_json)
		test -f $(SRC_DIR)/test_source1.cpp.o
		test -f $(SRC_DIR)/test_source2.cpp.o
		test -f $(SRC_DIR)/test_source3.cpp.o
		test -f $(SRC_DIR)/test_source4.cpp.o
		test -f $(SRC_DIR)/test_source5.c.o
		test -f $(SRC_DIR)/test_source6.c.o
		test -f $(SRC_DIR)/test_source7.c.o
		test -f $(SRC_DIR)/test_source8.c.o

	# Dependencies project
	rm -f $(COMPILE_COMMANDS)
	$(GENCC) -cxx g++ -cc gcc $(MAKE) -C $(SRC_DIR) -f Makefile.dependencies
	$(call compare_json)

	# Single call
	rm -f $(COMPILE_COMMANDS)
	$(GENCC) -cxx g++ -cc gcc $(MAKE) -C $(SRC_DIR) -f Makefile.single_call
	$(call compare_json_sc)

	# Single call disorder
	rm -f $(COMPILE_COMMANDS)
	$(GENCC) -cxx g++ -cc gcc $(MAKE) -C $(SRC_DIR) -f Makefile.single_call_disorder
	$(call compare_json_sc)

clean:
	rm -f $(SRC_DIR)/*.o $(SRC_DIR)/*.d

.PHONY: functional_tests
