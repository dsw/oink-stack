# see License.txt for copyright and terms of use

# Included makefile for testing just that qual can parse what it
# should.

ifndef QUAL_TEST_MAKEFILE
$(error This makefile should be included in qual_test.incl.mk, not used stand-alone)
endif

.PHONY: qual-module-check
qual-module-check:
	./qual -o-module Test/foo_dupl; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS)
