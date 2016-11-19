all: tests

tests: functional_tests

clean:
	cd functional_tests && $(MAKE) $@

functional_tests:
	cd $@ && $(MAKE)

.PHONY: tests functional_tests
