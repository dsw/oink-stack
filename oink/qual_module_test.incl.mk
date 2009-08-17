# see License.txt for copyright and terms of use

# Included makefile for testing just that qual can parse what it
# should.

ifndef QUAL_TEST_MAKEFILE
$(error This makefile should be included in qual_test.incl.mk, not used stand-alone)
endif

.PHONY: qual-module-check
qual-module-check: qual-module-check-misc qual-module-check-filter

.PHONY: qual-module-check-misc
qual-module-check-misc:
	./qual -o-module Test/mod_foo_dupl; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS)

# do a polymorphic analysis
QUALCC_FLAGS += -fq-poly

# do not do an instance-sensitive analysis
QUALCC_FLAGS += -fo-no-instance-sensitive

# FIX: We need to do a global qualifier analysis, so turn this back on
# QUALCC_FLAGS += -fo-report-link-errors

# it is more conservative to turn this off, so I do; FIX: we could
# turn it on if there is no const casting and we are trusting the
# compiler
QUALCC_FLAGS += -fq-no-use-const-subtyping

# make faster by suppressing output
QUALCC_FLAGS += -fq-no-names
QUALCC_FLAGS += -fq-no-explain-errors
QUALCC_FLAGS += -fq-no-name-with-loc

.PHONY: qual-module-check-filter
TEST_TOCLEAN += *.filter-good.c *.filter-bad.c
TEST_TOCLEAN += Test/mod_foo_hello_write_good.lattice
TEST_TOCLEAN += Test/mod_foo_hello_write_bad.lattice
qual-module-check-filter:
	@echo "$@: good"
	./test_filter -good < Test/mod_write_hello.c \
	  > Test/mod_write_hello.filter-good.c
	cd Test; ../module_make_lattice -write mod_write_hello_good mod_foo \
	  > mod_foo_hello_write_good.lattice
	cd Test; ../qual -fq-module-write $(QUALCC_FLAGS) \
	  -q-config mod_foo_hello_write_good.lattice \
	  -o-module mod_write_hello_good -o-module mod_foo \
	  mod_write_hello.filter-good.c mod_lib_foo.c
	@echo "$@: bad"
	./test_filter -bad < Test/mod_write_hello.c \
	  > Test/mod_write_hello.filter-bad.c
	cd Test; ../module_make_lattice -write mod_write_hello_bad mod_foo \
	  > mod_foo_hello_write_bad.lattice
	cd Test; ../qual -fq-module-write $(QUALCC_FLAGS) \
	  -q-config mod_foo_hello_write_bad.lattice \
	  -o-module mod_write_hello_bad -o-module mod_foo \
	  mod_write_hello.filter-bad.c mod_lib_foo.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)
