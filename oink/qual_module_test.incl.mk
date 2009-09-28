# see License.txt for copyright and terms of use

# Included makefile for testing just that qual can parse what it
# should.

ifndef QUAL_TEST_MAKEFILE
$(error This makefile should be included in qual_test.incl.mk, not used stand-alone)
endif

.PHONY: qual-module-check

qual-module-check: qual-module-check-misc
.PHONY: qual-module-check-misc
qual-module-check-misc:
# illegal mod spec
	./qual -o-mod-spec Test/mod_foo_dupl.mod; test $$? -eq 1
# duplicate module assignment for a file
	./qual -o-mod-spec foo:Test/mod_foo_dupl.mod; test $$? -eq 1
# check the verbose output of module_make_lattice
	./module_make_lattice --mod waga --mod zeeip 2>&1 | \
	  grep 'module_make_lattice modules: waga, zeeip'
# test that printing the class to module map works
	./oink -o-mod-spec gronk:Test/mod_gronk.mod -fo-module-print-class2mod Test/mod_gronk.ii | $(ELSA_DIR)/chop_out '---- START class to module map' '---- END class to module map' | grep 'D:Gronk gronk'
# test that multiple modules per class results in an error
	./oink -o-mod-spec gronk:Test/mod_gronk.mod -o-mod-spec gronk2:Test/mod_gronk_other.mod -fo-module-print-class2mod Test/mod_gronk.ii Test/mod_gronk_other.cc 2>&1 | grep 'class D:Gronk maps to two modules gronk2 and gronk'
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
ifndef VERBOSE
QUALCC_FLAGS += -fq-no-names
QUALCC_FLAGS += -fq-no-explain-errors
QUALCC_FLAGS += -fq-no-name-with-loc
endif

TEST_TOCLEAN += *.filter-good.c *.filter-bad.c

qual-module-check: qual-module-check-write-filter
.PHONY: qual-module-check-write-filter
TEST_TOCLEAN += Test/mod_foo_hello_write_good.lattice
TEST_TOCLEAN += Test/mod_foo_hello_write_bad.lattice
qual-module-check-write-filter:
	@echo "$@: good"
	./test_filter -good < Test/mod_write_hello.c \
	  > Test/mod_write_hello.filter-good.c
	./module_make_lattice --write \
          --mod hello --mod foo \
	  > Test/mod_foo_hello_write_good.lattice
	./qual -fq-module-write $(QUALCC_FLAGS) \
	  -q-config Test/mod_foo_hello_write_good.lattice \
	  -o-mod-spec hello:Test/mod_write_hello_good.mod \
	  -o-mod-spec foo:Test/mod_foo.mod \
	  Test/mod_write_hello.filter-good.c Test/mod_lib_foo.c
	@echo "$@: bad"
	./test_filter -bad < Test/mod_write_hello.c \
	  > Test/mod_write_hello.filter-bad.c
	./module_make_lattice --write \
          --mod hello --mod foo \
	  > Test/mod_foo_hello_write_bad.lattice
	./qual -fq-module-write $(QUALCC_FLAGS) \
	  -q-config Test/mod_foo_hello_write_bad.lattice \
	  -o-mod-spec hello:Test/mod_write_hello_bad.mod \
	  -o-mod-spec foo:Test/mod_foo.mod \
	  Test/mod_write_hello.filter-bad.c Test/mod_lib_foo.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-write-stack-filter
.PHONY: qual-module-check-write-stack-filter
TEST_TOCLEAN += Test/mod_foo_hello_write_stack_good.lattice
TEST_TOCLEAN += Test/mod_foo_hello_write_stack_bad.lattice
qual-module-check-write-stack-filter:
	@echo "$@: good"
	./test_filter -good < Test/mod_write_hello_stack.c \
	  > Test/mod_write_hello_stack.filter-good.c
	./module_make_lattice --write \
          --mod hello --mod foo \
	  > Test/mod_foo_hello_write_stack_good.lattice
	./qual -fq-module-write $(QUALCC_FLAGS) \
	  -q-config Test/mod_foo_hello_write_stack_good.lattice \
	  -o-mod-spec hello:Test/mod_write_hello_stack_good.mod \
	  -o-mod-spec foo:Test/mod_foo.mod \
	  Test/mod_write_hello_stack.filter-good.c Test/mod_lib_foo.c
	@echo "$@: bad"
	./test_filter -bad < Test/mod_write_hello_stack.c \
	  > Test/mod_write_hello_stack.filter-bad.c
	./module_make_lattice --write \
          --mod hello --mod foo \
	  > Test/mod_foo_hello_write_stack_bad.lattice
	./qual -fq-module-write $(QUALCC_FLAGS) \
	  -q-config Test/mod_foo_hello_write_stack_bad.lattice \
	  -o-mod-spec hello:Test/mod_write_hello_stack_bad.mod \
	  -o-mod-spec foo:Test/mod_foo.mod \
	  Test/mod_write_hello_stack.filter-bad.c Test/mod_lib_foo.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-access-filter
.PHONY: qual-module-check-access-filter
TEST_TOCLEAN += Test/mod_foo_hello_access_good.lattice
TEST_TOCLEAN += Test/mod_foo_hello_access_bad.lattice
qual-module-check-access-filter:
	@echo "$@: good"
	./test_filter -good < Test/mod_access_hello.c \
	  > Test/mod_access_hello.filter-good.c
	./module_make_lattice --access \
          --mod hello --mod foo \
	  > Test/mod_foo_hello_access_good.lattice
	./qual -fq-module-access $(QUALCC_FLAGS) \
	  -q-config Test/mod_foo_hello_access_good.lattice \
	  -o-mod-spec hello:Test/mod_access_hello_good.mod \
	  -o-mod-spec foo:Test/mod_foo.mod \
	  Test/mod_access_hello.filter-good.c Test/mod_lib_foo.c
	@echo "$@: bad"
	./test_filter -bad < Test/mod_access_hello.c \
	  > Test/mod_access_hello.filter-bad.c
	./module_make_lattice --access \
          --mod hello --mod foo \
	  > Test/mod_foo_hello_access_bad.lattice
	./qual -fq-module-access $(QUALCC_FLAGS) \
	  -q-config Test/mod_foo_hello_access_bad.lattice \
	  -o-mod-spec hello:Test/mod_access_hello_bad.mod \
	  -o-mod-spec foo:Test/mod_foo.mod \
	  Test/mod_access_hello.filter-bad.c Test/mod_lib_foo.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-access-lib_foo_simple1
.PHONY: qual-module-check-access-lib_foo_simple1
TEST_TOCLEAN += lib_foo_simple1.lattice
qual-module-check-access-lib_foo_simple1:
	@echo "$@"
	./module_make_lattice --access --mod hello --mod foo --mod default \
	  > Test/lib_foo_simple1.lattice
	./qual -q-config Test/lib_foo_simple1.lattice -fq-module-access \
	  $(QUALCC_FLAGS) \
	  -o-mod-spec hello:Test/lib_foo_simple1_hello.mod \
	  -o-mod-spec foo:Test/lib_foo_simple1_foo.mod \
	  -o-mod-default default \
	  Test/lib_foo_simple1.i 2>&1 | \
          grep -e 'lib_foo.c:19 WARNING (1 of 1): z treated as $$hello_alloc and $$hello_otherAccess'
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-trust-filter
.PHONY: qual-module-check-trust-filter
TEST_TOCLEAN += Test/mod_bar_hello_trust_good.lattice
TEST_TOCLEAN += Test/mod_bar_hello_trust_bad.lattice
qual-module-check-trust-filter:
	@echo "$@: good"
	./test_filter -good < Test/mod_trust_hello.c \
	  > Test/mod_trust_hello.filter-good.c
	./module_make_lattice --trust \
          --mod hello --mod bar \
	  > Test/mod_bar_hello_trust_good.lattice
	./qual -fq-module-trust $(QUALCC_FLAGS) \
	  -q-config Test/mod_bar_hello_trust_good.lattice \
	  -o-mod-spec hello:Test/mod_trust_hello_good.mod \
	  -o-mod-spec bar:Test/mod_bar.mod \
	  Test/mod_trust_hello.filter-good.c Test/mod_trust_bar.c
	@echo "$@: bad"
	./test_filter -bad < Test/mod_trust_hello.c \
	  > Test/mod_trust_hello.filter-bad.c
	./module_make_lattice --trust \
          --mod hello --mod bar \
	  > Test/mod_bar_hello_trust_bad.lattice
	./qual -fq-module-trust $(QUALCC_FLAGS) \
	  -q-config Test/mod_bar_hello_trust_bad.lattice \
	  -o-mod-spec hello:Test/mod_trust_hello_bad.mod \
	  -o-mod-spec bar:Test/mod_bar.mod \
	  Test/mod_trust_hello.filter-bad.c Test/mod_trust_bar.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-stack-access
.PHONY: qual-module-check-stack-access
TEST_TOCLEAN += Test/mod_waga_stack_access.lattice
qual-module-check-stack-access:
	./module_make_lattice --access \
          --mod waga --mod default \
	  > Test/mod_waga_stack_access.lattice
	./qual -fq-module-access $(QUALCC_FLAGS) \
	  -q-config Test/mod_waga_stack_access.lattice \
	  -o-mod-spec waga:Test/mod_waga.mod \
	  -o-mod-default default \
	  Test/hello_stack_access.c Test/waga_stack_access.c 2>&1

# | \
          grep -e 'Test/mod_baz.cc:5 WARNING (1 of 1): z treated as $$baz_alloc and $$baz_otherAccess'
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-access-array
.PHONY: qual-module-check-access-array
TEST_TOCLEAN += Test/mod_gronk_baz_array.lattice
qual-module-check-access-array:
	./module_make_lattice --access \
          --mod gronk --mod baz \
	  > Test/mod_gronk_baz_array.lattice
	./qual -fq-module-access $(QUALCC_FLAGS) \
	  -q-config Test/mod_gronk_baz_array.lattice \
	  -o-mod-spec gronk:Test/mod_gronk.mod \
	  -o-mod-spec baz:Test/mod_baz.mod \
	  Test/mod_gronk_baz_array.ii 2>&1 | \
          grep -e 'qual: Test/mod_baz.cc:5: class D:Gronk2 allocated in module baz but defined in module gronk'
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-stack-alloc-class
.PHONY: qual-module-check-stack-alloc-class
TEST_TOCLEAN += Test/mod_gronk_baz.lattice
qual-module-check-stack-alloc-class:
	./module_make_lattice --access \
          --mod gronk --mod baz \
	  > Test/mod_gronk_baz.lattice
	./qual -fq-module-access $(QUALCC_FLAGS) \
	  -q-config Test/mod_gronk_baz.lattice \
	  -o-mod-spec gronk:Test/mod_gronk.mod \
	  -o-mod-spec baz:Test/mod_baz.mod \
	  Test/mod_gronk_baz.ii 2>&1 | \
          grep -e 'qual: Test/mod_baz.cc:4: class D:Gronk allocated in module baz but defined in module gronk'
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-access-class
.PHONY: qual-module-check-access-class
TEST_TOCLEAN += Test/mod_gronk_baz_access_class.lattice
qual-module-check-access-class:
	./module_make_lattice --access \
          --mod gronk --mod baz --mod default \
	  > Test/mod_gronk_baz_access_class.lattice
	./qual -fq-module-access $(QUALCC_FLAGS) \
	  -q-config Test/mod_gronk_baz_access_class.lattice \
	  -o-mod-spec gronk:Test/mod_gronk.mod \
	  -o-mod-spec baz:Test/mod_baz.mod \
	  -o-mod-default default \
	  Test/mod_gronk_baz_access_class.ii 2>&1 | \
          grep -e 'Test/mod_baz.cc:4 WARNING (1 of 1):  (new struct Gronk1  ()) treated as $$gronk_alloc and $$gronk_otherAccess'
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-access-class-member
.PHONY: qual-module-check-access-class-member
TEST_TOCLEAN += Test/mod_gronk_baz2.lattice
qual-module-check-access-class-member:
	./module_make_lattice --access \
          --mod gronk --mod baz \
	  > Test/mod_gronk_baz2.lattice
	./qual -fq-module-access $(QUALCC_FLAGS) \
	  -q-config Test/mod_gronk_baz2.lattice \
	  -o-mod-spec gronk:Test/mod_gronk.mod \
	  -o-mod-spec baz:Test/mod_baz.mod \
	  Test/mod_gronk_baz2.ii 2>&1 | \
          grep -e 'Test/mod_baz.cc:5 WARNING (1 of 1): q treated as $$gronk_alloc and $$gronk_otherAccess'
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-new-across-mod
.PHONY: qual-module-check-new-across-mod
TEST_TOCLEAN += Test/mod_gronk_baz_new.lattice
qual-module-check-new-across-mod:
	./module_make_lattice --access \
          --mod gronk --mod baz --mod default \
	  > Test/mod_gronk_baz_new.lattice
	./qual -fq-module-access $(QUALCC_FLAGS) \
	  -q-config Test/mod_gronk_baz_new.lattice \
	  -o-mod-spec gronk:Test/mod_gronk.mod \
	  -o-mod-spec baz:Test/mod_baz.mod \
	  -o-mod-default default \
	  Test/mod_gronk_baz_new.ii 2>&1 | \
          grep -e 'qual: Test/mod_baz.cc:6: class D:Gronk2 allocated in module baz but defined in module gronk'
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-method
.PHONY: qual-module-check-method
TEST_TOCLEAN += Test/mod_gronk_baz_method.lattice
qual-module-check-method:
	./module_make_lattice --access \
          --mod gronk --mod baz --mod default \
	  > Test/mod_gronk_baz_method.lattice
	./qual -fq-module-access $(QUALCC_FLAGS) \
	  -q-config Test/mod_gronk_baz_method.lattice \
	  -o-mod-spec gronk:Test/mod_gronk.mod \
	  -o-mod-spec baz:Test/mod_baz.mod \
	  -o-mod-default default \
	  Test/mod_gronk_baz_method.ii 2>&1
	$(ANNOUNCE_TEST_PASS)

qual-module-check: qual-module-check-method2
.PHONY: qual-module-check-method2
TEST_TOCLEAN += Test/mod_gronk_baz_method2.lattice
qual-module-check-method2:
	./module_make_lattice --access \
          --mod gronk --mod baz --mod default \
	  > Test/mod_gronk_baz_method2.lattice
	./qual -fq-module-access $(QUALCC_FLAGS) \
	  -q-config Test/mod_gronk_baz_method2.lattice \
	  -o-mod-spec gronk:Test/mod_gronk.mod \
	  -o-mod-spec baz:Test/mod_baz.mod \
	  -o-mod-default default \
	  Test/mod_gronk_baz_method2.ii 2>&1 | \
          grep -e 'Test/mod_baz.cc:4 WARNING (1 of 1):  (new struct Gronk1  ()) treated as $$gronk_alloc and $$gronk_otherAccess'
	$(ANNOUNCE_TEST_PASS)
