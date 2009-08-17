# see License.txt for copyright and terms of use

# Included makefile for driving all the tests of qual.  Further
# includes other makefiles for this purpose.

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

# this variable allows us to check in other makefiles that they are
# not being run stand-alone
QUAL_TEST_MAKEFILE := something-other-than-empty-string

# quarl 2006-07-20
#    This variable would allow a default config file; make sure all qual
#    calls explicitly set the config file (don't allow user's default config
#    file).
QUALCC_CONFIG=

.PHONY: qual-check

include qual_parse_test.incl.mk
qual-check: qual-check-parse-c
# this is redundant with qual-check-pretty-idem
# qual-check: qual-check-parse
# this is redundant with qual-check-pretty-idem
# qual-check: qual-check-pretty
qual-check: qual-check-pretty-idem
qual-check: qual-check-pretty-diff
# dsw: this is so slow I am tempted to turn it off
qual-check: qual-check-resrz

include qual_result_test.incl.mk
qual-check: qual-check-good
qual-check: qual-check-good-c
qual-check: qual-check-bad
qual-check: qual-check-bad-c
qual-check: qual-check-overload
qual-check: qual-check-op-overload
qual-check: qual-check-filter-good
qual-check: qual-check-filter-bad
qual-check: qual-check-filter-userbad
qual-check: qual-check-main-extern
qual-check: qual-check-casts-preserve-func
qual-check: qual-check-extern-inline
qual-check: qual-check-unprototyped
qual-check: qual-check-unprototyped5
qual-check: qual-check-unprototyped-vararg
qual-check: qual-check-unprototyped6
qual-check: qual-check-int-as-voidstar-srz
qual-check: qual-check-int-as-voidstar-srz-is
qual-check: qual-check-ellipsis-as-voidstar-srz
qual-check: qual-check-ellipsis-as-voidstar-srz-is
qual-check: qual-check-resrz-return-func
qual-check: qual-check-srz-funcptr-c
qual-check: qual-check-srz-const-cast-func
qual-check: qual-check-poly
qual-check: qual-check-function-pointer-in-array1
qual-check: qual-check-function-pointer-in-array2
qual-check: qual-check-struct-statement-expr1
qual-check: qual-check-mismatched-funcptr-param1
qual-check: qual-check-inconsistent-func-type-param1
#qual-check: qual-check-inconsistent-func-type-param2
qual-check: qual-check-keywords1
qual-check: qual-check-noparam-compound1
qual-check: qual-check-nonfunc-cast-to-func
qual-check: qual-check-funccast-to-nonfunc1
qual-check: qual-check-instance
qual-check: qual-check-instance-srz
qual-check: qual-check-func-gran
qual-check: qual-check-func-gran-L

include qual_serialization_test.incl.mk
qual-check: qual-check-srz

# FIX: dsw: This makefile is broken so I have commented it out for
# now.
# include qual_test_big.incl.mk

# in this file
qual-check: qual-check-misc
qual-check: qual-check-multi
qual-check: qual-check-multi-virtual
qual-check: qual-check-prelude
# FIX: turn back on
# qual-check: qual-check-regression-my

include qual_module_test.incl.mk
qual-check: qual-module-check

# check miscellaneous stuff ****

TEST_TOCLEAN += Test/*.out
.PHONY: qual-check-misc
qual-check-misc:
	@echo "$@ ****************"
# check that if you use an illegal combination it tells you that
	$(QUALCFG) -fq-print-trans-qual Test/func3.c; test $$? -eq 1
# control experiment for further tests
	$(QUALCFG) Test/tb2000fail.c; test $$? -eq 32
# check -fq-no-inference flag.
	$(QUAL) -fq-no-inference Test/tb2000fail.c
# check -fq-no-ret-inf flag: don't return the inference result
	$(QUALCFG) -fq-no-ret-inf Test/tb2000fail.c
# check -fq-print-trans-qual; FIX: need a better way to test this
# 	$(QUALCFG) -fo-pretty-print -fq-print-trans-qual -fq-no-ret-inf \
# 	  Test/tb2040taint.c | $(ELSA_DIR)/chop_out > Test/tb2040taint.c.out
# 	diff -u Test/tb2040taint.c.correct Test/tb2040taint.c.out
# check we don't crash; need a better test for this
	$(QUALCFG) -fq-no-ret-inf -fo-pretty-print -fq-print-trans-qual Test/taint1.c
# just check that -fq-print-trans-qual doesn't fail on C++
	$(QUALCFG) -o-lang GNU_Cplusplus -fo-pretty-print -fq-print-trans-qual Test/print_trans_qual1.cc
# check that without -fq-strict-const, const inference is allowed
	$(QUALCFG) Test/strict_const1.c
# check that with -fq-strict-const, const inference is not allowed, as with most compilers
	$(QUALCFG) -fq-strict-const Test/strict_const1.c; test $$? -eq 32
# check that -fo-no-print-startstop doesn't let you use it incorrectly
	$(QUALCFG) -fo-pretty-print -fo-no-print-startstop \
          Test/tb2000fail.c Test/t1211simple.c; test $$? -eq 1
# check that we fail if you try to put edges between two functions one
# of which has an ellipsis and the other of which does not
# UPDATE: as a consequence of how we handle FF_NO_PARAM_INFO functions
# we sometimes create ellipses on FunctionValues that did not have one
# before, and so we have to tolerate this circumstance
#	$(QUALCFG) Test/ellipsis1.c; test $$? -eq 1
# check we generate this warning when attempting to assign edges
# between two function types that contain ellipses
# UPDATE: This warning is off at Karl's request.  It was rather
# imprecise and there is already a warning for function declarations
# that contain an ellipsis but not polymorphic qualifiers.
# 	$(QUALCFG) Test/ellipsis2.c 2>&1 | \
#           grep 'Edges not being inserted between ellipsis variables of two functions otherwise having edges inserted between them'
# check -q-catch-quals; control experiment
	$(QUALCFG) Test/exception2.cc
# check -q-catch-quals; test; NOTE: Doubling the '$' because 'make' eats one.
	$(QUALCFG) -q-catch-qual '$$untainted' Test/exception2.cc; test $$? -eq 32
# check you can't make a pointer to member this way: &(A::*x)
	$(QUALCFG) Test/ptr-to-member1M.cc; test $$? -eq 4
# check you can't make a pointer to member out of a static member
	$(QUALCFG) Test/ptr-to-member1S.cc; test $$? -eq 1
	$(QUALCFG) Test/ptr-to-member1Sd.cc; test $$? -eq 1
# check Scott's declaration typechecker won't allow pointer to reference ...
	$(QUALCFG) Test/ptr-to-memberI.cc; test $$? -eq 4
# or void members
	$(QUALCFG) Test/ptr-to-memberI2.cc; test $$? -eq 4
# check that if you take the size of a sizeless array that it produces an error
	./oink Test/sizelessarray1.cc; test $$? -eq 4
# check that if you try to do overloading in C that you get a failure
# FIX: this test no longer fails because in K&R C I allow two function
# signatures to match if one is '()'.  This is actually only allowed
# under certain circumstances in K&R C and I allow it too often, which
# is why this test isn't failing.
# 	$(QUAL) -o-lang KandR_C -fq-no-inference Test/overload_global.cc; test $$? -eq 4
	$(QUAL) -o-lang ANSI_C99 -fq-no-inference Test/overload_global.cc; test $$? -eq 4
	$(QUAL) -o-lang GNU_Cplusplus -fq-no-inference Test/overload_global.cc
# test that compound initializes aren't breaking qual with instance-sensitivity on
	$(QUALCFG) -o-lang ANSI_C99 Test/cpd_init1.c

qual-check: qual-check-casts-preserve
qual-check-casts-preserve:
# check -fq-casts-preserve off; this doesn't work any more as
# casts-preserve is now the default *in the lattice*
#	$(QUALCFG) Test/casts_preserve1.c
# check -fq-casts-preserve on
	$(QUALCFG) -fq-casts-preserve Test/casts_preserve1.c; test $$? -eq 32
# check -fq-casts-preserve still doesn't preserve them across casts containing user qualifiers
	$(QUALCFG) -o-lang GNU_Cplusplus -fq-casts-preserve Test/casts_preserve2.c
	$(QUALCFG) -o-lang GNU2_KandR_C -fq-casts-preserve Test/casts_preserve2.c
# verify that for $_1_2 qualifiers as well, per Jeff's email.
	$(QUALCFG) -o-lang GNU_Cplusplus -fq-casts-preserve Test/casts_preserve3.c
	$(QUALCFG) -o-lang GNU2_KandR_C -fq-casts-preserve Test/casts_preserve3.c
# check that casts are not preserved below functions when we turn it off
	$(QUALCFG) -fq-casts-preserve -fq-no-casts-preserve-below-functions Test/casts_preserve_func1.c
	$(QUALCFG) -fq-casts-preserve -fq-no-casts-preserve-below-functions Test/casts_preserve_func1b.c
# check that with -fq-casts-preserve-below-functions they are
	$(QUALCFG) -fq-casts-preserve -fq-casts-preserve-below-functions \
	  Test/casts_preserve_func1.c; test $$? -eq 32
	$(QUALCFG) -fq-casts-preserve -fq-casts-preserve-below-functions \
	  Test/casts_preserve_func1b.c; test $$? -eq 32

.PHONY: qual-check-multi
qual-check-multi:
	@echo "$@ ****************"
# check that multiple input files works
	$(QUALCFG) Test/multi1_a.c Test/multi1_b.c; test $$? -eq 32
# check that multiple input files works through -o-program-files
	$(QUALCFG) -o-program-files Test/multi1.in; test $$? -eq 32
# I don't care to catch this bug any more
# check that global static functions that are declared but not defined are caught
#	$(QUALCFG) Test/static1.c; test $$? -eq 1
# same: I don't care to catch this bug any more
# check that global functions that are declared extern but defined static are caught
#	$(QUALCFG) Test/static2.c; test $$? -eq 1
# check that multiple input files works correctly in the presence of static
	$(QUALCFG) Test/multi2_a.c Test/multi2_b.c
	$(QUALCFG) Test/multi3_a.c Test/multi3_b.c
	$(QUALCFG) Test/multi4_a.c Test/multi4_b.c; test $$? -eq 32
	$(QUALCFG) Test/multi5_a.c Test/multi5_b.c; test $$? -eq 32
# check that C linkage works, say if two functions have same names but different types
	$(QUALCFG) -o-lang KandR_C Test/c_linkage1.c Test/c_linkage2.c; test $$? -eq 32

# check that virtual method calls work across translation units
.PHONY: qual-check-multi-virtual
qual-check-multi-virtual:
	@echo "$@ ****************"
	$(QUALCFG) Test/method_call10a.cc Test/method_call10b.cc; test $$? -eq 32
	$(QUALCFG) Test/method_call10b.cc Test/method_call10a.cc; test $$? -eq 32
	$(QUALCFG) Test/method_call11a.cc Test/method_call11b.cc; test $$? -eq 32
	$(QUALCFG) Test/method_call11b.cc Test/method_call11a.cc; test $$? -eq 32
	$(QUALCFG) Test/method_call12a.cc Test/method_call12b.cc; test $$? -eq 32
	$(QUALCFG) Test/method_call12b.cc Test/method_call12a.cc; test $$? -eq 32
#
	$(QUALCFG) Test/method_call10a.cc -o-srz Test/method_call10a.qdir
	$(QUALCFG) Test/method_call10b.cc -o-srz Test/method_call10b.qdir
#
	$(QUALCFG) Test/method_call10b.qdir Test/method_call10a.qdir; test $$? -eq 32
	$(QUALCFG) Test/method_call10a.qdir Test/method_call10b.qdir; test $$? -eq 32

PRELUDE_FILTER_TEST :=
PRELUDE_FILTER_TEST += prelude_A1.c
PRELUDE_FILTER_TEST += prelude_A2.c
PRELUDE_FILTER_TEST += prelude_A3.c
TOCLEAN += $(addprefix Test/tmp_good_,$(PRELUDE_FILTER_TEST))
TOCLEAN += $(addprefix Test/tmp_bad_,$(PRELUDE_FILTER_TEST))
qual-check-prelude-filter/%:
	./test_filter -good < Test/$* > Test/tmp_good_$*
	$(QUALCFG) Test/tmp_good_$*
	./test_filter -bad  < Test/$* > Test/tmp_bad_$*
	$(QUALCFG) Test/tmp_bad_$* ; test $$? -eq 32
qual-check-prelude-filter: $(addprefix qual-check-prelude-filter/,$(PRELUDE_FILTER_TEST))

# TODO: move to oink_test.incl.mk
.PHONY: qual-check-prelude
qual-check-prelude: qual-check-prelude-filter
	@echo "$@ ****************"
	$(QUALCFG) Test/prelude_A0.c; test $$? -eq 32
	$(QUALCFG) Test/prelude_A0a.c -o-srz Test/prelude_A0a.qdir
	$(QUALCFG) Test/prelude_A0b.c -o-srz Test/prelude_A0b.qdir
	$(QUALCFG) Test/prelude_A0a.qdir Test/prelude_A0b.qdir; test $$? -eq 32
	$(QUALCFG) Test/prelude_A01.c
	$(QUALCFG) Test/prelude_A02.c; test $$? -eq 32
# FIX: perhaps I should also test Test/prelude_A4b.c as a filter test in the good version too
	$(QUALCFG) Test/prelude_A4a.c -o-srz Test/prelude_A4a.qdir
	$(QUALCFG) -fq-poly Test/prelude_A4a.c -o-srz Test/prelude_A4a.poly.qdir
	$(QUALCFG) Test/prelude_A4b.c -o-srz Test/prelude_A4b.qdir; test $$? -eq 32
	$(QUALCFG) Test/prelude_A4b.c -o-control Test/prelude_A4b.ctl -o-srz Test/prelude_A4b_nof.qdir
	$(QUALCFG) -fq-poly Test/prelude_A4b.c -o-control Test/prelude_A4b.ctl -o-srz Test/prelude_A4b_nof.poly.qdir
	$(QUALCFG) Test/prelude_A4b.qdir; test $$? -eq 32
	$(QUALCFG) -o-control Test/prelude_A4b.ctl Test/prelude_A4b_nof.qdir
	$(QUALCFG) -o-control Test/prelude_A4b.ctl Test/prelude_A4a.qdir Test/prelude_A4b_nof.qdir; test $$? -eq 32
	$(QUALCFG) -fq-poly -o-control Test/prelude_A4b.ctl Test/prelude_A4a.poly.qdir Test/prelude_A4b_nof.poly.qdir

.PHONY: qual-check-preludeB
qual-check-preludeB:
	rm -rf Test/prelude_A4a.qdir Test/prelude_A4b_nof.qdir
	@echo
	$(QUALCFG) -tr xmlPrintAST-indent -fq-no-poly Test/prelude_A4a.c -o-srz Test/prelude_A4a.qdir
	@echo
	$(QUALCFG) -tr xmlPrintAST-indent -fq-no-poly Test/prelude_A4b.c -o-control Test/prelude_A4b.ctl -o-srz Test/prelude_A4b_nof.qdir
	@echo
# 	$(QUALCFG) -o-control Test/prelude_A4b.ctl Test/prelude_A4a.qdir Test/prelude_A4b_nof.qdir; test $$? -eq 32
	$(QUALCFG) -tr xmlPrintAST-indent -fq-no-poly -o-control Test/prelude_A4b.ctl Test/prelude_A4a.qdir Test/prelude_A4b_nof.qdir

# FIX: turn these back on using print-trans-qual
# UPDATE: something about the output format has changed so that the
# .cor files don't look right anymore.
# my own regression tests using -fo-pretty-print -fq-print-trans-qual
# REGR_MY :=
# REGR_MY += compoundinit1.c
# REGR_MY += compoundinit2.c
# REGR_MY += compoundinit3.c
# REGR_MY += compoundinit4.c
# REGR_MY += compoundinit5.c
# REGR_MY += compoundinit6.c
# REGR_MY += kernel1.c
# .PHONY: qual-check-regression-my
# qual-check-regression-my: $(addprefix qual-check-regression-my/Test/,$(REGR_MY))
# TEST_TOCLEAN += Test/*.out
# qual-check-regression-my/%:
# 	$(QUALCFG) -fo-pretty-print -fq-print-trans-qual -fo-no-print-startstop $* > $*.out
# 	diff -u $*.out $*.cor

# **************** demo

# demo the canonical taint1 cqual example
.PHONY: qual-check-demo-taint1
qual-check-demo-taint1:
	./qual -q-config $(CFG) Test/taint1.c

# demo the horrible C++ example
.PHONY: qual-check-demo-horrible1
qual-check-demo-horrible1:
	./qual -q-config $(CFG) Test/horrible1.cc

# demo laundering dataflow through the program counter
.PHONY: qual-check-demo-launder
qual-check-demo-launder:
	@echo; echo Demonstrate that 'launder' computes 'cat'
	gcc -E Test/launder.c > Test/launder.i
	gcc Test/launder.i -o Test/launder
	echo 'Hello there!' > Test/hello.out
	Test/launder < Test/hello.out
#
	@echo; echo Demonstrate that Cqual++ does not find the dataflow
	gcc -DCQUAL -E Test/launder.c > Test/launder_qual.i
	grep '$tainted' Test/launder_qual.i
	./qual -q-config $(CFG) Test/launder_qual.i

# demo laundering dataflow through an array that computes the identity
# function
.PHONY: qual-check-demo-launder-array
qual-check-demo-launder-array:
	@echo Demonstrate that Cqual++ does not find the dataflow
	@echo when laundering the data through an array
	./qual -q-config ../libqual/config/lattice Test/launder_array.c

# demo funky qualifiers
.PHONY: qual-check-demo-funky
qual-check-demo-funky:
	./qual -q-config $(CFG) Test/funky_strcat1.c

# demo instance sensitivity
.PHONY: qual-check-demo-instance-sens
qual-check-demo-instance-sens:
# pass
	./qual -q-config $(CFG) -fo-instance-sensitive Test/instance_sensitive_demo.c
# fail
	./qual -q-config $(CFG) Test/instance_sensitive_demo.c

# demo void polymorphism; yes, you use the same flag:
# -fo-instance-sensitive
.PHONY: qual-check-demo-void-poly
qual-check-demo-void-poly:
# pass
	./qual -q-config $(CFG) -fo-instance-sensitive Test/void_poly_demo.c
# fail
	./qual -q-config $(CFG) Test/void_poly_demo.c

# Check that we don't allow inconsistent flags
qual-check: qual-check-flag-consistency1 qual-check-flag-consistency2
qual-check-flag-consistency1:
	$(QUALCFG) Test/empty.i          -o-srz Test/empty.1.qdir
	$(QUALCFG) Test/empty.i -fq-poly -o-srz Test/empty.2.qdir
	$(QUALCFG) Test/empty.i Test/empty.1.qdir Test/empty.2.qdir; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS)

qual-check-flag-consistency2:
	$(QUALCFG) Test/empty.i                        -o-srz Test/empty.1.qdir
	$(QUALCFG) Test/empty.i -fo-instance-sensitive -o-srz Test/empty.2.qdir
	$(QUALCFG) Test/empty.i Test/empty.1.qdir Test/empty.2.qdir; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS)

# This crashes due to inconsistent flags; don't expect it to work.
.PHONY: qual-check-prelude-crash
qual-check-prelude-crash:
	$(QUALCFG) -fq-no-poly Test/prelude_A4a.c -o-srz Test/prelude_A4a.qdir
	$(QUALCFG) -fq-no-poly Test/prelude_A4b.c -o-control Test/prelude_A4b.ctl -o-srz Test/prelude_A4b_nof.qdir
# Segmentation fault down in finish_quals()
	$(QUALCFG) -fq-poly -o-control Test/prelude_A4b.ctl Test/prelude_A4a.qdir Test/prelude_A4b_nof.qdir

# ****

# dsw: Rob, these should both issue a warning that $tainted flows to
# $untainted.
#
# When we do everything in-core it does the right thing.
#
# BAD: When serialize and do *not* compactify the graph, it concludes
# that $tainted flows into untainted, but then it has an assertion
# failure.
#
# BAD: When serialize and *do* compactify the graph, it *fails* to
# conclude that $tainted flows into untainted.  The resulting graph
# also seems to not contain $tainted or $untainted anywhere at all.

# IN-CORE version; this works
.PHONY: qual-check-prelude-A0
qual-check-prelude-A0:
	rm -f quals.dot A0-tainted.cc.ps A0-untainted.cc.ps
	./qual -fq-poly -fq-print-quals-graph -fq-no-ret-inf -q-config $(CFG) Test/prelude_A0a.c Test/prelude_A0b.c
	dot -Tps -o A0-cc.ps quals.dot
	./qual_graph_component -root '$$tainted'   < quals.dot > A0-tainted.dot
	dot -Tps -o A0-tainted.cc.ps   A0-tainted.dot
	./qual_graph_component -root '$$untainted' < quals.dot > A0-untainted.dot
	dot -Tps -o A0-untainted.cc.ps A0-untainted.dot
	$(ANNOUNCE_TEST_PASS)

# SERIALIZATION version; fails in different ways depending on how you
# set this flag

# use this to turn compaction on and off; NOTE the flag being ON means
# compaction is OFF
FLGS0 :=
#FLGS0 += -fq-no-compactify-graph

.PHONY: qual-check-prelude-A0srz
qual-check-prelude-A0srz:
	rm -f quals.dot A0srz-tainted.cc.ps A0srz-untainted.cc.ps
	rm -rf Test/prelude_A0a.qdir Test/prelude_A0b.qdir
	./qual -fq-poly -q-config $(CFG) Test/prelude_A0a.c $(FLGS0) -o-srz Test/prelude_A0a.qdir
	./qual -fq-poly -q-config $(CFG) Test/prelude_A0b.c $(FLGS0) -o-srz Test/prelude_A0b.qdir
	./qual -fq-poly -q-config $(CFG) -fq-print-quals-graph -fq-no-ret-inf Test/prelude_A0a.qdir Test/prelude_A0b.qdir
	dot -Tps -o A0srz-cc.ps quals.dot
	./qual_graph_component -root '$$tainted'   < quals.dot > A0srz-tainted.dot
	dot -Tps -o A0srz-tainted.cc.ps   A0srz-tainted.dot
	./qual_graph_component -root '$$untainted' < quals.dot > A0srz-untainted.dot
	dot -Tps -o A0srz-untainted.cc.ps A0srz-untainted.dot
	$(ANNOUNCE_TEST_PASS)


# dsw: this is now passing but essentially for the wrong reason (I
# just avoid the NULL pointer deref that comes about because the type
# system points back into the AST but the AST is gone once we have
# deserialized); I am still serializing and annotating lots of
# template variables
qual-check: qual-check-template-dep-resrz1
qual-check-template-dep-resrz1:
	./qual -q-config $(CFG) Test/template_dep_resrz1.cc -o-srz Test/template_dep_resrz1.1.qdir
	./qual -q-config $(CFG)                                    Test/template_dep_resrz1.1.qdir -o-srz Test/template_dep_resrz1.2.qdir
	$(ANNOUNCE_TEST_PASS)

# 18cca471-ba67-40dd-865a-c787b1c65ade
qual-check: qual-check-ptr-to-member-cast1
qual-check-ptr-to-member-cast1:
	$(QUALCFG) Test/ptr-to-member-cast1.cc
	$(ANNOUNCE_TEST_PASS)

# f66eefd9-fad0-4b47-b57f-558b5f877f67
XFAIL += qual-check-funcptr-cast-ret-compound1
qual-check-funcptr-cast-ret-compound1:
	$(QUALCFG) Test/funcptr_cast_ret_compound1.c
	$(ANNOUNCE_TEST_PASS)

# 61010461-dc93-4312-bca7-219392176c22
qual-check: qual-check-funcptr-implicit-addr-resrz1
qual-check-funcptr-implicit-addr-resrz1: qual-check-resrz/Test/funcptr_implicit_addr1.c

qual-check: qual-check-ellipsis-noparam1
qual-check-ellipsis-noparam1:
	$(QUALCFG) Test/ellipsis_noparam1.c
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-noparam-cast1
qual-check-noparam-cast1:
	$(QUALCFG) Test/noparam_cast1.c
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-max-errors1
qual-check-max-errors1:
	($(QUALCFG) Test/max_errors1.ii -q-max-errors 5 2>&1) | grep 'subsequent warnings omitted'
	$(ANNOUNCE_TEST_PASS)

# check the performance difference that
# merge_E_variable_and_var_values makes
QUALCFG1 := $(QUALCFG) -fq-no-ret-inf
FILE1 := Test/nonport.ii

TEST_TOCLEAN += $(FILE1)
.PHONY: qual-perf-merge_values
qual-perf-merge_values:
	cp $(ELSA_DIR)/in/big/nonport.i $(FILE1)
#
	@echo; echo "**** $@ no optimization"
	rm -rf $(FILE1).1.qdir
	time $(QUALCFG1) -fo-no-merge-E_variable-and-var-values $(FILE1) -o-srz $(FILE1).1.qdir &> /dev/null
	ls -lh $(FILE1).1.qdir
#
	@echo; echo "**** $@ with optimization"
	rm -rf $(FILE1).2.qdir
	time $(QUALCFG1) -fo-merge-E_variable-and-var-values $(FILE1) -o-srz $(FILE1).2.qdir &> /dev/null
	ls -lh $(FILE1).2.qdir

qual-check: qual-check-usedindataflow1-q
qual-check-usedindataflow1-q:
	./qual Test/usedindataflow1.cc -fq-no-inference -o-srz Test/usedindataflow1.qdir
	if grep foo Test/usedindataflow1.qdir/value.xml; then false; else true; fi
	if grep __builtin_cos Test/usedindataflow1.qdir/value.xml; then false; else true; fi
	rm -rf Test/usedindataflow1.qdir
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-func-param-ret-qual
qual-check-func-param-ret-qual:
# check -q-func-param-ret-qual; control experiment
	$(QUALCFG) Test/func_param_ret1.c
# check -q-func-param-ret-qual; test; NOTE: Doubling the '$' because 'make' eats one.
	$(QUALCFG) -q-func-param-ret-qual '$$untainted' Test/func_param_ret1.c; test $$? -eq 32
# 	$(QUALCFG) -fq-names -fq-name-vars -fq-name-expressions -fq-name-if-missing -fq-name-with-loc -q-func-param-ret-qual '$$untainted' Test/func_param_ret1.c -fq-print-quals-graph
# 	./qual_graph_component -root '$$tainted'   < quals.dot > func_param_ret-tainted.dot
# 	dot -Tps -o func_param_ret-tainted.cc.ps   func_param_ret-tainted.dot
# 	./qual_graph_component -root '$$untainted' < quals.dot > func_param_ret-untainted.dot
# 	dot -Tps -o func_param_ret-untainted.cc.ps func_param_ret-untainted.dot
#
# check -q-func-param-ret-qual; control experiment
	$(QUALCFG) Test/func_param_ret2.c
# check -q-func-param-ret-qual; test; NOTE: Doubling the '$' because 'make' eats one.
	$(QUALCFG) -q-func-param-ret-qual '$$untainted' Test/func_param_ret2.c; test $$? -eq 32
