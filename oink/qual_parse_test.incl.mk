# see License.txt for copyright and terms of use

# Included makefile for testing just that qual can parse what it
# should.

ifndef QUAL_TEST_MAKEFILE
$(error This makefile should be included in qual_test.incl.mk, not used stand-alone)
endif

.PHONY: qual-check-parse-c
qual-check-parse-c:
# check that the cqual additions assert_type and check_type at least parse
	$(QUAL) -fq-no-inference -o-lang KandR_C -fo-exit-after-parse Test/assert_type1.cc
	$(ANNOUNCE_TEST_PASS)

# check parsing only ****

PARSE_QUAL_TESTS :=
# uses qualifier constants that are not in the lattice file
PARSE_QUAL_TESTS += parse_mess1.cc
# uses qualifier constants that are not in the lattice file
PARSE_QUAL_TESTS += parse_mess2.cc

PARSE_QUAL_TESTS := $(addprefix Test/,$(PARSE_QUAL_TESTS))

.PHONY: qual-check-parse qual-check-parse-start
qual-check-parse-start:; @echo "qual-check-parse ****************"
qual-check-parse: qual-check-parse-start
qual-check-parse: $(addprefix qual-check-parse/,$(PARSE_QUAL_TESTS))
.PHONY: qual-check-parse/%
qual-check-parse/%:
	$(QUAL) -fq-no-inference $*
	$(ANNOUNCE_TEST_PASS)

# check pretty printing runs ****

.PHONY: qual-check-pretty qual-check-pretty-start
qual-check-pretty-start:; @echo "qual-check-pretty ****************"
qual-check-pretty: qual-check-pretty-start
qual-check-pretty: $(addprefix qual-check-pretty/,$(PARSE_QUAL_TESTS))
.PHONY: qual-check-pretty/%
qual-check-pretty/%:
	$(QUAL) -fq-no-inference -fo-pretty-print $* > /dev/null
	$(ANNOUNCE_TEST_PASS)

# check pretty printing by checking it is idempotent ****

PRETTY_IDEM_TESTS :=
PRETTY_IDEM_TESTS += Test/parse_mess1.cc
PRETTY_IDEM_TESTS += Test/parse_mess2.cc
PRETTY_IDEM_TESTS += Test/assert_type1.c
#PRETTY_IDEM_TESTS += Test/ull1.c

TEST_TOCLEAN += Test/*.out1 Test/*.out2
.PHONY: qual-check-pretty-idem qual-check-pretty-idem-start
qual-check-pretty-idem-start:; @echo "qual-check-pretty-idem ****************"
qual-check-pretty-idem: qual-check-pretty-idem-start
qual-check-pretty-idem: $(addprefix qual-check-pretty-idem/,$(PRETTY_IDEM_TESTS))
.PHONY: qual-check-pretty-idem/%
qual-check-pretty-idem/%:
	$(QUAL) -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop $* > $*.out1
	@if ! test -s $*.out1; then echo "File is empty: $*.out1"; exit 1; fi
	$(QUAL) -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop $*.out1 > $*.out2
	diff -u $*.out1 $*.out2
	$(ANNOUNCE_TEST_PASS)

# check pretty printing by diffing with known output ****

PRETTY_DIFF_TESTS :=
# check compile-time sizeof computation
PRETTY_DIFF_TESTS += Test/struct_sizeof.c
PRETTY_DIFF_TESTS += Test/tb2000fail.c
PRETTY_DIFF_TESTS += Test/parse_mess1.cc
PRETTY_DIFF_TESTS += Test/parse_mess2.cc

.PHONY: qual-check-pretty-diff qual-check-pretty-diff-start
qual-check-pretty-diff-start:; @echo "qual-check-pretty-diff ****************"
qual-check-pretty-diff: qual-check-pretty-diff-start
qual-check-pretty-diff: $(addprefix qual-check-pretty-diff/,$(PRETTY_DIFF_TESTS))
.PHONY: qual-check-pretty-diff/%
qual-check-pretty-diff/%:
	$(QUAL) -fo-no-do-elaboration -fq-no-inference -fo-pretty-print -fo-no-print-startstop $* > $*.out
	diff -u $*.correct $*.out
	$(ANNOUNCE_TEST_PASS)

# quarl 2006-05-28
#    Check that re-serializing a serialized-and-de-serialized file works.
# quarl 2006-05-31
#    Check that everything except the qual.dat (and its map) is identical
# quarl 2006-06-03
#    Use TEST_TMP_DIR: much faster!
qual-check-resrz: $(addprefix qual-check-resrz/,$(IN_TESTS))
qual-check-resrz/%: $(TEST_TMP_DIR)
	@echo "# $@"
	$(QUAL) $* -o-srz $(TEST_TMP_DIR)/$(notdir $*.1.qdir) -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph
	$(QUAL) $(TEST_TMP_DIR)/$(notdir $*.1.qdir) -o-srz $(TEST_TMP_DIR)/$(notdir $*.2.qdir) -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph
	diff -u $(TEST_TMP_DIR)/$(notdir $*.1.qdir)/format.txt $(TEST_TMP_DIR)/$(notdir $*.2.qdir)/format.txt
	diff -u $(TEST_TMP_DIR)/$(notdir $*.1.qdir)/files.xml $(TEST_TMP_DIR)/$(notdir $*.2.qdir)/files.xml
	diff -u $(TEST_TMP_DIR)/$(notdir $*.1.qdir)/value.xml $(TEST_TMP_DIR)/$(notdir $*.2.qdir)/value.xml
	rm -r $(TEST_TMP_DIR)/$(notdir $*.1.qdir) $(TEST_TMP_DIR)/$(notdir $*.2.qdir)
	$(ANNOUNCE_TEST_PASS)

oink-check-resrz: $(addprefix oink-check-resrz/,$(IN_TESTS))
oink-check-resrz/%: $(TEST_TMP_DIR)
	@echo "# $@"
	oink $* -o-srz $(TEST_TMP_DIR)/$(notdir $*.1.odir) -tr nohashlines
	oink $(TEST_TMP_DIR)/$(notdir $*.1.odir) -o-srz $(TEST_TMP_DIR)/$(notdir $*.2.odir) -tr nohashlines
	diff -u $(TEST_TMP_DIR)/$(notdir $*.1.odir)/format.txt $(TEST_TMP_DIR)/$(notdir $*.2.odir)/format.txt
	diff -u $(TEST_TMP_DIR)/$(notdir $*.1.odir)/files.xml $(TEST_TMP_DIR)/$(notdir $*.2.odir)/files.xml
	diff -u $(TEST_TMP_DIR)/$(notdir $*.1.odir)/value.xml $(TEST_TMP_DIR)/$(notdir $*.2.odir)/value.xml
	rm -r $(TEST_TMP_DIR)/$(notdir $*.1.odir) $(TEST_TMP_DIR)/$(notdir $*.2.odir)
	$(ANNOUNCE_TEST_PASS)


QUAL_RESRZ_XFAIL := 

# 18cca471-ba67-40dd-865a-c787b1c65ade
# QUAL_RESRZ_XFAIL += t0117.cc t0134.cc t0311.cc t0449.cc t0462.cc

# ed1f952c-dbf5-41bf-9778-d5b0c0bda5af
QUAL_RESRZ_XFAIL += t0561.cc

# # 9cc54383-8e24-4a23-8df2-a802fe9abd03
# QUAL_RESRZ_XFAIL += t0308.cc

XFAIL += $(addprefix qual-check-resrz/$(ELSA_DIR)/in/,$(QUAL_RESRZ_XFAIL))

