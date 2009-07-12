# see License.txt for copyright and terms of use

# Included makefile for testing the oink empty analysis proper which
# exhibits the basic functionality shared among all the analysis
# tools.

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

.PHONY: oink-check
oink-check: oink-check-misc
oink-check: oink-check-control
oink-check: oink-check-parse-c
oink-check: oink-check-parse-gcc
oink-check: oink-check-parse-kandr
oink-check: oink-check-link
oink-check: oink-check-digraph-component
oink-check: oink-check-func-gran

# This is almost completely redundant with check-pretty
# oink-check: oink-check-parse

# This is redundant with qual-check-resrz
# oink-check: oink-check-pretty

oink-check-pretty/%: 

# quarl 2006-05-26 known parse/dataflow failures
PARSE_XFAIL := 

oink-check: oink-check-nested-no-def-constr1
oink-check-nested-no-def-constr1:
	./oink Test/nested_no_def_constr1.cc

# 53f8abcb-a6e1-40a9-9b95-53adb8aa3b86
# dsw: I have crawled around in elsa and I cannot find where operator
# int *&() is resolved; if I could I could find where to introduce the
# overloading.
PARSE_XFAIL += t0148.cc

PARSE_XFAIL += t0149.cc

# c312d480-2c8d-43ee-9d9d-13d900e51aa3
PARSE_XFAIL += t0151.cc

# I don't know why this is off, but the qual serialization tests are
# testing the xml quite thoroughly.
# oink-check: oink-check-xml

oink-check: oink-check-extern-unused-undefined

.PHONY: oink-check-misc
oink-check-misc:
# FIX: turn this on when we turn on LibCpdInit again
# check that no-size arrays initialized by compound initializers get their size set correctly
# 	./oink -o-lang ANSI_C99 -fo-print-typed-ast Test/nosize_array_init.c \
#           | grep '\]$$' | perl -lpe 's/^\s*//' > Test/nosize_array_init.c.out
# 	diff -u Test/nosize_array_init.c.cor Test/nosize_array_init.c.out
# check that two gotos to the same target work
	./oink Test/double_goto.i
# check that 'char foo[] = "boink";' makes foo a 'char*' not a 'char const*'
	./oink Test/init_from_array_const.i
# check we deal with pseudoinstantiation type tags
	./oink Test/atom_type_tag.cc
# check variables not added to the list of vars for a translation unit
# if they contain an error type
	./oink Test/error_var1.cc
# check that TypeFactory_O::setCVQualifiers() can subtract flags as
# well as add them
	./oink Test/set_cv.cc
# check that if you use a typedef to make, say, a const array, that we
# don't barf on that
	./oink Test/const_array.cc
# check TypeFactory_O::applyCVToType unions instead of replacing
	./oink Test/applycv1.cc
# check the 'suffix' language flag works
	./oink -o-lang SUFFIX Test/bool.i; test $$? -eq 4
	./oink -o-lang SUFFIX Test/bool.ii
# check that leaving out an argument to a flag doesn't segfault
	./oink -o-lang; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-const-array2
oink-check-const-array2:
	./oink Test/const_array2.cc
	$(ANNOUNCE_TEST_PASS)

TEST_TOCLEAN += Test/*.out.ctl
.PHONY: oink-check-control
oink-check-control: oink-check-control/Test/simple1.ctl
.PHONY: oink-check-control/%
oink-check-control/%: %
	./oink -o-control $* -fo-print-controls-and-exit | $(ELSA_DIR)/chop_out '---- CONTROL_START ----' '---- CONTROL_STOP ----' > $*.1.out.ctl
	@if ! test -s $*.1.out.ctl; then echo "File is empty: $*.1.out.ctl"; exit 1; fi
	./oink -o-control $*.1.out.ctl -fo-print-controls-and-exit < $*.1.out.ctl | $(ELSA_DIR)/chop_out '---- CONTROL_START ----' '---- CONTROL_STOP ----' > $*.2.out.ctl
	diff -c $*.1.out.ctl $*.2.out.ctl
	$(ANNOUNCE_TEST_PASS)

.PHONY: oink-check-parse-c
oink-check-parse-c:
# check that parsing C works; control experiment
	./oink -o-lang GNU_Cplusplus $(ELSA_DIR)/in/c/t0001.c; test $$? -eq 2
# check that parsing C works
	./oink -o-lang KandR_C $(ELSA_DIR)/in/c/t0001.c
# check that parsing K&R function definitions works
	./oink -fo-exit-after-parse -o-lang KandR_C Test/kandrfunc1.c
	./oink -fo-exit-after-parse -o-lang KandR_C Test/kandrfunc2.c
# check that we can have newlines in string literals
	./oink -fo-exit-after-parse Test/strlit_newline1.c
# check you can typedef C++ keywords
	./oink -o-lang KandR_C -fo-exit-after-parse Test/c_keyword_as_cpp_id.i
	$(ANNOUNCE_TEST_PASS)

# check things about gcc-C
.PHONY: oink-check-parse-gcc
oink-check-parse-gcc:
# check "_Bool" means "bool" in C mode
	./oink -o-lang GNU_C Test/gcc_Bool.i
	./oink -o-lang GNU_C Test/gcc_Bool_member.i
	./oink -o-lang GNU_C Test/gcc2_Bool.i; test ! $$? -eq 0
# but not for gcc 2
	./oink -o-lang GNU2_KandR_C Test/gcc_Bool.i; test ! $$? -eq 0
	./oink -o-lang GNU2_KandR_C Test/gcc2_Bool.i
# and not in C++ mode
	./oink -o-lang GNU_Cplusplus Test/gcc_Bool.i; test ! $$? -eq 0
	./oink -o-lang GNU_Cplusplus Test/gcc_Bool_member.i; test ! $$? -eq 0
# check __assert_fail
	./oink -o-lang GNU_C Test/gcc_assert_fail.i
	./oink -o-lang GNU_C Test/gcc_assert_fail2.i
# check __FUNCTION__ and __PRETTY_FUNCTION__
	./oink -o-lang GNU_C Test/gcc_pretty.i
# check '?:'
	./oink Test/cond_omit_then.cc
# check multiple semi-colons in a struct/class member list
	./oink Test/multi_semi_member.cc
# check parses __builtin_va_arg
	./oink -o-lang GNU_C Test/builtin_va_arg.i
# check nested functions
	./oink Test/nested_func.cc
# check stack allocated arrays are allowed to have their size
# determined at runtime
	./oink Test/dynam_size_stack_arr.i
# check that open arrays (arrays of no size in a struct) are allowed
	./oink Test/open_array.cc
# check that __asm parses
	./oink Test/asm.i
# check that parameter variables are cloned correctly
	./oink Test/clone1.cc
	$(ANNOUNCE_TEST_PASS)

oink-check-parse-gcc: oink-check-parse-gcc1
oink-check-parse-gcc1:
# check '<?' and '?>'
	./oink Test/bin_minmax1.cc
	$(ANNOUNCE_TEST_PASS)

oink-check-parse-gcc: oink-check-parse-gcc2
oink-check-parse-gcc2:
# check parsing templates
	./oink Test/template_constr1.cc
	$(ANNOUNCE_TEST_PASS)

oink-check-parse-gcc: oink-check-parse-too-many-initializers
oink-check-parse-too-many-initializers:
	./oink Test/too_many_initializers1.c
	$(ANNOUNCE_TEST_PASS)

# check some very specific K and R things
.PHONY: oink-check-parse-kandr
oink-check-parse-kandr:
# check you can call an undeclared function
	./oink -o-lang KandR_C Test/kandr_call_undeclared_func.i
# and not in ansi C
# sm: "ansi C" means C89 to Elsa, and you can call undeclared functions in C89
#	./oink -o-lang ANSI_C99 Test/kandr_call_undeclared_func.i; test $$? -eq 4
# check K and R function definitions work
	./oink -o-lang KandR_C Test/kandr_func_def.i
# check in K&R C *sometimes* a function need not provide its
# parameters every time
	./oink -o-lang KandR_C Test/kandr_empty_args_def.i
# check if K&R function declarations where one of the parameters is a
# function type will typecheck; this required turning off initializers
# for declarators
	./oink -o-lang KandR_C Test/kandr_func_param.c
# check in C (only K&R ?) an omitted return type in a function
# definition defaults to int
	./oink -o-lang KandR_C Test/impl_ret_int.i
# check in K&R C an omitted return type in a function definition
# defaults to int in a K&R-style function definition
	./oink -o-lang KandR_C Test/impl_ret_int_kandr.i
# check k and r implicit in parameters
	./oink -o-lang KandR_C Test/kandr_impl_int_param.c
# check implicit-int stuff
	./oink -o-lang KandR_C Test/impl_int_s_decl.c
	./oink -o-lang KandR_C Test/impl_int_tf_decl.c
	./oink -o-lang KandR_C Test/impl_int_param_decl.c
	./oink -o-lang KandR_C Test/impl_int_fail1.c; test 0 -ne $$?
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-mix-c-cxx
oink-check-mix-c-cxx:
	./oink Test/operator_bracket.ii Test/empty.i
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-repeated-file
oink-check-repeated-file:
	./oink Test/repeated.i Test/repeated.i 2>&1 | grep 'input file name repeated'
	$(ANNOUNCE_TEST_PASS)

# The intent is that elsa_tests.gen.incl.mk will get generated and
# then checked in.  The invariant is that when you check out and run
# 'make check' that the tests pass.  I want the user to have to run
# this target manually if elsa/regrtest changes, but I want it run
# automatically if elsa_test_omit.files changes; if it were to just
# get generated each time, what would happen is that a new test would
# get added to elsa/regrtest and then suddenly the oink tests would be
# broken.
#
# Howver, if you have to manually run 'make elsa_tests.gen.incl.mk'
# when elsa/regrtest changes then when you do you can see that a new
# test has been added that breaks the build *then* and ether fix it
# then before checking in again, or filter it out.  When the invariant
# is satisfied again, then check in the new elsa_tests.gen.incl.mk.
# For the above reason, I do *not* have elsa_tests.gen.incl.mk depend
# on $(ELSA_DIR)/regrtest.
#
# FIX: I don't know why when I run this target the action runs and
# then I am told that elsa_tests.gen.incl.mk is up to date.
#
# This dependency is causing problems; just remember to run it.
# elsa_tests.gen.incl.mk: elsa_tests_omit.files
elsa_tests.gen.incl.mk:
# dsw: What the hell did I put this in for?  It makes no sense to me now.
# 	@echo "$@ has been modified and checked-in so see those diffs before re-running this"
# 	exit 1
	rm -f $@
	echo '# see License.txt for copyright and terms of use' > $@
	echo >> $@
	./elsa_tests_get -omit elsa_tests_omit.files -regrtest $(ELSA_DIR)/regrtest >> $@
	chmod a-w $@
clean/elsa_tests.gen.incl.mk: clean/%:
	rm -f $*

IN_TESTS :=
-include elsa_tests.gen.incl.mk
IN_TESTS := $(addprefix $(ELSA_DIR)/in/,$(IN_TESTS))

.PHONY: oink-check-parse-start
oink-check-parse-start:; @echo "oink-check-parse ****************"
# some more parsing tests
	./oink Test/min1_d0008.cc
# FIX: In oink, this fails with the correct error message, but in
# elsa/ccparse does not
#	./oink Test/no_candidate1.cc

.PHONY: oink-check-parse
oink-check-parse: oink-check-parse-start
oink-check-parse: $(addprefix oink-check-parse/,$(IN_TESTS))
.PHONY: oink-check-parse/%
oink-check-parse/%:
	./oink $*


PRETTY_TESTS := $(IN_TESTS)

.PHONY: oink-check-pretty-start
oink-check-pretty-start:; @echo "oink-check-pretty ****************"

.PHONY: oink-check-pretty
oink-check-pretty: oink-check-pretty-start
oink-check-pretty: $(addprefix oink-check-pretty/,$(PRETTY_TESTS))
.PHONY: oink-check-pretty/%
oink-check-pretty/%:
	./oink -fo-pretty-print $* > /dev/null

oink-check-extern-unused-undefined:
	./oink -fo-report-link-errors Test/extern_unused_undefined.cc
	./oink -fo-report-link-errors Test/extern_undefined1.cc ; test $$? -eq 8
	./oink -fo-report-link-errors Test/extern_undefined1.cc ; test $$? -eq 8
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-compound-init10
oink-check-compound-init10:
	./oink Test/compoundinit10.c
	$(ANNOUNCE_TEST_PASS)

# this now gives a warning just as gcc does, though not the same
# warning
oink-check: oink-check-compound-init11
oink-check-compound-init11:
	./oink Test/compoundinit11.c
	$(ANNOUNCE_TEST_PASS)

# This is not legal input and we do not guarantee to work on non-legal
# input.
# oink-check: oink-check-compound-init12
# oink-check-compound-init12:
# 	./oink Test/compoundinit12.c
# 	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-compound-init13
oink-check-compound-init13:
	./oink Test/compoundinit13.c
	$(ANNOUNCE_TEST_PASS)

# dsw: in my opinion gcc should not accept this; it certainly is wrong
# unless there is some rule that I don't know about; now we give a
# warning
oink-check: oink-check-compound-init14
oink-check-compound-init14:
	./oink Test/compoundinit14.c
	$(ANNOUNCE_TEST_PASS)

# dsw: in my opinion gcc should not accept this; it certainly is wrong
# unless there is some rule that I don't know about; now we give a
# warning
oink-check: oink-check-compound-init15
oink-check-compound-init15:
	./oink Test/compoundinit15.c
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-default-from-typename
oink-check-default-from-typename:
	./oink Test/default_from_typename1.cc
	./oink Test/default_from_typename2.cc
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-template-default
oink-check-template-default:
	./oink Test/template_default1.cc
	./oink Test/template_default2.cc
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-cast-array-init1
oink-check-cast-array-init1:
	./oink Test/cast_array_init1.cc
	$(ANNOUNCE_TEST_PASS)

oink-check-link: oink-check-link1
oink-check-link1:
	./oink -fo-report-link-errors Test/implicit1.cc Test/implicit2.cc
	./oink Test/implicit1.cc -o-srz Test/implicit1.odir
	./oink Test/implicit2.cc -o-srz Test/implicit2.odir
	./oink -fo-report-link-errors Test/implicit1.odir Test/implicit2.odir
	$(ANNOUNCE_TEST_PASS)

oink-check-link: oink-check-link2
oink-check-link2:
	./oink -fo-report-link-errors Test/link_inline1a.cc Test/link_inline1b.cc
	./oink Test/link_inline1a.cc -o-srz Test/link_inline1a.odir
	./oink Test/link_inline1b.cc -o-srz Test/link_inline1b.odir
	./oink -fo-report-link-errors Test/link_inline1a.odir Test/link_inline1b.odir
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-namespace-static
oink-check-namespace-static:
	./oink Test/namespace_static1.cc
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-oldstyle-too-many-args
oink-check-oldstyle-too-many-args:
	./oink Test/oldstyle-too-many-args1.c
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-template-class-member-srz
oink-check-template-class-member-srz:
	./oink Test/template_class_member_srz1.cc -o-srz Test/template_class_member_srz1.odir
	$(ANNOUNCE_TEST_PASS)

# TODO: add to elsa tests when this passes so we get re-srz automatically

# c9e8f15c-0e47-4852-b7f1-1ede70a102dd
# XFAIL += oink-check-template-extern-srz1
oink-check: oink-check-template-extern-srz1
oink-check-template-extern-srz1:
	./oink Test/template_extern_srz1.cc -o-srz Test/template_extern_srz1.odir
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-unsound-if-used
oink-check-unsound-if-used:
	(./oink Test/warn_unsound1.cc -o-control Test/warn_unsound1.ctl 2>&1) | grep 'Unsoundness warning:'
	(./oink Test/warn_unsound2.cc -o-control Test/warn_unsound1.ctl 2>&1) | grep 'Unsoundness warning:' ; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS)


oink-check: oink-check-inline
oink-check-inline: oink-check-inline1 oink-check-inline2 oink-check-inline3
oink-check-inline: oink-check-inline4 oink-check-inline5 oink-check-inline6
oink-check-inline1:
	./oink -fo-report-link-errors Test/inline1a.cc Test/inline1b.cc
	$(ANNOUNCE_TEST_PASS)

oink-check-inline2:
	./oink -fo-report-link-errors Test/inline2a.cc Test/inline2b.cc
	$(ANNOUNCE_TEST_PASS)

oink-check-inline3:
	./oink -fo-report-link-errors Test/inline3a.cc Test/inline3b.cc
	$(ANNOUNCE_TEST_PASS)

oink-check-inline4:
	./oink -fo-report-link-errors Test/inline4a.cc Test/inline4b.cc ; test $$? -eq 8
	$(ANNOUNCE_TEST_PASS)

oink-check-inline5:
	./oink -fo-report-link-errors Test/inline5a.c Test/inline5b.c
	$(ANNOUNCE_TEST_PASS)

oink-check-inline6:
	./oink -fo-report-link-errors Test/inline6a.c Test/inline6b.c ; test $$? -eq 8
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-link-template-function-inst1
oink-check-link-template-function-inst1:
	./oink Test/link_template_function_inst1a.cc Test/link_template_function_inst1b.cc -fo-report-link-errors
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-inconsistent-implicit1
oink-check-inconsistent-implicit1:
	./oink Test/inconsistent_implicit1a.cc Test/inconsistent_implicit1b.cc ; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-link-unsat1
oink-check-link-unsat1:
	./oink Test/oink_link_unsat1.cc -fo-report-link-errors ; test $$? -eq 8
	./oink Test/oink_link_unsat1.cc -o-srz Test/oink_link_unsat1.odir
	./oink Test/oink_link_unsat1.odir -fo-report-link-errors ; test $$? -eq 8
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-link-pure-virtual1
oink-check-link-pure-virtual1:
# 	echo "ONE ****"
# 	./oink -fo-trace-link -fo-report-link-errors Test/link_pure_virtual1.cc
# 	echo "TWO ****"
# 	./oink -fo-trace-link -fo-report-link-errors Test/link_pure_virtual1.cc -o-srz Test/link_pure_virtual1.odir
# 	echo "THREE ****"
# 	./oink -fo-trace-link -fo-report-link-errors Test/link_pure_virtual1.odir
	./oink -fo-report-link-errors Test/link_pure_virtual1.cc
	./oink -fo-report-link-errors Test/link_pure_virtual1.cc -o-srz Test/link_pure_virtual1.odir
	./oink -fo-report-link-errors Test/link_pure_virtual1.odir
	$(ANNOUNCE_TEST_PASS)

qual-check-link-pure-virtual1:
	$(QUALCFG) -fo-report-link-errors Test/link_pure_virtual1.cc
	$(QUALCFG) -fo-report-link-errors Test/link_pure_virtual1.cc -o-srz Test/link_pure_virtual1.qdir
	$(QUALCFG) -fo-report-link-errors Test/link_pure_virtual1.qdir
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-link-typeof-undefined1
oink-check-link-typeof-undefined1:
	./oink Test/typeof_undefined1.c -fo-report-link-errors
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-link-outlined-template-dtor1
oink-check-link-outlined-template-dtor1:
	./oink Test/outlined_template_dtor1.cc -fo-report-link-errors
	$(ANNOUNCE_TEST_PASS)

XFAIL += oink-check-link-builtin1
oink-check-link-builtin1:
	./oink Test/builtin1.cc -fo-report-link-errors
	$(ANNOUNCE_TEST_PASS)

XFAIL += $(addprefix oink-check-parse/$(ELSA_DIR)/in/,$(PARSE_XFAIL))

oink-check: oink-check-weak1
oink-check-weak1:
	./oink Test/weak1a.cc Test/weak1b.cc -fo-report-link-errors ; test $$? -eq 8
	./oink Test/weak1a.cc Test/weak1b.cc -fo-report-link-errors -o-control Test/weak1.ctl
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-weak2
oink-check-weak2:
	./oink Test/weak1a.cc -o-srz Test/weak1a.odir
	./oink Test/weak1b.cc -o-srz Test/weak1b.odir
	./oink Test/weak1a.odir Test/weak1b.odir -fo-report-link-errors -o-control Test/weak2.ctl
	$(ANNOUNCE_TEST_PASS)

# **** check computing the function granularity control flow graph

oink-check-digraph-component:
	./digraph_component --root=foo Test/test1.digraph > Test/test1.digraph.out
	diff Test/test1.digraph.cor Test/test1.digraph.out
	rm -f Test/test1.digraph.out

FUNC_GRAN_TEST :=
FUNC_GRAN_TEST += Test/func_gran1.cc
FUNC_GRAN_TEST += Test/func_gran2.cc
FUNC_GRAN_TEST += Test/func_gran3.cc
FUNC_GRAN_TEST += Test/func_gran4.cc
FUNC_GRAN_TEST += Test/func_gran5.cc
# gap
FUNC_GRAN_TEST += Test/func_gran8.cc
FUNC_GRAN_TEST += Test/func_gran9.cc

# just test it runs without failing and doesn't change
TEST_TOCLEAN += $(addsuffix .fgcfg,$(FUNC_GRAN_TEST))
oink-check-func-gran: $(addprefix oink-check-func-gran/,$(FUNC_GRAN_TEST))
oink-check-func-gran/%:
# the output order is not canonical and therefore I weaken the test by
# running it through the dependency generator before checking it
# against a saved version; to make the output canonical would slow it
# down
	./oink -fo-func-gran $* | \
          $(ELSA_DIR)/chop_out '---- START ---- fg-CFG' '---- STOP ---- fg-CFG' > $*.fgcfg
	./digraph_component --root=super-main $*.fgcfg > $*.vars
# this will fail in general since the output order is not canonical
#	diff $*.fgcfg.cor $*.fgcfg
	diff $*.vars.cor $*.vars
	./oink $* -o-func-filter $*.vars -o-srz $*.odir
	./oink $*.odir -fo-all-pass-filter -o-func-filter $*.vars
	rm -f $*.fgcfg
	rm -f $*.vars
	rm -rf $*.odir

# print it as a DOT graph so they are easy to visually inspect; this
# is not a test that is run; it is just here for convenience of seeing
# the graphs
TEST_TOCLEAN += $(addsuffix .ps,$(FUNC_GRAN_TEST)) $(addsuffix .dot,$(FUNC_GRAN_TEST))
oink-check-func-gran-dot: $(addprefix oink-check-func-gran-dot/,$(FUNC_GRAN_TEST))
$(addprefix oink-check-func-gran-dot/,$(FUNC_GRAN_TEST)): oink-check-func-gran-dot/%:
	./oink -fo-func-gran -fo-func-gran-dot $* | \
        $(ELSA_DIR)/chop_out '---- START ---- fg-CFG-dot' '---- STOP ---- fg-CFG-dot' > $*.dot
	dot -Tps -o $*.ps $*.dot

# **** check-exclude-extra-star-amp analysis

OINK_XXSA := ./oink -fo-exclude-extra-star-amp

XXSA_GOOD :=
XXSA_GOOD += Test/hello.cc
XXSA_GOOD += Test/exclude_star_amp_good1.c

XXSA_BAD :=
XXSA_BAD += Test/exclude_amp_star_bad1.c
XXSA_BAD += Test/exclude_star_amp_bad1.c

oink-check: oink-check-xxsa

.PHONY: oink-check-xxsa
oink-check-xxsa: $(addprefix oink-check-xxsa-good/,$(XXSA_GOOD))
oink-check-xxsa: $(addprefix oink-check-xxsa-bad/,$(XXSA_BAD))

.PHONY: oink-check-xxsa-good/%
oink-check-xxsa-good/%:
#	$(OINK_XXSA) $* -fo-pretty-print -fq-print-trans-oink -fo-verbose 
	$(OINK_XXSA) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: oink-check-xxsa-bad/%
oink-check-xxsa-bad/%:
#	$(OINK_XXSA) $* -fo-pretty-print -fq-print-trans-oink -fo-verbose 
	$(OINK_XXSA) $* ; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS) 

# ****

oink-check: oink-check-virtual4
oink-check-virtual4:
	./oink Test/virtual4.cc
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-virtual5
oink-check-virtual5:
	./oink -fo-report-link-errors Test/virtual5.cc
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-virtual6
oink-check-virtual6:
	./oink -fo-report-link-errors Test/virtual6.cc
	$(ANNOUNCE_TEST_PASS)

XFAIL += oink-check-virtual7
oink-check-virtual7:
	./oink -fo-report-link-errors Test/virtual7.cc
	$(ANNOUNCE_TEST_PASS)

XFAIL += oink-check-virtual7b
oink-check-virtual7b:
	./oink -fo-report-link-errors Test/virtual7b.cc
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-template-inner-default1
oink-check-template-inner-default1:
	./oink Test/template_inner_default1.cc
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-link-static1
oink-check-link-static1:
	./oink -fo-report-link-errors Test/link_static1.cc ; test $$? -eq 8
	$(ANNOUNCE_TEST_PASS)

XFAIL += oink-check-link-static2
oink-check-link-static2:
	./oink -fo-report-link-errors Test/link_static2.cc
	$(ANNOUNCE_TEST_PASS)

XFAIL += oink-check-operator-assign1
oink-check-operator-assign1:
	./oink -fo-report-link-errors Test/operator_assign1.cc
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-oldstyle-ellipsis1
oink-check-oldstyle-ellipsis1:
	./oink Test/oldstyle-ellipsis1.c
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-inconsistent-function1
oink-check-inconsistent-function1:
	./oink -fo-report-link-errors Test/inconsistent_function1a.c Test/inconsistent_function1b.cc
	$(ANNOUNCE_TEST_PASS)

# Check that "foo" and "__builtin_cos" do not appear in the value file.
#
# Note: using "!" for negation is not portable across shells.
oink-check: oink-check-usedindataflow1
oink-check-usedindataflow1:
	./oink Test/usedindataflow1.cc -o-srz Test/usedindataflow1.odir
	if grep foo Test/usedindataflow1.odir/value.xml; then false; else true; fi
	if grep __builtin_cos Test/usedindataflow1.odir/value.xml; then false; else true; fi
	rm -rf Test/usedindataflow1.odir
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-anon-struct1
oink-check-anon-struct1:
	./oink -fo-report-link-errors Test/anon_struct1a.cc Test/anon_struct1b.cc
	./oink Test/anon_struct1a.cc -o-srz Test/anon_struct1a.odir
	./oink Test/anon_struct1b.cc -o-srz Test/anon_struct1b.odir
	./oink -fo-report-link-errors Test/anon_struct1a.odir Test/anon_struct1b.odir
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-anon-struct2
oink-check-anon-struct2:
	./oink -fo-report-link-errors Test/anon_struct2a.cc Test/anon_struct2b.cc
	./oink Test/anon_struct2a.cc -o-srz Test/anon_struct2a.odir
	./oink Test/anon_struct2b.cc -o-srz Test/anon_struct2b.odir
	./oink -fo-report-link-errors Test/anon_struct2a.odir Test/anon_struct2b.odir
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-anon-struct3
oink-check-anon-struct3:
	./oink -fo-report-link-errors Test/anon_struct3a.cc
	./oink Test/anon_struct3a.cc -o-srz Test/anon_struct3a.odir
	./oink -fo-report-link-errors Test/anon_struct3a.odir
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-attralias2
oink-check-attralias2:
	./oink Test/attralias2a.c Test/attralias2b.c -fo-report-link-errors
	./oink Test/attralias2a.c -o-srz Test/attralias2a.odir
	./oink Test/attralias2b.c -o-srz Test/attralias2b.odir
	./oink Test/attralias2a.odir Test/attralias2b.odir -fo-report-link-errors
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-template-impl-addrof-func1
oink-check-template-impl-addrof-func1:
	./oink -fo-report-link-errors Test/template_impl_addrof_func1.cc
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-template-instant1
oink-check-template-instant1:
	./oink -fo-report-link-errors Test/template_instant1a.cc Test/template_instant1b.cc
	./oink Test/template_instant1a.cc -o-srz Test/template_instant1a.odir
	./oink Test/template_instant1b.cc -o-srz Test/template_instant1b.odir
	./oink -fo-report-link-errors Test/template_instant1a.odir Test/template_instant1b.odir
	rm -rf Test/template_instant1a.odir Test/template_instant1b.odir
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-extern-template1
oink-check-extern-template1:
	./oink -fo-report-link-errors Test/extern_template1.cc
	./oink -fo-report-link-errors Test/extern_template1.cc -o-srz Test/extern_template1.odir
	./oink -fo-report-link-errors Test/extern_template1.odir
	$(ANNOUNCE_TEST_PASS)

oink-check: oink-check-pretty-print

# TODO: test pretty-printing of all test files.
oink-check-pretty-print: 

XFAIL += oink-check-pretty-print1
#oink-check-pretty-print: oink-check-pretty-print1
oink-check-pretty-print1:
	./oink -fo-pretty-print Test/template_func8.cc

## This will be solved using libstdc++ 
##
# oink-check-operator-newdel1:
# 	./oink -fo-report-link-errors Test/operator_newdel1.cc

# XML_TESTS := Test/hello.cc

# .PHONY: oink-check-xml-start
# oink-check-xml-start:; @echo "oink-check-xml ****************"

# .PHONY: oink-check-xml
# oink-check-xml: oink-check-xml-start
# oink-check-xml: $(addprefix oink-check-xml/,$(XML_TESTS))
# .PHONY: oink-check-xml/%
# oink-check-xml/%:
# 	./oink -fo-linker-vis-to-xml $* > /dev/null
