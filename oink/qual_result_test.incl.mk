# see License.txt for copyright and terms of use

# Included makefile for testing the qualifier analysis provided by
# qual.

ifndef QUAL_TEST_MAKEFILE
$(error This makefile should be included in qual_test.incl.mk, not used stand-alone)
endif

# handy strings:
# -tr xmlPrintAST-indent

# check simple good and bad tests ****

GOOD_TEST :=
BAD_TEST :=

# FIX: we don't deal with va_arg
# BAD_TEST += varargs2.c
# not a legal C++ file; doesn't typecheck
# GOOD_TEST += mess1.c

# test that void works normally when not in instance-sensitive mode
BAD_TEST += void_nonfs1.cc
BAD_TEST += void_nonfs2.cc
BAD_TEST += void_nonfs3.cc

BAD_TEST += non-compound_IN_ctor1.cc
BAD_TEST += non-compound_IN_ctor2.cc
BAD_TEST += non-compound_IN_ctor3.cc
BAD_TEST += non-compound_IN_ctor4.cc

BAD_TEST += method_call1.cc
BAD_TEST += method_call2.cc
BAD_TEST += method_call3.cc

BAD_TEST += tb2000fail.c
BAD_TEST += tb2010array_assign.c
BAD_TEST += tb2020cast.c
BAD_TEST += tb2021cast.c
BAD_TEST += tb2030pointer_assign.c
BAD_TEST += tb2040taint.c
BAD_TEST += const1.c
BAD_TEST += const2.c
BAD_TEST += typedef1.c

GOOD_TEST += abstract-func.cc
GOOD_TEST += method_call3g.cc

FILTER_TEST += array_pointer1.c
GOOD_TEST += simple5.c

# This should not compile
#GOOD_TEST += deep1.c
# This should not pass.
#GOOD_TEST += ellipsis1.c
GOOD_TEST += empty.c
GOOD_TEST += func1.c
GOOD_TEST += func2.c
GOOD_TEST += func3.c
GOOD_TEST += funcall2.c
GOOD_TEST += funky1.c
GOOD_TEST += funky2.c
GOOD_TEST += funky3.c
GOOD_TEST += funky4.c
GOOD_TEST += globaldef1.c
GOOD_TEST += linkage1.c
GOOD_TEST += null.c
GOOD_TEST += ref1.c
GOOD_TEST += simple1.c
GOOD_TEST += simple2.c
GOOD_TEST += simple4.c
GOOD_TEST += simple_str.c
GOOD_TEST += ssh1.c
GOOD_TEST += struct_sizeof.c
GOOD_TEST += taint0.c
GOOD_TEST += taintB.c
GOOD_TEST += varargs1.c
GOOD_TEST += enum_subscript.i
GOOD_TEST += new1.cc
# FIX: this should be a FILTER_TEST, but I can't figure out how to get
# the $tainted onto the retType for the constructor
GOOD_TEST += method_call13.cc
GOOD_TEST += cpd_lit_arg.cc
GOOD_TEST += inc_ref.cc
GOOD_TEST += def_arg_cast.ii
GOOD_TEST += grouping_method.cc
GOOD_TEST += meth_param_by_value.cc

GOOD_TEST += tertiary_cast_func_ptr.c

GOOD_TEST += empty_statement_expr1.c

# tests for C mode, not C++ mode
GOOD_TEST_C :=
GOOD_TEST_C += void_star_func.i
GOOD_TEST_C += this_var.i
GOOD_TEST_C += void_star_to_double_ptr.i
GOOD_TEST_C += cast_assign.i
GOOD_TEST_C += stmt_expr_no_edge.i
GOOD_TEST_C += varargsE.i
GOOD_TEST_C += char_array_lit_init.i
GOOD_TEST_C += char_array_lit_init2.i
GOOD_TEST_C += void_star_to_func.i

BAD_TEST_C :=
# hmm, there is nothing bad about this test
# BAD_TEST_C += char_pointer_lit_init.i

.PHONY: qual-check-good/% qual-check-good-c/% qual-check-bad/%
qual-check-good/%:
	@echo; echo QUAL GOOD $@
	$(QUALCFG) $*
	$(ANNOUNCE_TEST_PASS)
qual-check-good-c/%:
	@echo; echo QUAL GOOD C $@
	$(QUALCFG) -o-lang GNU2_KandR_C $*
	$(ANNOUNCE_TEST_PASS)
qual-check-bad/%:
	@echo; echo QUAL BAD $@
	$(QUALCFG) $*; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)
qual-check-bad-c/%:
	@echo; echo QUAL BAD C $@
	$(QUALCFG) -o-lang GNU2_KandR_C $*; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)
.PHONY: qual-check-good qual-check-good-c qual-check-bad qual-check-bad-c
qual-check-good: $(addprefix qual-check-good/Test/,$(GOOD_TEST))
qual-check-good-c: $(addprefix qual-check-good-c/Test/,$(GOOD_TEST_C))
qual-check-bad: $(addprefix qual-check-bad/Test/,$(BAD_TEST))
qual-check-bad-c: $(addprefix qual-check-bad-c/Test/,$(BAD_TEST_C))

# check overload flag is making the difference for these tests ****

# NOTE: 24 sept 2004: the semantics of overloading checking in elsa is
# now stronger; before if it were off, you would just get the first
# function, whereas now you get an error: it still notices the
# overloading and does not degrade to just using the first function as
# it used to.  Therefore, you just can't do these tests anymore.

OLOAD_TEST :=

# OLOAD_TEST += overload_global.cc
# OLOAD_TEST += overload_static_member.cc
# OLOAD_TEST += overload_method.cc
# OLOAD_TEST += overload_IN_ctor.cc
# OLOAD_TEST += overload_E_constructor.cc
# OLOAD_TEST += overload_E_new.cc
# # Test the first overloading call in void Function::tcheck_memberInits(Env &env)
# OLOAD_TEST += overload_MemberInit1.cc
# # Test the second overloading call in void Function::tcheck_memberInits(Env &env)
# OLOAD_TEST += overload_MemberInit2.cc

GOOD_OLOAD_TEST := $(OLOAD_TEST)
BAD_OLOAD_TEST  := $(OLOAD_TEST)

MISC_OLOAD_TEST :=
MISC_OLOAD_TEST += overload_implicit_copy_ctor1.cc
MISC_OLOAD_TEST += overload_implicit_copy_assign1.cc
# Seems this should be failing since the implicit dtor doesn't have a
# 'this' param, but it isn't.
MISC_OLOAD_TEST += overload_implicit_dtor1.cc

.PHONY: qual-check-overload-good/% qual-check-overload-bad/% qual-check-overload-misc/%
qual-check-overload-good/%:
	@echo; echo QUAL OVERLOAD GOOD $@
# The test against 1 is for the fact that the number of arguments will
# fail to match in the absence of overloading
	$(QUALCFG) -fo-no-do-overload $*; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS)
qual-check-overload-bad/%:
	@echo; echo QUAL OVERLOAD BAD $@
	$(QUALCFG) -fo-do-overload $*; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)
qual-check-overload-misc/%:
	$(QUALCFG) -fo-do-overload $*
	$(ANNOUNCE_TEST_PASS)
.PHONY: qual-check-overload-good qual-check-overload-bad qual-check-overload-misc
qual-check-overload-good: $(addprefix qual-check-overload-good/Test/,$(GOOD_OLOAD_TEST))
qual-check-overload-bad: $(addprefix qual-check-overload-bad/Test/,$(BAD_OLOAD_TEST))
qual-check-overload-misc: $(addprefix qual-check-overload-misc/Test/,$(MISC_OLOAD_TEST))
.PHONY: qual-check-overload
qual-check-overload: qual-check-overload-good qual-check-overload-bad qual-check-overload-misc

# VALUE-FIX:
# OP_OLOAD_TEST += overload_oper1.cc
OP_OLOAD_TEST += overload_oper2.cc

GOOD_OP_OLOAD_TEST := $(OP_OLOAD_TEST)
BAD_OP_OLOAD_TEST  := $(OP_OLOAD_TEST)

.PHONY: qual-check-op-overload-good/% qual-check-op-overload-bad/%
qual-check-op-overload-good/%:
	@echo; echo QUAL OP OVERLOAD GOOD $@
	$(QUALCFG) -fo-no-do-op-overload $*
qual-check-op-overload-bad/%:
	@echo; echo QUAL OP OVERLOAD BAD $@
	$(QUALCFG) -fo-do-op-overload $*; test $$? -eq 32
.PHONY: qual-check-op-overload-good qual-check-op-overload-bad
qual-check-op-overload-good: $(addprefix qual-check-op-overload-good/Test/,$(GOOD_OP_OLOAD_TEST))
qual-check-op-overload-bad: $(addprefix qual-check-op-overload-bad/Test/,$(BAD_OP_OLOAD_TEST))
.PHONY: qual-check-op-overload
qual-check-op-overload: qual-check-op-overload-good qual-check-op-overload-bad

qual-check-main-extern:
	$(QUALCFG) -o-lang SUFFIX Test/main_externc1a.cc Test/main_externc1b.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# test -fq-casts-preserve-below-functions
qual-check-casts-preserve-func:
	$(QUALCFG) -fq-casts-preserve-below-functions Test/casts_preserve_func2.c; test $$? -eq 32
	$(QUALCFG) -fq-casts-preserve-below-functions Test/casts_preserve_func2.c -o-srz Test/casts_preserve_func2.qdir; test $$? -eq 32
	$(QUALCFG) -fq-casts-preserve-below-functions Test/casts_preserve_func2.qdir; test $$? -eq 32
	$(QUALCFG) -fq-casts-preserve-below-functions Test/casts_preserve_func3.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# test void autounion works for function values
qual-check-void-func-autounion:
	$(QUALCFG) -fq-casts-preserve-below-functions -fo-instance-sensitive Test/function_through_void1.c; test $$? -eq 32
	$(QUALCFG) -fq-casts-preserve-below-functions -fo-instance-sensitive Test/function_through_void2.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)


# **** extern inline tests

# NOTE: these tests happen to also be the only tests that verify that
# linking is idempotent

# test an extern inline declaration and a normal definition in another
# file; tests the linking behavior
qual-check-extern-inline: qual-check-extern-inline-srz2
qual-check-extern-inline-srz2:
	$(QUALCFG) Test/extern_inline_func2a.c Test/extern_inline_func2b.c ; test $$? -eq 32
	$(QUALCFG) Test/extern_inline_func2a.c -o-srz Test/extern_inline_func2a.qdir
	$(QUALCFG) Test/extern_inline_func2b.c -o-srz Test/extern_inline_func2b.qdir
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func2a.qdir Test/extern_inline_func2b.qdir -o-srz Test/extern_inline_func2linked.qdir  ; test $$? -eq 32
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func2linked.qdir ; test $$? -eq 32
# reverse order
	$(QUALCFG) Test/extern_inline_func2a.c -o-srz Test/extern_inline_func2a.qdir
	$(QUALCFG) Test/extern_inline_func2b.c -o-srz Test/extern_inline_func2b.qdir
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func2b.qdir Test/extern_inline_func2a.qdir -o-srz Test/extern_inline_func2linked.qdir  ; test $$? -eq 32
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func2linked.qdir ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# test an extern inline definition and a normal definition in another
# file; tests the compiling behavior
qual-check-extern-inline: qual-check-extern-inline-srz3
qual-check-extern-inline-srz3:
# 	optimizations OFF: should flow tainted to untainted
	$(QUALCFG) -tr handleExternInline-asPrototype Test/extern_inline_func3a.c Test/extern_inline_func3b.c ; test $$? -eq 32
	$(QUALCFG) -tr handleExternInline-asPrototype Test/extern_inline_func3a.c -o-srz Test/extern_inline_func3a.qdir
	$(QUALCFG) -tr handleExternInline-asPrototype Test/extern_inline_func3b.c -o-srz Test/extern_inline_func3b.qdir
	$(QUALCFG) -tr handleExternInline-asPrototype -fo-report-link-errors Test/extern_inline_func3a.qdir Test/extern_inline_func3b.qdir -o-srz Test/extern_inline_func3linked.qdir  ; test $$? -eq 32
	$(QUALCFG) -tr handleExternInline-asPrototype -fo-report-link-errors Test/extern_inline_func3linked.qdir ; test $$? -eq 32
# 	optimizations ON: should NOT flow tainted to untainted
	$(QUALCFG) Test/extern_inline_func3a.c Test/extern_inline_func3b.c
	$(QUALCFG) Test/extern_inline_func3a.c -o-srz Test/extern_inline_func3a-opt.qdir
	$(QUALCFG) Test/extern_inline_func3b.c -o-srz Test/extern_inline_func3b-opt.qdir
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func3a-opt.qdir Test/extern_inline_func3b-opt.qdir -o-srz Test/extern_inline_func3linked-opt.qdir
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func3linked-opt.qdir
	$(ANNOUNCE_TEST_PASS)

# test an extern inline definition and a normal definition in another
# file; tests the compiling behavior
qual-check-extern-inline: qual-check-extern-inline-srz4
qual-check-extern-inline-srz4:
# 	optimizations OFF: should flow tainted to untainted
	$(QUALCFG) -tr handleExternInline-asPrototype Test/extern_inline_func4a.c -o-srz Test/extern_inline_func4a.qdir
	$(QUALCFG) -tr handleExternInline-asPrototype Test/extern_inline_func4b.c -o-srz Test/extern_inline_func4b.qdir
	$(QUALCFG) -tr handleExternInline-asPrototype -fo-report-link-errors Test/extern_inline_func4a.qdir Test/extern_inline_func4b.qdir -o-srz Test/extern_inline_func4linked.qdir  ; test $$? -eq 32
	$(QUALCFG) -tr handleExternInline-asPrototype -fo-report-link-errors Test/extern_inline_func4linked.qdir ; test $$? -eq 32
# 	optimizations ON: also should flow tainted to untainted
	$(QUALCFG) Test/extern_inline_func4a.c -o-srz Test/extern_inline_func4a-opt.qdir
	$(QUALCFG) Test/extern_inline_func4b.c -o-srz Test/extern_inline_func4b-opt.qdir
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func4a-opt.qdir Test/extern_inline_func4b-opt.qdir -o-srz Test/extern_inline_func4linked-opt.qdir  ; test $$? -eq 32
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func4linked-opt.qdir  ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-extern-inline: qual-check-extern-inline-srz5
qual-check-extern-inline-srz5:
# 	optimizations OFF: should flow tainted to untainted
	$(QUALCFG) -tr handleExternInline-asPrototype Test/extern_inline_func5a.c -o-srz Test/extern_inline_func5a.qdir  ; test $$? -eq 32
	$(QUALCFG) -tr handleExternInline-asPrototype Test/extern_inline_func5b.c -o-srz Test/extern_inline_func5b.qdir
	$(QUALCFG) -tr handleExternInline-asPrototype -fo-report-link-errors Test/extern_inline_func5a.qdir Test/extern_inline_func5b.qdir -o-srz Test/extern_inline_func5linked.qdir  ; test $$? -eq 32
	$(QUALCFG) -tr handleExternInline-asPrototype -fo-report-link-errors Test/extern_inline_func5linked.qdir ; test $$? -eq 32
# 	optimizations ON: also should flow tainted to untainted
	$(QUALCFG) Test/extern_inline_func5a.c -o-srz Test/extern_inline_func5a-opt.qdir  ; test $$? -eq 32
	$(QUALCFG) Test/extern_inline_func5b.c -o-srz Test/extern_inline_func5b-opt.qdir
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func5a-opt.qdir Test/extern_inline_func5b-opt.qdir -o-srz Test/extern_inline_func5linked-opt.qdir  ; test $$? -eq 32
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func5linked-opt.qdir  ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-extern-inline: qual-check-extern-inline5
qual-check-extern-inline5:
# 	optimizations OFF: should flow tainted to untainted
	$(QUALCFG) -tr handleExternInline-asPrototype -fo-report-link-errors Test/extern_inline_func5a.c Test/extern_inline_func5b.c  ; test $$? -eq 32
# 	optimizations ON: also should flow tainted to untainted
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func5a.c Test/extern_inline_func5b.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-extern-inline: qual-check-extern-inline6
qual-check-extern-inline6:
# 	optimizations OFF: should flow tainted to untainted
	$(QUALCFG) -tr handleExternInline-asPrototype -fo-report-link-errors Test/extern_inline_func6a.c Test/extern_inline_func6b.c ; test $$? -eq 32
# 	optimizations ON: also should flow tainted to untainted
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func6a.c  ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-extern-inline: qual-check-extern-inline7
qual-check-extern-inline7:
# 	optimizations OFF: should flow tainted to untainted
	$(QUALCFG) -tr handleExternInline-asPrototype -fo-report-link-errors Test/extern_inline_func7a.c ; test $$? -eq 32
# 	optimizations ON: also should flow tainted to untainted
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func7a.c  ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-extern-inline: qual-check-extern-inline8
qual-check-extern-inline8:
	$(QUALCFG) -tr handleExternInline-asPrototype -fo-report-link-errors Test/extern_inline_func8a.cc Test/extern_inline_func8b.cc
	$(QUALCFG) -fo-report-link-errors Test/extern_inline_func8a.cc Test/extern_inline_func8b.cc
	$(ANNOUNCE_TEST_PASS)

# ****

qual-check-unprototyped:
	$(QUALCFG) Test/unprototyped1a.c Test/unprototyped1b.c -fo-report-link-errors ; test $$? -eq 32
	$(QUALCFG) Test/unprototyped2a.c Test/unprototyped1b.c -fo-report-link-errors ; test $$? -eq 32
	$(QUALCFG) Test/unprototyped3a.c Test/unprototyped1b.c -fo-report-link-errors ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-unprototyped5:
	$(QUALCFG) Test/unprototyped5a.c Test/unprototyped5b.c -fo-report-link-errors -fo-instance-sensitive ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-unprototyped-vararg:
	$(QUALCFG) Test/unprototyped4a.c Test/unprototyped4b.c -fo-report-link-errors ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-unprototyped6:
	$(QUALCFG) Test/unprototyped6.c
	$(ANNOUNCE_TEST_PASS)

# ****

# check that int-as-void* works across serialization
qual-check-int-as-voidstar-srz:
	$(QUALCFG) Test/int_as_void_star_poly5a.c -o-srz Test/int_as_void_star_poly5a.qdir
	$(QUALCFG) Test/int_as_void_star_poly5b.c -o-srz Test/int_as_void_star_poly5b.qdir
	$(QUALCFG) -fo-report-link-errors Test/int_as_void_star_poly5a.qdir Test/int_as_void_star_poly5b.qdir -o-srz Test/int_as_void_star_poly5.qdir; test $$? -eq 32
	$(QUALCFG) -fo-report-link-errors Test/int_as_void_star_poly5.qdir; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# check that int-as-void* works across serialization works with
# instance sensitivity
qual-check-int-as-voidstar-srz-is:
	$(QUALCFG) -fo-instance-sensitive Test/int_as_void_star_poly5a.c -o-srz Test/int_as_void_star_poly5a-is.qdir
	$(QUALCFG) -fo-instance-sensitive Test/int_as_void_star_poly5b.c -o-srz Test/int_as_void_star_poly5b-is.qdir
	$(QUALCFG) -fo-instance-sensitive -fo-report-link-errors Test/int_as_void_star_poly5a-is.qdir Test/int_as_void_star_poly5b-is.qdir -o-srz Test/int_as_void_star_poly5-is.qdir; test $$? -eq 32
	$(QUALCFG) -fo-instance-sensitive -fo-report-link-errors Test/int_as_void_star_poly5-is.qdir; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

XFAIL += qual-check-ellipsis-as-voidstar
qual-check-ellipsis-as-voidstar:
	$(QUALCFG) -fo-instance-sensitive Test/vararg_input2.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# check that ellipsis-as-void* works across serialization
qual-check-ellipsis-as-voidstar-srz:
	$(QUALCFG) Test/ellipsis_as_void_star_poly5a.c -o-srz Test/ellipsis_as_void_star_poly5a.qdir
	$(QUALCFG) Test/ellipsis_as_void_star_poly5b.c -o-srz Test/ellipsis_as_void_star_poly5b.qdir
	$(QUALCFG) -fo-report-link-errors Test/ellipsis_as_void_star_poly5a.qdir Test/ellipsis_as_void_star_poly5b.qdir -o-srz Test/ellipsis_as_void_star_poly5.qdir; test $$? -eq 32
	$(QUALCFG) -fo-report-link-errors Test/ellipsis_as_void_star_poly5.qdir; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# check that ellipsis-as-void* works across serialization works with
# instance sensitivity
qual-check-ellipsis-as-voidstar-srz-is:
	$(QUALCFG) -fo-instance-sensitive Test/ellipsis_as_void_star_poly5a.c -o-srz Test/ellipsis_as_void_star_poly5a-is.qdir
	$(QUALCFG) -fo-instance-sensitive Test/ellipsis_as_void_star_poly5b.c -o-srz Test/ellipsis_as_void_star_poly5b-is.qdir
	$(QUALCFG) -fo-instance-sensitive -fo-report-link-errors Test/ellipsis_as_void_star_poly5a-is.qdir Test/ellipsis_as_void_star_poly5b-is.qdir -o-srz Test/ellipsis_as_void_star_poly5-is.qdir; test $$? -eq 32
	$(QUALCFG) -fo-instance-sensitive -fo-report-link-errors Test/ellipsis_as_void_star_poly5-is.qdir; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# check that instance-sensitivity works across serialization
qual-check-instsens-srz:
	@echo "1 ****"
	$(QUALCFG) Test/inst_sens_srz_a.c Test/inst_sens_srz_b.c; test $$? -eq 32
#
	@echo "2 ****"
	$(QUALCFG) Test/inst_sens_srz_a.c -o-srz Test/inst_sens_srz_a.qdir
	$(QUALCFG) Test/inst_sens_srz_b.c -o-srz Test/inst_sens_srz_b.qdir
	$(QUALCFG) -fo-report-link-errors Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir -o-srz Test/inst_sens_srz.qdir; test $$? -eq 32
	$(QUALCFG) -fo-report-link-errors Test/inst_sens_srz.qdir; test $$? -eq 32
#
	@echo "3 ****"
	$(QUALCFG) -fo-instance-sensitive Test/inst_sens_srz_a.c Test/inst_sens_srz_b.c; test $$? -eq 32
#
	@echo "4 ****"
	$(QUALCFG) -tr xmlPrintAST-indent -fo-instance-sensitive Test/inst_sens_srz_a.c -o-srz Test/inst_sens_srz_a.qdir
	$(QUALCFG) -tr xmlPrintAST-indent -fo-instance-sensitive Test/inst_sens_srz_b.c -o-srz Test/inst_sens_srz_b.qdir
	$(QUALCFG) -fo-instance-sensitive -fo-report-link-errors Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir; test $$? -eq 32
	$(QUALCFG) -tr xmlPrintAST-indent -fo-instance-sensitive -fo-report-link-errors Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir -o-srz Test/inst_sens_srz2.qdir; test $$? -eq 32
	$(QUALCFG) -fo-instance-sensitive -fo-report-link-errors Test/inst_sens_srz2.qdir; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-resrz-return-func:
	$(QUALCFG) Test/return_func1.c -o-srz Test/return_func1.1.qdir
	$(QUALCFG) Test/return_func1.1.qdir -o-srz Test/return_func1.2.qdir
	$(ANNOUNCE_TEST_PASS)

qual-check-srz-funcptr-c:
	$(QUALCFG) Test/funcptr1.c -o-srz Test/funcptr1.qdir
	$(ANNOUNCE_TEST_PASS)

qual-check-srz-const-cast-func:
	$(QUAL) -q-config Test/lattice1 Test/constcastfunc1.cc -o-srz Test/constcastfunc1.1.qdir
	$(QUAL) -q-config Test/lattice1 Test/constcastfunc1.1.qdir -o-srz Test/constcastfunc1.2.qdir
	$(ANNOUNCE_TEST_PASS)

# check that test passes filtered into both good and bad forms ****

# C versions ****************

# filter test for C mode
C_FILTER_TEST :=

# filter test for C mode using the lattice where tainted flows through
# casts; FIX: this is no longer necessary as flowing through casts is
# now the default for tainted
C_FILTER_TEST_TAINTXCAST :=

# applies with and without instance sensitivity
C_FILTER_TEST += instance_sensitive0.cc
C_FILTER_TEST += instance_sensitive1.cc
C_FILTER_TEST += instance_sensitive2a.cc
C_FILTER_TEST += instance_sensitive2b.cc
C_FILTER_TEST += instance_sensitive2c.cc
C_FILTER_TEST += inst_sens_cpdinit1.c

C_FILTER_TEST += void_star_poly1.cc
C_FILTER_TEST += void_star_poly1call.cc
C_FILTER_TEST += void_star_poly2.cc
C_FILTER_TEST += void_star_poly2call.cc
C_FILTER_TEST += void_star_poly3_typedef.cc

C_FILTER_TEST_TAINTXCAST += int_as_void_star_poly1.cc
C_FILTER_TEST_TAINTXCAST += int_as_void_star_poly1call.cc
C_FILTER_TEST_TAINTXCAST += int_as_void_star_poly2.cc
C_FILTER_TEST_TAINTXCAST += int_as_void_star_poly2call.cc
C_FILTER_TEST_TAINTXCAST += int_as_void_star_poly3.cc
C_FILTER_TEST_TAINTXCAST += int_as_void_star_poly4a.cc
C_FILTER_TEST_TAINTXCAST += int_as_void_star_poly4b.cc
C_FILTER_TEST_TAINTXCAST += int_as_void_star_poly4c.cc
C_FILTER_TEST_TAINTXCAST += int_as_void_star_poly4d.cc

# C++ versions ****************

# Some basic tests: Attempt to test every combination of flow edges.
# Much is left to be done.
FILTER_TEST :=
FILTER_TEST_CC_INST_SENS :=

# applies with and without instance sensitivity
FILTER_TEST += instance_sensitive0.cc
FILTER_TEST_CC_INST_SENS += instance_sensitive0.cc

FILTER_TEST_CC_INST_SENS += instance_sensitive2a.cc
FILTER_TEST_CC_INST_SENS += instance_sensitive2b.cc
FILTER_TEST_CC_INST_SENS += instance_sensitive2c.cc

FILTER_TEST_CC_INST_SENS += void_star_poly1.cc
FILTER_TEST_CC_INST_SENS += void_star_poly1call.cc
FILTER_TEST_CC_INST_SENS += void_star_poly2.cc
FILTER_TEST_CC_INST_SENS += void_star_poly2call.cc

FILTER_TEST_CC_INST_SENS += int_as_void_star_poly1.cc
FILTER_TEST_CC_INST_SENS += int_as_void_star_poly1call.cc
FILTER_TEST_CC_INST_SENS += int_as_void_star_poly2.cc
FILTER_TEST_CC_INST_SENS += int_as_void_star_poly2call.cc
FILTER_TEST_CC_INST_SENS += int_as_void_star_poly3.cc
FILTER_TEST_CC_INST_SENS += int_as_void_star_poly4a.cc
FILTER_TEST_CC_INST_SENS += int_as_void_star_poly4b.cc
FILTER_TEST_CC_INST_SENS += int_as_void_star_poly4c.cc
FILTER_TEST_CC_INST_SENS += int_as_void_star_poly4d.cc

# this is a C version of what happens when a ctor is called.  Rob says
# that polymorphism won't work right; this should fail
# BAD_TEST += poor_mans_ctor.c

FILTER_TEST_CC_INST_SENS += rich_mans_copy_assign.cc
FILTER_TEST_CC_INST_SENS += rich_mans_copy_assign_b.cc
FILTER_TEST_CC_INST_SENS += rich_mans_copy_ctor.cc
FILTER_TEST_CC_INST_SENS += rich_mans_copy_ctor_b.cc
FILTER_TEST_CC_INST_SENS += rich_mans_ctor.cc
FILTER_TEST_CC_INST_SENS += rich_mans_ctor_b.cc
FILTER_TEST_CC_INST_SENS += rich_mans_method.cc
FILTER_TEST_CC_INST_SENS += rich_mans_method_b.cc
FILTER_TEST_CC_INST_SENS += rich_mans_method_c.cc

# ****************

# test that an expression is an l-value
FILTER_TEST += assign3.cc

# test that declarators work
FILTER_TEST += decl1.cc

# test that nested functions work
FILTER_TEST += nested_func1.cc
FILTER_TEST += nested_func2.cc

# forwarding of function members of templatized classes
FILTER_TEST += template_class4_func_fwd1.cc

# mutual recursion of function members of templatized classes
FILTER_TEST += template_class4_func_fwd2.cc

# mutual recursion of function members of specializations of
# templatized classes
FILTER_TEST += template_class4_func_fwd3.cc

# FIX: At least the first of these does not even seem to be legal C++.
#
# forwarding of function members of templatized classes, def after
# inst
# FILTER_TEST += template_class4_func_defafteruse1.cc
# mutual recursion of function members of templatized classes, def
# after inst
# FILTER_TEST += template_class4_func_defafteruse2.cc
# mutual recursion of function members of specializations of
# templatized classes, def after inst
# FILTER_TEST += template_class4_func_defafteruse3.cc


FILTER_TEST += template_func1.cc
FILTER_TEST += template_func2.cc
FILTER_TEST += template_func2_multiple.cc
# sm: there is no such thing as a function template partial specialization
#FILTER_TEST += template_func3_partial.cc
# FIX: function template overloading doesn't work
# FILTER_TEST += template_func4_overload.cc
FILTER_TEST += template_func5_arg_deduce1.cc
FILTER_TEST += template_func5_arg_deduce2.cc
FILTER_TEST += template_func5_arg_deduce3.cc
FILTER_TEST += template_func5_arg_deduce4.cc
# forwarding of function template specializations
FILTER_TEST += template_func6_fwd1.cc

FILTER_TEST += template_func6_defafteruse1.cc
FILTER_TEST += template_func6_defafteruse2.cc
FILTER_TEST += template_func6_defafteruse3.cc

# function template forwarding with both declaration and definition of
# specialization after use; this is illegal C++ that we should give a
# user error for.  FIX: make this an elsa failing test.  FIX: we do
# not catch this one; the failure mode is that we call the most
# specific specialization declared available at the point of
# instantiation
#FILTER_TEST += template_func6_defafteruse4.cc

FILTER_TEST += template_func6_recursive1.cc
FILTER_TEST += template_func6_recursive3.cc
# mutual recursion for specializations
FILTER_TEST += template_func6_recursive4.cc

# testing function template overloading
FILTER_TEST += template_func7_oload1.cc
FILTER_TEST += template_func7_oload2.cc
FILTER_TEST += template_func7_oload3.cc
FILTER_TEST += template_func7_oload4.cc
FILTER_TEST += template_func7_oload5.cc
FILTER_TEST += template_func7_oload6.cc

FILTER_TEST += template_class1.cc
FILTER_TEST += template_class2.cc
# test forwarded class templates in the presence of specializations
FILTER_TEST += template_class2_forward1.cc
FILTER_TEST += template_class2_multiple.cc
FILTER_TEST += template_class3_partial_int.cc
FILTER_TEST += template_class3_partial_cvatomic.cc
FILTER_TEST += template_class3_partial_pointer.cc
FILTER_TEST += template_class3_partial_ref.cc
FILTER_TEST += template_class3_partial_array.cc

# FIX: Bug in the interaction between typchecking function and class
# when a function occurs in a template argument
# FILTER_TEST += template_class3_partial_function.cc

FILTER_TEST += template_class3_partial_ptr-to-mem.cc
FILTER_TEST += template_class3_partial_ptr-to-mem2.cc

FILTER_TEST += stack_cdtor1.cc
FILTER_TEST += stack_cdtor2.cc
FILTER_TEST += pass1.cc
FILTER_TEST += pass2.cc
FILTER_TEST += pass2int.cc
FILTER_TEST += return1.cc
FILTER_TEST += return2.cc
FILTER_TEST += return3.cc
FILTER_TEST += heap_cdtor1.cc
FILTER_TEST += heap_cdtor2.cc
FILTER_TEST += heap_cdtor3.cc
FILTER_TEST += immediate_cdtor1.cc
FILTER_TEST += immediate_cdtor2.cc
FILTER_TEST += immediate_cdtor2b.cc
FILTER_TEST += immediate_cdtor3.cc
FILTER_TEST += memberInit_cdtor1.cc
FILTER_TEST += memberInit_cdtor1b.cc
FILTER_TEST += memberInit_cdtor2.cc
FILTER_TEST += memberInit_cdtor2_1.cc
FILTER_TEST += memberInit_cdtor2_1b.cc
FILTER_TEST += memberInit_cdtor2b.cc
FILTER_TEST += memberInit_cdtor3.cc
FILTER_TEST += memberInit_cdtor3b.cc
FILTER_TEST += cn_decl1.cc

FILTER_TEST += exception1.cc
FILTER_TEST += exception1b.cc
FILTER_TEST += exception1c.cc
FILTER_TEST += exception1d.cc
FILTER_TEST += exception1E.cc
FILTER_TEST += exception1bE.cc
FILTER_TEST += exception1cE.cc
FILTER_TEST += exception1dE.cc

# ->*
FILTER_TEST += ptr-to-member1.cc
FILTER_TEST += ptr-to-member1d.cc
FILTER_TEST += ptr-to-member3.cc
FILTER_TEST += ptr-to-member3d.cc
FILTER_TEST += ptr-to-member1-B.cc
FILTER_TEST += ptr-to-member1d-B.cc
FILTER_TEST += ptr-to-member3-B.cc
FILTER_TEST += ptr-to-member3d-B.cc

# .*
FILTER_TEST += ptr-to-member1-B_dot.cc
FILTER_TEST += ptr-to-member1_dot.cc
FILTER_TEST += ptr-to-member1d-B_dot.cc
FILTER_TEST += ptr-to-member1d_dot.cc
FILTER_TEST += ptr-to-member3-B_dot.cc
FILTER_TEST += ptr-to-member3_dot.cc
FILTER_TEST += ptr-to-member3d-B_dot.cc
FILTER_TEST += ptr-to-member3d_dot.cc

# ?:
FILTER_TEST += cond_omit_then1.cc

FILTER_TEST += ptr-to-member1P.cc

FILTER_TEST += ptr-to-member2.cc
# I can't figure out what this is testing
# FILTER_TEST += ptr-to-member9.cc

FILTER_TEST += t1000.cc
FILTER_TEST += t1001.cc
FILTER_TEST += t1001c.cc
# I think this is just wrong
#FILTER_TEST += t1001d.cc

# These tests were attempts to test the fan-up/fan-down properties of
# assigning to and from an int and int*.  However, I have changed this
# effect to only work for void*-s.
#FILTER_TEST += t1001e.cc
#FILTER_TEST += t1004c.cc

# FILTER_TEST += t1002.cc
# FIX: BUG!
#FILTER_TEST += t1002b.cc
# this is just not legal C nor C++
# FILTER_TEST += t1003.cc
FILTER_TEST += t1004.cc
FILTER_TEST += t1004b.cc
FILTER_TEST += t1005.cc
FILTER_TEST += t1005b.cc
FILTER_TEST += t1005c.cc
FILTER_TEST += t1005d.cc
FILTER_TEST += t1006.cc
# FIX: Won't let me attach qualifiers to array declarations.
#FILTER_TEST += t1006b.cc
# FIX: Don't know how to apply qualifiers to a function.
#FILTER_TEST += t1007.cc
# FIX: applying qualifiers to an array
#FILTER_TEST += t1008.cc
# FIX: applying qualifiers to an array
#FILTER_TEST += t1009.cc
# FIX: applying qualifiers to an array
#FILTER_TEST += t1010.cc
# empty; not sure if meaningful
#FILTER_TEST += t1011.cc
# FIX: Don't know how to apply qualifiers to a function.
#FILTER_TEST += t1012.cc
# FIX: Don't know how to apply qualifiers to a function.
#FILTER_TEST += t1013.cc
# FIX: Don't know how to apply qualifiers to a function and array.
#FILTER_TEST += t1014.cc
FILTER_TEST += t1015.cc

FILTER_TEST += t1210simple.c
FILTER_TEST += t1211simple.c
FILTER_TEST += t1220init.c
FILTER_TEST += t1221funcall.c
FILTER_TEST += t1222funptr.c
FILTER_TEST += t1223cast.c
FILTER_TEST += t1200ptrfuncall.c
FILTER_TEST += t1202ptrfuncall.c
# Not even legal C
# FILTER_TEST += t1203ptrfuncall.c
# Not even legal C
# FILTER_TEST += t1204ptrfuncall.c

# I have now turned on casts-preserve by default for the taint
# analysis, so this test just doesn't work any more.
# FILTER_TEST += t1330_cast.c

FILTER_TEST += t1340_const.c
FILTER_TEST += t1341_const.c
FILTER_TEST += t1342_const.c
FILTER_TEST += t1343_const.c
FILTER_TEST += t1350_ref.cc
FILTER_TEST += t1351_ref.cc
FILTER_TEST += t1352_ref.cc
# sm: t1353_ref.cc triggers an assertion failure in Oink because
# the argument to '++' is (I believe correctly) computed to have
# type 'int', not 'int const &' as Oink expects (granted, Elsa
# ideally should reject the program for not having an lvalue
# argument to '++', but that's another whole can of worms)
# FILTER_TEST += t1353_ref.cc
# this relies on elsa not doing const checking which it now does; see
# the note in the test
#FILTER_TEST += t1354_ref.cc

FILTER_TEST += method_call4.cc
FILTER_TEST += method_call5.cc
FILTER_TEST += method_call6.cc
FILTER_TEST += method_call7.cc
FILTER_TEST += method_call8.cc
FILTER_TEST += method_call9.cc

FILTER_TEST += memcpy_struct.c
FILTER_TEST += return_struct_ptr.c

#FILTER_TEST += function_pointer_in_array.c

FILTER_TEST += auto_union_struct1.c
FILTER_TEST += auto_union_struct2.c

# test that extern inline functions work
C_FILTER_TEST += extern_inline_func1.c
C_FILTER_TEST += void_auto_union.c
C_FILTER_TEST += instance_sensitive3.i
C_FILTER_TEST += return_struct_fs.c
C_FILTER_TEST += pass_struct.c
C_FILTER_TEST += transparent_union1.c

# test these with instance-sensitivity ON
C_FILTER_TEST += vararg_input1.c
# C_FILTER_TEST += vararg_input2.c
C_FILTER_TEST += vararg_output1.c
C_FILTER_TEST += vararg_copy1.c
C_FILTER_TEST += vararg_copy2.c
C_FILTER_TEST += vararg_valist1.c

# and with instance-sensitivity OFF
FILTER_TEST += vararg_input1.c
# FILTER_TEST += vararg_input2.c
FILTER_TEST += vararg_output1.c
FILTER_TEST += vararg_copy1.c
FILTER_TEST += vararg_copy2.c
FILTER_TEST += vararg_valist1.c

FILTER_TEST += expect1.cc

# check for polymorphic features ****************

POLY_FILTER_TEST :=
POLY_FILTER_TEST += poly_global1.cc
POLY_FILTER_TEST += poly_global2.cc

# ****************

# FIX: these should be elsa tests
# NOTE NOTE NOTE: Different kind of bad here.
# These tested that if you called a function with the wrong number of
# arguments that the dataflow analysis would catch it; it is now
# caught in elsa, so the return error is 4 not 1 as it was.
FILTER_USER_TEST :=
FILTER_USER_TEST := t1300_args.c
FILTER_USER_TEST += t1301_args.c
FILTER_USER_TEST += t1302_args.c
FILTER_USER_TEST += t1303_args.c
FILTER_USER_TEST += t1310_varargs.c
FILTER_USER_TEST += t1311_varargs.c
FILTER_USER_TEST += t1312_varargs.c
FILTER_USER_TEST += t1313_varargs.c
FILTER_USER_TEST += t1320_voidarg.c

.PRECIOUS: %.filter-good.cc
TEST_TOCLEAN += Test/*.filter-good.cc
%.filter-good.cc: %
	@echo; echo QUAL FILTER GOOD $@
	./test_filter -good <$* >$@

.PRECIOUS: %.filter-bad.cc
TEST_TOCLEAN += Test/*.filter-bad.cc
%.filter-bad.cc: %
	@echo; echo QUAL FILTER BAD $@
	./test_filter -bad <$* >$@

# NOTE: two kinds of bad here
# NOTE: C tests default to instance-sensitive
# NOTE: even though we have polymorphic-specific tests, all tests
# default to polymorphic now
.PHONY: qual-check-filter-good/%
qual-check-filter-good/%: %.filter-good.cc
	$(QUALCFG) $<
.PHONY: qual-check-filter-good-inst-sens/%
qual-check-filter-good-inst-sens/%: %.filter-good.cc
	$(QUALCFG) -fo-instance-sensitive $<
.PHONY: qual-check-filter-good-poly/%
qual-check-filter-good-poly/%: %.filter-good.cc
	$(QUALCFG) -fq-poly $<
.PHONY: qual-check-c-filter-good/%
qual-check-c-filter-good/%: %.filter-good.cc
	$(QUALCFG) -o-lang GNU2_KandR_C -fo-instance-sensitive $<
.PHONY: qual-check-c-filter-good-taintXcast/%
qual-check-c-filter-good-taintXcast/%: %.filter-good.cc
	$(QUAL) -o-lang GNU2_KandR_C -fo-instance-sensitive -q-config qual_taintXcasts.lattice $<

.PHONY: qual-check-filter-bad/%
qual-check-filter-bad/%: %.filter-bad.cc
	$(QUALCFG) $<; test $$? -eq 32
.PHONY: qual-check-filter-bad-inst-sens/%
qual-check-filter-bad-inst-sens/%: %.filter-bad.cc
	$(QUALCFG) -fo-instance-sensitive $<; test $$? -eq 32
.PHONY: qual-check-filter-bad-poly/%
qual-check-filter-bad-poly/%: %.filter-bad.cc
	$(QUALCFG) -fq-poly $<; test $$? -eq 32
.PHONY: qual-check-filter-bad/%
qual-check-c-filter-bad/%: %.filter-bad.cc
	$(QUALCFG) -o-lang GNU2_KandR_C -fo-instance-sensitive $<; test $$? -eq 32
.PHONY: qual-check-c-filter-bad-taintXcast/%
qual-check-c-filter-bad-taintXcast/%: %.filter-bad.cc
	$(QUAL) -o-lang GNU2_KandR_C -fo-instance-sensitive -q-config qual_taintXcasts.lattice $<; test $$? -eq 32
.PHONY: qual-check-filter-userbad/%
qual-check-filter-userbad/%: %.filter-bad.cc
	$(QUALCFG) $<; test $$? -eq 4

# NOTE: the assymetry between good and bad
.PHONY: qual-check-filter-good qual-check-filter-bad qual-check-filter-userbad

qual-check-filter-good: $(addprefix qual-check-filter-good/Test/,$(FILTER_TEST))
# instance-sensitivity not implemented for C++
# qual-check-filter-good: $(addprefix qual-check-filter-good-inst-sens/Test/,$(FILTER_TEST_CC_INST_SENS))
qual-check-filter-good: $(addprefix qual-check-filter-good-poly/Test/,$(POLY_FILTER_TEST))
qual-check-filter-good: $(addprefix qual-check-c-filter-good/Test/,$(C_FILTER_TEST))
qual-check-filter-good: $(addprefix qual-check-c-filter-good-taintXcast/Test/,$(C_FILTER_TEST_TAINTXCAST))
qual-check-filter-good: $(addprefix qual-check-filter-good/Test/,$(FILTER_USER_TEST))

qual-check-filter-bad: $(addprefix qual-check-filter-bad/Test/,$(FILTER_TEST))
# instance-sensitivity not implemented for C++
# qual-check-filter-bad: $(addprefix qual-check-filter-bad-inst-sens/Test/,$(FILTER_TEST_CC_INST_SENS))
qual-check-filter-bad: $(addprefix qual-check-filter-bad-poly/Test/,$(POLY_FILTER_TEST))
qual-check-filter-bad: $(addprefix qual-check-c-filter-bad/Test/,$(C_FILTER_TEST))
qual-check-filter-bad: $(addprefix qual-check-c-filter-bad-taintXcast/Test/,$(C_FILTER_TEST_TAINTXCAST))
qual-check-filter-userbad: $(addprefix qual-check-filter-userbad/Test/,$(FILTER_USER_TEST))

# just some more simple polymorphism tests
.PHONY: qual-check-poly
qual-check-poly: qual-check-poly1
qual-check-poly1:
	$(QUALCFG) -fq-no-poly Test/poly1.c; test $$? -eq 32
	$(QUALCFG) -fq-poly Test/poly1.c
	$(ANNOUNCE_TEST_PASS)

qual-check-poly: qual-check-poly4
qual-check-poly4:
	$(QUALCFG) -fq-no-poly Test/poly4.c; test $$? -eq 32
	$(QUALCFG) -fq-poly Test/poly4.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# ****************

.PHONY: qual-ellipsis/%
qual-ellipsis/%:
	./qual -fq-casts-preserve -q-config ../libqual/config/lattice $*; echo $$?
	$(ANNOUNCE_TEST_PASS)

.PHONY: qual-ellipsis-is/%
qual-ellipsis-is/%:
	./qual -fo-instance-sensitive -fq-casts-preserve -q-config ../libqual/config/lattice $*; echo $$?
	$(ANNOUNCE_TEST_PASS)

qual-ellipsis: qual-ellipsis/Test/vararg_input0.c
qual-ellipsis: qual-ellipsis/Test/vararg_input0b.c
qual-ellipsis: qual-ellipsis-is/Test/vararg_input0.c
qual-ellipsis: qual-ellipsis-is/Test/vararg_input0b.c

qual-check-function-pointer-in-array1:
	$(QUALCFG) -o-lang GNU_C Test/function_pointer_in_array.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-function-pointer-in-array2:
	$(QUALCFG) -o-lang ANSI_Cplusplus Test/function_pointer_in_array.c; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)


qual-check-struct-statement-expr1:
	$(QUALCFG) Test/struct_statement_expr1.c
	$(ANNOUNCE_TEST_PASS)

qual-check-mismatched-funcptr-param1:
	$(QUALCFG) Test/mismatched_funcptr_param1.c  -fo-no-merge-E_variable-and-var-values
	$(QUALCFG) Test/mismatched_funcptr_param1b.c -fo-no-merge-E_variable-and-var-values
	$(QUALCFG) Test/mismatched_funcptr_param2.c  -fo-no-merge-E_variable-and-var-values
	$(QUALCFG) Test/mismatched_funcptr_param2b.c -fo-no-merge-E_variable-and-var-values
	$(QUALCFG) Test/mismatched_funcptr_param1.c
	$(QUALCFG) Test/mismatched_funcptr_param1b.c
	$(QUALCFG) Test/mismatched_funcptr_param2.c
	$(QUALCFG) Test/mismatched_funcptr_param2b.c
	$(ANNOUNCE_TEST_PASS)

qual-check-inconsistent-func-type-param1:
	$(QUALCFG) Test/inconsistent_func_type_param1.c
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-inconsistent-func-type-param2
qual-check-inconsistent-func-type-param2:
	$(QUALCFG) Test/inconsistent_func_type_param2.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-inconsistent-func-type-param3
qual-check-inconsistent-func-type-param3:
	$(QUALCFG) Test/inconsistent_func_type_param3.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# 2679bf5a-87cb-40ca-891f-28e060d30cf3
# XFAIL += qual-check-inconsistent-func-type-param4
qual-check: qual-check-inconsistent-func-type-param4
qual-check-inconsistent-func-type-param4:
	$(QUALCFG) Test/inconsistent_func_type_param4.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# 4798ba56-1da1-48ce-8371-1cfddccfd17c
# XFAIL += qual-check-inconsistent-func-type-param5
qual-check: qual-check-inconsistent-func-type-param5
qual-check-inconsistent-func-type-param5:
	$(QUALCFG) Test/inconsistent_func_type_param5.c
	$(ANNOUNCE_TEST_PASS)

# XFAIL += qual-check-inconsistent-func-type-param6
qual-check: qual-check-inconsistent-func-type-param6
qual-check-inconsistent-func-type-param6:
	$(QUALCFG) Test/inconsistent_func_type_param6.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# XFAIL += qual-check-inconsistent-func-type-param7
qual-check: qual-check-inconsistent-func-type-param7
qual-check-inconsistent-func-type-param7:
	$(QUALCFG) Test/inconsistent_func_type_param7.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# XFAIL += qual-check-inconsistent-func-type-param8
qual-check: qual-check-inconsistent-func-type-param8
qual-check-inconsistent-func-type-param8:
	$(QUALCFG) Test/inconsistent_func_type_param8.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-keywords1:
	$(QUALCFG) Test/keywords1.cc
	$(ANNOUNCE_TEST_PASS)

qual-check-noparam-compound1:
	$(QUALCFG) Test/noparam_compound1.c
	$(ANNOUNCE_TEST_PASS)

qual-check-nonfunc-cast-to-func: qual-check-nonfunc-cast-to-func1 qual-check-nonfunc-cast-to-func2

qual-check-nonfunc-cast-to-func1:
	$(QUALCFG) Test/nonfunc_cast_to_func1.cc
	$(ANNOUNCE_TEST_PASS)

qual-check-nonfunc-cast-to-func2:
	$(QUALCFG) Test/nonfunc_cast_to_func2.cc
	$(ANNOUNCE_TEST_PASS)

qual-check-funccast-to-nonfunc1:
	$(QUALCFG) Test/funccast_to_nonfunc1.cc
	$(ANNOUNCE_TEST_PASS)

# check the combination of instance sensitive and compound
# initializers; this is a bit redundant with the same test being in
# C_FILTER_TEST
FILTER_INST_SENS :=
FILTER_INST_SENS += Test/inst_sens_cpdinit1.c
qual-check-instance: $(addprefix qual-check-instance-good/,$(FILTER_INST_SENS))
qual-check-instance: $(addprefix qual-check-instance-bad/,$(FILTER_INST_SENS))
$(addprefix qual-check-instance-good/,$(FILTER_INST_SENS)): qual-check-instance-good/%:
	./test_filter -good <$* >$*.filter-good.c
# NOTE: the assymmetry here: with instance-sensitivity off, the good test still warns
	$(QUALCFG) $*.filter-good.c; test $$? -eq 32
# NO warning
	$(QUALCFG) -fo-instance-sensitive $*.filter-good.c
$(addprefix qual-check-instance-bad/,$(FILTER_INST_SENS)): qual-check-instance-bad/%:
	./test_filter -bad <$* >$*.filter-bad.c
# warning
	$(QUALCFG) $*.filter-bad.c; test $$? -eq 32
# warning
	$(QUALCFG) -fo-instance-sensitive $*.filter-bad.c; test $$? -eq 32

qual-check-instance-srz: qual-check-instance-srz1
qual-check-instance-srz1: qual-check-instance-srz1-isens qual-check-instance-srz1-noisens

qual-check-instance-srz1-isens:
# 	test without serialization:
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz1a.c ; test $$? -eq 32
# 	test with serialization, instance sensitivity ON:
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz1a.c -o-srz Test/instance_sensitive_srz1a_isens.qdir ; test $$? -eq 32
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz1a_isens.qdir ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-instance-srz1-noisens:
# 	test without serialization:
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz1a.c ; test $$? -eq 32
#	test with serialization, instance sensitivity ON:
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz1a.c -o-srz Test/instance_sensitive_srz1a_noisens.qdir ; test $$? -eq 32
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz1a_noisens.qdir ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# instance_sensitive_srz2[ab].c: expect warning in both cases

qual-check-instance-srz: qual-check-instance-srz2
qual-check-instance-srz2: qual-check-instance-srz2-isens qual-check-instance-srz2-noisens

qual-check-instance-srz2-isens:
# 	test without serialization:
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz2a.c Test/instance_sensitive_srz2b.c ; test $$? -eq 32
# 	test with serialization, instance sensitivity ON:
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz2a.c -o-srz Test/instance_sensitive_srz2a_isens.qdir
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz2b.c -o-srz Test/instance_sensitive_srz2b_isens.qdir
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz2a_isens.qdir Test/instance_sensitive_srz2b_isens.qdir ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-instance-srz2-noisens:
# 	test without serialization:
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz2a.c Test/instance_sensitive_srz2b.c ; test $$? -eq 32
#	test with serialization, instance sensitivity ON:
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz2a.c -o-srz Test/instance_sensitive_srz2a_noisens.qdir
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz2b.c -o-srz Test/instance_sensitive_srz2b_noisens.qdir
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz2a_noisens.qdir Test/instance_sensitive_srz2b_noisens.qdir ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# Test/instance_sensitive_srz3[ab].c: expect warning only when not instance sensitive

qual-check-instance-srz: qual-check-instance-srz3
qual-check-instance-srz3: qual-check-instance-srz3-isens qual-check-instance-srz3-noisens

qual-check-instance-srz3-isens:
# 	test without serialization:
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz3a.c Test/instance_sensitive_srz3b.c ; test $$? -eq 0
# 	test with serialization, instance sensitivity ON:
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz3a.c -o-srz Test/instance_sensitive_srz3a_isens.qdir
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz3b.c -o-srz Test/instance_sensitive_srz3b_isens.qdir
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz3a_isens.qdir Test/instance_sensitive_srz3b_isens.qdir ; test $$? -eq 0
	$(ANNOUNCE_TEST_PASS)

qual-check-instance-srz3-noisens:
# 	test without serialization:
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz3a.c Test/instance_sensitive_srz3b.c ; test $$? -eq 32
#	test with serialization, instance sensitivity ON:
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz3a.c -o-srz Test/instance_sensitive_srz3a_noisens.qdir
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz3b.c -o-srz Test/instance_sensitive_srz3b_noisens.qdir
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz3a_noisens.qdir Test/instance_sensitive_srz3b_noisens.qdir ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)


qual-check-instance-srz: qual-check-instance-srz4
qual-check-instance-srz4: qual-check-instance-srz4-isens qual-check-instance-srz4-noisens

qual-check-instance-srz4-isens:
# 	test without serialization:
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz4a.c Test/instance_sensitive_srz4b.c ; test $$? -eq 32
# 	test with serialization, instance sensitivity ON:
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz4a.c -o-srz Test/instance_sensitive_srz4a_isens.qdir
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz4b.c -o-srz Test/instance_sensitive_srz4b_isens.qdir
	$(QUALCFG) -fo-instance-sensitive Test/instance_sensitive_srz4a_isens.qdir Test/instance_sensitive_srz4b_isens.qdir ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-instance-srz4-noisens:
# 	test without serialization:
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz4a.c Test/instance_sensitive_srz4b.c ; test $$? -eq 32
#	test with serialization, instance sensitivity ON:
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz4a.c -o-srz Test/instance_sensitive_srz4a_noisens.qdir
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz4b.c -o-srz Test/instance_sensitive_srz4b_noisens.qdir
	$(QUALCFG) -fo-no-instance-sensitive Test/instance_sensitive_srz4a_noisens.qdir Test/instance_sensitive_srz4b_noisens.qdir ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)


# ed1f952c-dbf5-41bf-9778-d5b0c0bda5af
XFAIL += qual-check-funcptr-template1
qual-check-funcptr-template1:
	$(QUALCFG) Test/funcptr_template1.cc -o-srz Test/funcptr_template1.1.qdir
	$(QUALCFG) Test/funcptr_template1.1.qdir -o-srz Test/funcptr_template1.2.qdir
	$(ANNOUNCE_TEST_PASS)

XFAIL += qual-check-templatized-on-func1
qual-check-templatized-on-func1:
	$(QUALCFG) Test/templatized_on_func1.cc -o-srz Test/templatized_on_func1.qdir
	$(QUALCFG) Test/templatized_on_func1.qdir -o-srz Test/templatized_on_func1.2.qdir
	$(ANNOUNCE_TEST_PASS)

XFAIL += qual-check-templatized-on-func2
qual-check-templatized-on-func2:
	$(QUALCFG) Test/templatized_on_func2.cc -o-srz Test/templatized_on_func2.qdir
	$(QUALCFG) Test/templatized_on_func2.qdir -o-srz Test/templatized_on_func2.2.qdir
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-using-alias1
qual-check-using-alias1:
	$(QUALCFG) Test/using_alias1a.cc Test/using_alias1b.cc; test $$? -eq 32
	$(QUALCFG) Test/using_alias1a.cc -o-srz Test/using_alias1a.qdir
	$(QUALCFG) Test/using_alias1b.cc -o-srz Test/using_alias1b.qdir
	$(QUALCFG) Test/using_alias1a.qdir Test/using_alias1b.qdir; test $$? -eq 32
	rm -rf Test/using_alias1a.qdir Test/using_alias1b.qdir
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-virtual1
qual-check-virtual1:
	$(QUALCFG) Test/virtual1.cc ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-virtual2
qual-check-virtual2:
	$(QUALCFG) Test/virtual2.cc ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-virtual3
qual-check-virtual3:
	$(QUALCFG) Test/virtual3.cc ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-link-template-static-member1
qual-check-link-template-static-member1:
	$(QUALCFG) Test/template_static_member1a.cc Test/template_static_member1b.cc ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# builtin type signatures
qual-check: qual-check-builtin2
qual-check-builtin2:
	$(QUALCFG) Test/builtin2.cc
	$(ANNOUNCE_TEST_PASS)

# linking in C++
qual-check: qual-check-builtin3
qual-check-builtin3:
	$(QUALCFG) Test/builtin2.cc Test/builtin3.cc ; test $$? -eq 32
# the next test is a superset of the first, but the $$? -eq 32 is what is more important
	$(QUALCFG) Test/builtin2.cc Test/builtin3.cc -fo-report-link-errors ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# linking in C
qual-check: qual-check-builtin3b
qual-check-builtin3b:
	$(QUALCFG) Test/builtin2b.c Test/builtin3b.c ; test $$? -eq 32
	$(QUALCFG) Test/builtin2b.c Test/builtin3b.c -fo-report-link-errors ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-link-instantiate1
qual-check-link-instantiate1:
	$(QUALCFG) Test/instantiate1a.cc Test/instantiate1b.cc ; test $$? -eq 32
#
	$(QUALCFG) Test/instantiate1a.cc -o-srz Test/instantiate1a.1.qdir
	$(QUALCFG) Test/instantiate1b.cc -o-srz Test/instantiate1b.1.qdir
	$(QUALCFG) Test/instantiate1a.1.qdir Test/instantiate1b.1.qdir ; test $$? -eq 32
#
	$(QUALCFG) Test/instantiate1a.1.qdir -o-srz Test/instantiate1a.2.qdir
	$(QUALCFG) Test/instantiate1b.1.qdir -o-srz Test/instantiate1b.2.qdir
	$(QUALCFG) Test/instantiate1a.2.qdir Test/instantiate1b.2.qdir ; test $$? -eq 32

qual-check: qual-check-attralias1
qual-check-attralias1:
	$(QUALCFG) Test/attralias1.c -fo-report-link-errors; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# **************** check function granularity CFG filtering

FUNC_GRAN_TEST :=
FUNC_GRAN_TEST += Test/func_gran1q.cc
FUNC_GRAN_TEST += Test/func_gran2q.cc
FUNC_GRAN_TEST += Test/func_gran3q.cc
FUNC_GRAN_TEST += Test/func_gran4q.cc
FUNC_GRAN_TEST += Test/func_gran5q.cc

FUNC_GRAN_TEST_BAD += Test/func_gran7.cc

# just test it runs without failing and doesn't change
TEST_TOCLEAN += $(addsuffix .fgcfg,$(FUNC_GRAN_TEST))
qual-check-func-gran: $(addprefix qual-check-func-gran/,$(FUNC_GRAN_TEST)) $(addprefix qual-check-func-gran-bad/,$(FUNC_GRAN_TEST_BAD))
	$(ANNOUNCE_TEST_PASS)

$(addprefix qual-check-func-gran/,$(FUNC_GRAN_TEST)): qual-check-func-gran/%:
	$(QUALCFG) $*; test $$? -eq 32
	./qual $(QUALFLAGS) -fq-no-inference -fo-func-gran $* -o-srz $*.fgcfg
	./digraph_component --root=super-main $*.fgcfg > $*.vars
	rm -f $*.fgcfg
	$(QUALCFG) $* -o-func-filter $*.vars
	$(QUALCFG) $* -o-func-filter $*.vars -o-srz $*.qdir
	rm -f $*.vars
	rm -rf $*.qdir
	$(ANNOUNCE_TEST_PASS)

XFAIL += qual-check-func-gran-flow-compoundDown
qual-check-func-gran-flow-compoundDown:
	./qual -fq-no-explain-errors -fq-no-names -fq-flow-compoundDown -q-config ../libqual/config/lattice Test/func_gran1q.cc -o-func-filter Test/func_gran1q.cc.vars
# when this is fixed, enable "QUALFLAGS := -fq-flow-compoundDown" in
# Makefile.in.

qual-check-func-gran-bad/%:
	$(QUALCFG) $*; test $$? -eq 32
	./qual -fq-no-inference -fo-func-gran $* -o-srz $*.fgcfg
	./digraph_component --root=super-main $*.fgcfg > $*.vars
	rm -f $*.fgcfg
	$(QUALCFG) $* -o-func-filter $*.vars ; test $$? -eq 32
	$(QUALCFG) $* -o-func-filter $*.vars -o-srz $*.qdir ; test $$? -eq 32
	rm -f $*.vars
	rm -rf $*.qdir
	$(ANNOUNCE_TEST_PASS)

# check works when linking is involved
FUNC_GRAN_TEST_L :=
FUNC_GRAN_TEST_L += Test/func_gran6q
qual-check-func-gran-L: $(addprefix qual-check-func-gran-L/,$(FUNC_GRAN_TEST_L))
	$(ANNOUNCE_TEST_PASS)

$(addprefix qual-check-func-gran-L/,$(FUNC_GRAN_TEST_L)): qual-check-func-gran-L/%:
# test works normally
	$(QUALCFG) $*_1.cc $*_2.cc -fo-report-link-errors; test $$? -eq 32
# do the abstract build
	./qual $(QUALFLAGS) -fq-no-inference -fo-func-gran $*_1.cc -o-srz $*_1.fgcfg
	./qual $(QUALFLAGS) -fq-no-inference -fo-func-gran $*_2.cc -o-srz $*_2.fgcfg
	./digraph_component --root=super-main $*_1.fgcfg $*_2.fgcfg > $*.vars
	rm -f $*_1.fgcfg $*_2.fgcfg
# check no warning when done together with abstract build configuration
	$(QUALCFG) $*_1.cc $*_2.cc -o-func-filter $*.vars -fo-report-link-errors
# check no warning when done separately with abstract build configuration
	$(QUALCFG) $*_1.cc -o-func-filter $*.vars -o-srz $*_1.qdir
	$(QUALCFG) $*_2.cc -o-func-filter $*.vars -o-srz $*_2.qdir
	$(QUALCFG) $*_1.qdir $*_2.qdir -fo-report-link-errors -fo-all-pass-filter -o-func-filter $*.vars
	rm -f $*.vars
	rm -rf $*_1.qdir $*_2.qdir
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-template-instant2
qual-check-template-instant2:
	$(QUALCFG) -fo-report-link-errors Test/template_instant2a.cc Test/template_instant2b.cc ; test $$? -eq 32
	$(QUALCFG) Test/template_instant2a.cc -o-srz Test/template_instant2a.qdir
	$(QUALCFG) Test/template_instant2b.cc -o-srz Test/template_instant2b.qdir
	$(QUALCFG) -fo-report-link-errors Test/template_instant2a.qdir Test/template_instant2b.qdir ; test $$? -eq 32
	rm -rf Test/template_instant2a.qdir Test/template_instant2b.qdir
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-fread1
qual-check-fread1:
	$(QUALCFG) -fq-flow-compoundDown Test/fread1.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-fread2
qual-check-fread2:
	$(QUALCFG) -fq-flow-compoundDown -fq-flow-pointDown Test/fread2.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-memcpy1
qual-check-memcpy1:
	$(QUALCFG) -fo-no-instance-sensitive Test/memcpy1.c ; test $$? -eq 32
	$(QUALCFG) -fo-instance-sensitive Test/memcpy1.c ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-assign1
qual-check-assign1:
	$(QUALCFG) Test/assign1.cc ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check: qual-check-assign2
qual-check-assign2:
	$(QUALCFG) Test/assign2.cc ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS) 

qual-check: qual-check-array-index-flows1and2
qual-check-array-index-flows1and2:
	$(QUALCFG) Test/array_index_flows1.cc ; test $$? -eq 0
	$(QUALCFG) -fo-array-index-flows Test/array_index_flows1.cc ; test $$? -eq 32
	$(QUALCFG) Test/array_index_flows2.cc ; test $$? -eq 0
	$(QUALCFG) -fo-array-index-flows Test/array_index_flows2.cc ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS) 

qual-check: qual-check-copy1
qual-check-copy1:
	$(QUALCFG) Test/copy1.cc ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS) 

# **** check stackness analysis

CFGS := $(LIBQUAL_DIR)/config/stackness.lattice
QUALCFGS := $(QUAL) -fq-stackness -q-config $(CFGS)

STACKNESS_GOOD :=
STACKNESS_GOOD += Test/stackness0.c

STACKNESS_BAD :=
STACKNESS_BAD += Test/stackness1.c
STACKNESS_BAD += Test/stackness2.c
STACKNESS_BAD += Test/stackness3.c
STACKNESS_BAD += Test/stackness4.c

qual-check: qual-check-stackness

.PHONY: qual-check-stackness
qual-check-stackness: $(addprefix qual-check-stackness-good/,$(STACKNESS_GOOD))
qual-check-stackness: $(addprefix qual-check-stackness-bad/,$(STACKNESS_BAD))

.PHONY: qual-check-stackness-good/%
qual-check-stackness-good/%:
#	$(QUALCFGS) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUALCFGS) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-stackness-bad/%
qual-check-stackness-bad/%:
#	$(QUALCFGS) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUALCFGS) $* ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS) 

# **** check const_when_param analysis

# QUAL_CWP := $(QUAL) -fq-no-inference -q-const-when-param '$$!const_when_param'
# QUAL_CWPPTR := $(QUAL) -fq-no-inference -q-const-when-param '$$!const_when_param*'
QUAL_CWP    := $(QUALCFG) -q-const-when-param '$$!const_when_param'
QUAL_CWPPTR := $(QUALCFG) -q-const-when-param '$$!const_when_ptr_to_param*'

CWP_GOOD :=
CWP_GOOD += Test/const_when_param1.c
CWPPTR_GOOD += Test/const_when_param_ptr1.c

CWP_BAD :=
CWP_BAD += Test/const_when_param2.c
CWPPTR_BAD += Test/const_when_param_ptr2.c

qual-check: qual-check-const_when_param

.PHONY: qual-check-const_when_param
qual-check-const_when_param: $(addprefix qual-check-const_when_param-good/,$(CWP_GOOD))
qual-check-const_when_param: $(addprefix qual-check-const_when_param-bad/,$(CWP_BAD))
qual-check-const_when_param: $(addprefix qual-check-const_when_param-ptr-good/,$(CWPPTR_GOOD))
qual-check-const_when_param: $(addprefix qual-check-const_when_param-ptr-bad/,$(CWPPTR_BAD))

.PHONY: qual-check-const_when_param-good/%
qual-check-const_when_param-good/%:
#	$(QUAL_CWP) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_CWP) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-const_when_param-bad/%
qual-check-const_when_param-bad/%:
#	$(QUAL_CWP) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_CWP) $* ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-const_when_param-ptr-good/%
qual-check-const_when_param-ptr-good/%:
#	$(QUAL_CWPPTR) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_CWPPTR) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-const_when_param-ptr-bad/%
qual-check-const_when_param-ptr-bad/%:
#	$(QUAL_CWPPTR) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_CWPPTR) $* ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS) 

# **** check exclude-global (xglobal) analysis

QUAL_XGLOBAL := $(QUAL) -fq-no-inference -q-exclude-global '$$!nonglobal'

XGLOBAL_GOOD :=
XGLOBAL_GOOD += Test/exclude_good1.c

XGLOBAL_BAD :=
XGLOBAL_BAD += Test/exclude_bad1.c
XGLOBAL_BAD += Test/exclude_bad2.c
XGLOBAL_BAD += Test/exclude_bad3.c
XGLOBAL_BAD += Test/exclude_bad4.c

qual-check: qual-check-xglobal

.PHONY: qual-check-xglobal
qual-check-xglobal: $(addprefix qual-check-xglobal-good/,$(XGLOBAL_GOOD))
qual-check-xglobal: $(addprefix qual-check-xglobal-bad/,$(XGLOBAL_BAD))

.PHONY: qual-check-xglobal-good/%
qual-check-xglobal-good/%:
#	$(QUAL_XGLOBAL) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_XGLOBAL) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-xglobal-bad/%
qual-check-xglobal-bad/%:
#	$(QUAL_XGLOBAL) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_XGLOBAL) $* ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS) 

# **** check exclude-cast (xcast) analysis

QUAL_XCAST := $(QUAL) -fq-no-inference -q-exclude-cast '$$!noncast'

XCAST_GOOD :=
XCAST_GOOD += Test/exclude_cast_good1.c

XCAST_BAD :=
XCAST_BAD += Test/exclude_cast_bad1.c
XCAST_BAD += Test/exclude_cast_bad2.cc

qual-check: qual-check-xcast

.PHONY: qual-check-xcast
qual-check-xcast: $(addprefix qual-check-xcast-good/,$(XCAST_GOOD))
qual-check-xcast: $(addprefix qual-check-xcast-bad/,$(XCAST_BAD))

.PHONY: qual-check-xcast-good/%
qual-check-xcast-good/%:
#	$(QUAL_XCAST) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_XCAST) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-xcast-bad/%
qual-check-xcast-bad/%:
#	$(QUAL_XCAST) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_XCAST) $* ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS) 

# **** check stack variable registration

QUAL_STKVARREG := $(QUAL)
QUAL_STKVARREG += -fq-no-inference -q-reg-stack '$$!regme*'
QUAL_STKVARREG += -q-reg-stack-regfunc register_var
QUAL_STKVARREG += -q-reg-stack-un-regfunc unregister_var
# FIX: the tests are not thorough enough to fail if these names are
# spelled wrong: the tests that use them detect an error but not of
# the kind that the test was intended to test
QUAL_STKVARREG += -q-reg-stack-array-regfunc register_array_var
QUAL_STKVARREG += -q-reg-stack-array-un-regfunc unregister_array_var

STKVARREG_GOOD :=
STKVARREG_GOOD += Test/stkvarreg_good1.c

STKVARREG_BAD :=
STKVARREG_BAD += Test/stkvarreg_bad1.c
STKVARREG_BAD += Test/stkvarreg_bad2.c
STKVARREG_BAD += Test/stkvarreg_bad3.c
STKVARREG_BAD += Test/stkvarreg_bad4.c
STKVARREG_BAD += Test/stkvarreg_bad5.c
STKVARREG_BAD += Test/stkvarreg_bad6.c
STKVARREG_BAD += Test/stkvarreg_bad7.c
STKVARREG_BAD += Test/stkvarreg_bad8.c
STKVARREG_BAD += Test/stkvarreg_bad9.c
STKVARREG_BAD += Test/stkvarreg_bad10.c
STKVARREG_BAD += Test/stkvarreg_bad11.c
STKVARREG_BAD += Test/stkvarreg_bad12.c
STKVARREG_BAD += Test/stkvarreg_bad13.c

qual-check: qual-check-stkvarreg

.PHONY: qual-check-stkvarreg
qual-check-stkvarreg: $(addprefix qual-check-stkvarreg-good/,$(STKVARREG_GOOD))
qual-check-stkvarreg: $(addprefix qual-check-stkvarreg-bad/,$(STKVARREG_BAD))
qual-check-stkvarreg:
# should fail because I didn't set -q-reg-stack-regfunc
	$(QUAL) -fq-no-inference -q-reg-stack '$!regme*' Test/stkvarreg_good1.c ; test $$? -eq 1

.PHONY: qual-check-stkvarreg-good/%
qual-check-stkvarreg-good/%:
#	$(QUAL_STKVARREG) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_STKVARREG) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-stkvarreg-bad/%
qual-check-stkvarreg-bad/%:
#	$(QUAL_STKVARREG) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_STKVARREG) $* ; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS) 

# **** check -q-exl-perm-alias

QUAL_DPFAD := $(QUAL) -fq-no-inference -q-exl-perm-alias '$$!argorderef**'

DPFAD_GOOD :=
DPFAD_GOOD += Test/dpfad_good1.c

DPFAD_BAD :=
DPFAD_BAD += Test/dpfad_bad1.c
DPFAD_BAD += Test/dpfad_bad2.c
DPFAD_BAD += Test/dpfad_bad3.c

qual-check: qual-check-dpfad

.PHONY: qual-check-dpfad
qual-check-dpfad: $(addprefix qual-check-dpfad-good/,$(DPFAD_GOOD))
qual-check-dpfad: $(addprefix qual-check-dpfad-bad/,$(DPFAD_BAD))

.PHONY: qual-check-dpfad-good/%
qual-check-dpfad-good/%:
#	$(QUAL_DPFAD) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_DPFAD) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-dpfad-bad/%
qual-check-dpfad-bad/%:
#	$(QUAL_DPFAD) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_DPFAD) $* ; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS) 

# **** check -q-exl-value-ref-call

QUAL_NVAR := $(QUAL) -fq-no-inference -q-exl-value-ref-call '$$!noargandref*'

NVAR_GOOD :=
NVAR_GOOD += Test/nvar_good1.c

NVAR_BAD :=
NVAR_BAD += Test/nvar_bad1.c
NVAR_BAD += Test/nvar_bad2.c
NVAR_BAD += Test/nvar_bad3.c

qual-check: qual-check-nvar

.PHONY: qual-check-nvar
qual-check-nvar: $(addprefix qual-check-nvar-good/,$(NVAR_GOOD))
qual-check-nvar: $(addprefix qual-check-nvar-bad/,$(NVAR_BAD))

.PHONY: qual-check-nvar-good/%
qual-check-nvar-good/%:
#	$(QUAL_NVAR) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_NVAR) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-nvar-bad/%
qual-check-nvar-bad/%:
#	$(QUAL_NVAR) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_NVAR) $* ; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS) 

# **** check -q-arg-local-var

QUAL_ASPV := $(QUAL) -fq-no-inference -q-arg-local-var '$$!argstackparam*'

ASPV_GOOD :=
ASPV_GOOD += Test/aspv_good1.c

ASPV_BAD :=
ASPV_BAD += Test/aspv_bad1.c
ASPV_BAD += Test/aspv_bad2.c
ASPV_BAD += Test/aspv_bad3.c

qual-check: qual-check-aspv

.PHONY: qual-check-aspv
qual-check-aspv: $(addprefix qual-check-aspv-good/,$(ASPV_GOOD))
qual-check-aspv: $(addprefix qual-check-aspv-bad/,$(ASPV_BAD))

.PHONY: qual-check-aspv-good/%
qual-check-aspv-good/%:
#	$(QUAL_ASPV) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_ASPV) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-aspv-bad/%
qual-check-aspv-bad/%:
#	$(QUAL_ASPV) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_ASPV) $* ; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS) 

# **** check -q-temp-immediately-used

QUAL_TMPIMUSED := $(QUAL) -fq-no-inference -q-temp-immediately-used '$$!useimmediately*'

TMPIMUSED_GOOD :=
TMPIMUSED_GOOD += Test/tmpimused_good1.c

TMPIMUSED_BAD :=
TMPIMUSED_BAD += Test/tmpimused_bad1.c
TMPIMUSED_BAD += Test/tmpimused_bad2.c
TMPIMUSED_BAD += Test/tmpimused_bad3.c

qual-check: qual-check-tmpimused

.PHONY: qual-check-tmpimused
qual-check-tmpimused: $(addprefix qual-check-tmpimused-good/,$(TMPIMUSED_GOOD))
qual-check-tmpimused: $(addprefix qual-check-tmpimused-bad/,$(TMPIMUSED_BAD))

.PHONY: qual-check-tmpimused-good/%
qual-check-tmpimused-good/%:
#	$(QUAL_TMPIMUSED) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_TMPIMUSED) $*
	$(ANNOUNCE_TEST_PASS) 

.PHONY: qual-check-tmpimused-bad/%
qual-check-tmpimused-bad/%:
#	$(QUAL_TMPIMUSED) $* -fo-pretty-print -fq-print-trans-qual -fo-verbose 
	$(QUAL_TMPIMUSED) $* ; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS) 

# **************** debugging support: print the dataflow graph
# GRAPH :=
# GRAPH += Test/t1000.cc
#GRAPH += Test/vararg_input0.c

GRAPH_TARGET :=
GRAPH_TARGET += gronk.c gronkb.c

GRAPH_OUT := gronk

TEST_TOCLEAN += *.dot
TEST_TOCLEAN += *.ps
.PHONY: qual-graph
qual-graph:
	rm -f quals.dot Test/*.dot Test/*.ps
	$(QUALCFG) -fq-name-with-serialno -fq-casts-preserve -fq-name-with-loc -fq-print-quals-graph -fq-ugly -fq-no-ret-inf $(GRAPH_TARGET)
	dot -Tps -o $(GRAPH_OUT)-cc.ps quals.dot
	./qual_graph_component -root '$$tainted'   < quals.dot > $(GRAPH_OUT)-tainted.dot
	dot -Tps -o $(GRAPH_OUT)-tainted.cc.ps   $(GRAPH_OUT)-tainted.dot
	./qual_graph_component -root '$$untainted' < quals.dot > $(GRAPH_OUT)-untainted.dot
	dot -Tps -o $(GRAPH_OUT)-untainted.cc.ps $(GRAPH_OUT)-untainted.dot
# .PHONY: qual-graph
# qual-graph: $(addprefix qual-graph/,$(GRAPH))
