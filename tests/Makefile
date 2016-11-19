all: tests

tests: functional_tests unit_tests

clean:
	cd functional_tests && $(MAKE) $@
	cd unit_tests && $(MAKE) $@

functional_tests unit_tests:
	cd $@ && $(MAKE)

.PHONY: tests functional_tests unit_tests
