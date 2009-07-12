# $Id$

.PHONY: check check-quick check-full

TESTS = $(shell find . \( -name .svn -o -name .backup -o -name .TRASH \) -prune -o -name \*.c -print | sort )

check: check-quick

check-quick:
	@echo
	@echo "*** To run all tests in platform-model, do 'make check-full'"
	@echo
	./runtests stdio/printf1.c

check-full:
	./runtests $(TESTS)

TESTS:
	@echo $(TESTS) | xargs -n 1
