# see License.txt for copyright and terms of use

# Included makefile for testing the qualifier analysis serialization
# feature.

ifndef QUAL_TEST_MAKEFILE
$(error This makefile should be included in qual_test.incl.mk, not used stand-alone)
endif

TEST_TOCLEAN_DIR += Test/*.qdir

SRZ_TESTS :=
SRZ_TESTS += qual-check-srz-regr/Test/taint1.c
SRZ_TESTS += qual-check-srz-regr/Test/allvalues.cc
SRZ_TESTS += qual-check-srz-two-units
SRZ_TESTS += qual-check-srz-single
SRZ_TESTS += qual-check-srz-double
SRZ_TESTS += qual-check-unsat-symbol
SRZ_TESTS += qual-check-control-files

# these are all failing for the same reason: BaseClassSubobj::parents
# are not being serialized a second time; this does not affect C++ so
# I leave it for now
# SRZ_TESTS += qual-check-srz-regr/Test/inherit0.ii
# SRZ_TESTS += elsa-check-srz-regr/Test/inherit0.ii
# SRZ_TESTS += qual-check-srz-regr-cc/../elsa/in/big/nsUnicodeToTeXCMRt1.i

.PHONY: qual-check-srz
qual-check-srz: $(SRZ_TESTS) qual-check-srz-misc

# check some other misc stuff
.PHONY: qual-check-srz-misc
qual-check-srz-misc:
	$(QUAL) -q-config $(CFG) Test/linker_visible1.c
	$(ANNOUNCE_TEST_PASS)

# check we do not accept a request to "compile" more than one file at a time
#   $(QUAL) -q-config $(CFG) $(FA).c $(FB).c -o-srz $(FA).qdir; test $$? -eq 1

# Check the canonicality of serialization: that is, test if we
# serialize and de-serialize we get the same thing.  NOTE: this is
# where xml_canonicalize used to be used, but we no longer need it.
.PHONY: qual-check-srz-regr/%
qual-check-srz-regr/%:
	@echo; echo $@
	rm -rf $*.1.qdir $*.2.qdir
# .c -> .1.qdir
	time $(QUAL) -fq-no-ret-inf -q-config $(CFG) $* -o-srz $*.1.qdir
	@if ! test -s $*.1.qdir/value.xml; then echo "empty file: $*.1.qdir/value.xml"; exit 1; fi
# .1.qdir -> .2.qdir
	time $(QUAL) -fq-no-ret-inf -q-config $(CFG) $*.1.qdir -o-srz $*.2.qdir
# diff 1 2
	diff -u $*.1.qdir/value.xml $*.2.qdir/value.xml
	$(ANNOUNCE_TEST_PASS)

# ELSASRZ := -tr xmlPrintAST,xmlPrintAST-types,xmlPrintAST-lowered
ELSASRZ := -tr xmlPrintAST,xmlPrintAST-types

# Check the canonicality of serialization: that is, test if we
# serialize and de-serialize we get the same thing.  NOTE: this is
# where xml_canonicalize used to be used, but we no longer need it.
.PHONY: elsa-check-srz-regr/%
elsa-check-srz-regr/%:
	@echo; echo $@
	rm -rf $*.1.xml $*.2.xml
# .c -> .1.xml
# try with -tr no-elaborate for another bug
	../elsa/ccparse $(ELSASRZ) $* | ../elsa/chop_out > $*.1.xml
#	../elsa/ccparse -tr parseXml,parseXml-no-danglingPointers,no-typecheck,no-elaborate -tr prettyPrint $*.1.xml > $*.1.pp
	@if ! test -s $*.1.xml; then echo "empty file: $*.1.xml"; exit 1; fi
# .1.xml -> .2.xml
	../elsa/ccparse -tr parseXml,parseXml-no-danglingPointers,no-typecheck,no-elaborate $(ELSASRZ) $*.1.xml | ../elsa/chop_out > $*.2.xml
#	../elsa/ccparse -tr parseXml,parseXml-no-danglingPointers,no-typecheck,no-elaborate -tr prettyPrint $*.1.xml > $*.2.pp
# diff 1 2
	diff -u $*.1.xml $*.2.xml
	$(ANNOUNCE_TEST_PASS)

# sometimes I have to force the language as the suffix is not what I
# expect; code duplication is the easiest way to do that
.PHONY: qual-check-srz-regr-cc/%
qual-check-srz-regr-cc/%:
	@echo; echo $@
	rm -rf $*.1.qdir $*.2.qdir
# .c -> .1.qdir
	$(QUAL) -o-lang GNU_Cplusplus -fq-no-ret-inf -q-config $(CFG) $* -o-srz $*.1.qdir
	@if ! test -s $*.1.qdir/value.xml; then echo "empty file: $*.1.qdir/value.xml"; exit 1; fi
# .1.qdir -> .2.qdir
	$(QUAL) -fq-no-ret-inf -q-config $(CFG) $*.1.qdir -o-srz $*.2.qdir
# diff 1 2
	diff -u $*.1.qdir/value.xml $*.2.qdir/value.xml
	$(ANNOUNCE_TEST_PASS)

# test de-serializing two translation units without borking
FA := Test/taint2_a.c
FB := Test/taint2_b.c

# just check that we can serialize two translation units without borking
.PHONY: qual-check-srz-two-units
qual-check-srz-two-units:
	@echo; echo $@
	rm -rf $(FA).a.qdir $(FB).b.qdir $(FA).ab.qdir
# FA.c -> .a.qdir
	$(QUAL) -q-config $(CFG) $(FA) -o-srz $(FA).a.qdir
# FB.c -> .b.qdir
	$(QUAL) -q-config $(CFG) $(FB) -o-srz $(FB).b.qdir
# .a.qdir & .b.qdir -> .ab.qdir
	$(QUAL) -fq-no-ret-inf -q-config $(CFG) $(FA).a.qdir $(FB).b.qdir -o-srz $(FA).ab.qdir

# check that if we have a single "bad" file that we can send it out
# and read it back and find the error
F1 := Test/taintA.c
.PHONY: qual-check-srz-single
qual-check-srz-single:
	@echo; echo $@
	@echo; echo check it works the old way
	$(QUAL) -q-config $(CFG) $(F1); test $$? -eq 32
#
	@echo; echo check we can serialize out
	rm -rf $(F1).qdir
	$(QUAL) -q-config $(CFG) $(F1) -fq-no-ret-inf -o-srz $(F1).qdir
#
# NOTE: this currently fails with compaction on
	@echo; echo check that when we read it back in it works
	$(QUAL) -q-config $(CFG) $(F1).qdir; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

# check that if we have a single "bad" file that we can send it out
# and read it back and find the error
F2A := Test/taint2_a.c
F2B := Test/taint2_b.c
.PHONY: qual-check-srz-double
qual-check-srz-double:
	@echo; echo $@
	@echo check it works the old way
	$(QUAL) -q-config $(CFG) $(F2A) $(F2B); test $$? -eq 32
#
	@echo; echo check we can serialize out
	rm -f *.ps *.dot
	rm -rf $(F2A).qdir $(F2B).qdir
	$(QUAL) -fq-poly -q-config $(CFG) $(F2A) -fq-no-ret-inf -fq-name-with-loc -fq-name-with-serialno -fq-ugly -o-srz $(F2A).qdir
	$(QUAL) -fq-poly -q-config $(CFG) $(F2B) -fq-no-ret-inf -fq-name-with-loc -fq-name-with-serialno -fq-ugly -o-srz $(F2B).qdir
#
	@echo; echo check that when we read it back in it works
	$(QUAL) -fq-poly -q-config $(CFG) $(F2A).qdir $(F2B).qdir -fq-print-quals-graph; test $$? -eq 32
	$(QUAL) -fq-poly -q-config $(CFG) $(F2A).qdir $(F2B).qdir -fq-print-quals-graph -o-srz $(F2A).2.qdir ; test $$? -eq 32
#
# 	./qual_graph_component -root '$$tainted'   < quals.dot > quals-tainted.dot
# 	dot -Tps -o quals-tainted.cc.ps   quals-tainted.dot
# 	./qual_graph_component -root '$$untainted' < quals.dot > quals-untainted.dot
# 	dot -Tps -o quals-untainted.cc.ps quals-untainted.dot
	@echo; echo check that we can mix serialization and immediate compiling
	$(QUAL) -fq-poly -q-config $(CFG) $(F2A) $(F2B).qdir; test $$? -eq 32
	$(QUAL) -fq-poly -q-config $(CFG) $(F2A).qdir $(F2B); test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

UNSAT_SYM :=
UNSAT_SYM += Test/unsat_symbol1.c
UNSAT_SYM += Test/unsat_symbol2.c

.PHONY: qual-check-unsat-symbol/%
TEST_TOCLEAN += Test/*.filter-good.c
TEST_TOCLEAN += Test/*.filter-bad.c
TEST_TOCLEAN += Test/*.filter-good.cc
TEST_TOCLEAN += Test/*.filter-bad.cc
$(addprefix qual-check-unsat-symbol/,$(UNSAT_SYM)): qual-check-unsat-symbol/%:
	./test_filter -good <$* >$*.filter-good.c
	$(QUAL) -q-config $(CFG) $*.filter-good.c -o-srz $*.filter-good.qdir
	$(QUAL) -q-config $(CFG) -fo-report-link-errors $*.filter-good.qdir
#
	./test_filter -bad <$* >$*.filter-bad.c
	$(QUAL) -q-config $(CFG) $*.filter-bad.c -o-srz $*.filter-bad.qdir
	$(QUAL) -q-config $(CFG) -fo-report-link-errors $*.filter-bad.qdir 2>&1 | grep 'Un-satisfied symbol .*foo'
	$(ANNOUNCE_TEST_PASS)

.PHONY: qual-check-unsat-symbol
qual-check-unsat-symbol: $(addprefix qual-check-unsat-symbol/,$(UNSAT_SYM))
# generate some files
	$(QUAL) -q-config $(CFG) Test/completely_empty.c -o-srz Test/completely_empty.qdir
	$(QUAL) -q-config $(CFG) Test/unsat_symbol3a.c -o-srz Test/unsat_symbol3a.qdir
	$(QUAL) -q-config $(CFG) Test/unsat_symbol3b.c -o-srz Test/unsat_symbol3b.qdir
	$(QUAL) -q-config $(CFG) Test/unsat_symbol3c.c -o-srz Test/unsat_symbol3c.qdir
	$(QUAL) -q-config $(CFG) Test/unsat_symbol4.cc -o-srz Test/unsat_symbol4.qdir
	$(QUAL) -q-config $(CFG) Test/unsat_symbol5a.cc -o-srz Test/unsat_symbol5a.qdir
	$(QUAL) -q-config $(CFG) Test/unsat_symbol5b.cc -o-srz Test/unsat_symbol5b.qdir
	$(QUAL) -q-config $(CFG) Test/unsat_symbol5c.cc -o-srz Test/unsat_symbol5c.qdir
# check we notice main() is missing and that we can deal with an empty input
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/completely_empty.qdir 2>&1 | grep 'Un-satisfied symbol: no definition for .main'
# check that C++ symbols are reported as fully qualified
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol4.qdir 2>&1 | grep 'Un-satisfied symbol .*A::foo'
# check we work when a symbol is indeed satisfied
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol3a.qdir Test/unsat_symbol3b.qdir
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol3b.qdir Test/unsat_symbol3a.qdir
# check we find unsatisfied symbols
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol3a.qdir 2>&1 | grep 'Un-satisfied symbol .*foo'
# check we find oversatisifed symbols
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol3b.qdir Test/unsat_symbol3c.qdir 2>&1 | grep 'Over-satisfied symbol .*foo'
	$(ANNOUNCE_TEST_PASS)

# check that control files work
.PHONY: qual-check-control-files
qual-check-control-files: qual-check-control-files1 qual-check-control-files2

qual-check-control-files1:
# $(QUAL) -q-config $(CFG) Test/unsat_symbol3a.c -o-srz Test/unsat_symbol3a.qdir
# $(QUAL) -q-config $(CFG) Test/unsat_symbol3b.c -o-srz Test/unsat_symbol3b.qdir
# $(QUAL) -q-config $(CFG) Test/unsat_symbol3c.c -o-srz Test/unsat_symbol3c.qdir
# $(QUAL) -q-config $(CFG) Test/unsat_symbol4.cc -o-srz Test/unsat_symbol4.qdir
# $(QUAL) -q-config $(CFG) Test/unsat_symbol5a.cc -o-srz Test/unsat_symbol5a.qdir
# $(QUAL) -q-config $(CFG) Test/unsat_symbol5b.cc -o-srz Test/unsat_symbol5b.qdir
# $(QUAL) -q-config $(CFG) Test/unsat_symbol5c.cc -o-srz Test/unsat_symbol5c.qdir
#
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol3b.qdir Test/unsat_symbol3c.qdir -o-control Test/unsat_symbol3c.ctl 2>&1 | grep 'Over-satisfied symbol .*foo'; test $$? -eq 1
# check that filename "*" works in a control file
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol3b.qdir Test/unsat_symbol3c.qdir -o-control Test/unsat_symbol3c2.ctl 2>&1 | grep 'Un-satisfied symbol .*foo'
# check we find unused controls
	$(QUAL) -q-config $(CFG) -fo-report-link-errors -fo-report-unused-controls Test/unsat_symbol3b.qdir Test/unsat_symbol3c.qdir -o-control Test/unsat_symbol3c2.ctl ; test $$? -eq 8
	$(QUAL) -q-config $(CFG) -fo-report-link-errors -fo-report-unused-controls Test/unsat_symbol3b.qdir Test/unsat_symbol3c.qdir -o-control Test/unsat_symbol3c2bar.ctl; test $$? -eq 8
# check that visibility static works in a control file
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol3b.qdir Test/unsat_symbol3c.qdir -o-control Test/unsat_symbol3c3.ctl 2>&1 | grep 'Over-satisfied symbol .*foo'
	$(QUAL) -q-config $(CFG) Test/unsat_symbol3d.c -o-srz Test/unsat_symbol3d.qdir -o-control Test/unsat_symbol3c3.ctl 2>&1 | grep 'ignoring function body since in ignore file: foo'
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol3d.qdir -o-control Test/unsat_symbol3c3.ctl 2>&1 | grep 'Un-satisfied symbol .*;F:foo'; test $$? -eq 0
#
# check control files on C++
	@echo; echo 'check control files on C++'
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol5b.qdir ; test $$? -eq 8
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol5b.qdir Test/unsat_symbol5c.qdir 2>&1 | grep 'Over-satisfied symbol .*A::foo'
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol5b.qdir Test/unsat_symbol5c.qdir -o-control Test/unsat_symbol5c.ctl 2>&1 | grep 'Over-satisfied symbol .*A::foo'; test $$? -eq 1
	$(ANNOUNCE_TEST_PASS)

# check weak definitions
qual-check-control-files2:
	$(QUAL) -q-config $(CFG) -fo-report-link-errors Test/unsat_symbol3a.c Test/unsat_symbol3b.c Test/unsat_symbol3c.c -o-control Test/unsat_symbol3c4.ctl
	$(ANNOUNCE_TEST_PASS)

qual-check-srz: qual-check-srz-auto-union1
qual-check-srz-auto-union1:
	$(QUAL) -q-config $(CFG) Test/void_auto_union.c ; test $$? -eq 32
	$(QUAL) -q-config $(CFG) Test/void_auto_union.c -o-srz Test/void_quto_union.qdir ; test $$? -eq 32
	$(ANNOUNCE_TEST_PASS)

qual-check-srz: qual-check-srz-multi-file-enum
qual-check-srz-multi-file-enum:
	$(QUALCFG) Test/multi_enum1a.cc  -o-srz Test/multi_enum1a.qdir
	$(QUALCFG) Test/multi_enum1b.cc  -o-srz Test/multi_enum1b.qdir
	$(QUALCFG) Test/multi_enum1a.qdir Test/multi_enum1b.qdir -o-srz Test/multi_enum1x.qdir
	$(QUALCFG) Test/multi_enum1x.qdir
	$(ANNOUNCE_TEST_PASS)


# **************** serialization demo

# simple serialization demo
.PHONY: qual-srz-demo
qual-srz-demo:
	$(QUAL) -q-config $(CFG) Test/taint2_a.c -fq-no-ret-inf -o-srz Test/taint2_a.c.qdir
	$(QUAL) -q-config $(CFG) Test/taint2_b.c -fq-no-ret-inf -o-srz Test/taint2_b.c.qdir
	./qual -q-config $(CFG) Test/taint2_a.c.qdir Test/taint2_b.c.qdir
