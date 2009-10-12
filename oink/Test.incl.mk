# see License.txt for copyright and terms of use

# A makefile for testing everything.  It is meant to be included from
# Makefile.

ifndef TOP_LEVEL_MAKEFILE
$(error This makefile should be included in the top-level Makefile, not used stand-alone)
endif

# this variable allows us to check in other makefiles that they are
# not being run stand-alone
TEST_MAKEFILE := something-other-than-empty-string

TEST_TMP_DIR := $(TMP_DIR)/test
$(TEST_TMP_DIR): $(TMP_DIR)
	mkdir $(TEST_TMP_DIR)

.PHONY: testtmpdir
testtmpdir: $(TEST_TMP_DIR)

# using '+=', add to TEST_TOCLEAN artifacts of running tests that you
# want to get cleaned by the clean-check: target; Note that this is an
# '=' variable, not a ':=' one
TEST_TOCLEAN =
TEST_TOCLEAN += gmon.out
# use TEST_TOCLEAN_DIR for directories
TEST_TOCLEAN_DIR = $(TEST_TMP_DIR)
.PHONY: clean-check
clean-check:
	rm -f $(TEST_TOCLEAN)
	rm -rf $(TEST_TOCLEAN_DIR)

# utilities
ANNOUNCE_TEST_PASS = @echo "\# **** PASS: $@"
ANNOUNCE_TEST_PASS_MAJOR = @echo; echo; echo "\# ******** PASS: $@"; echo; echo

# **** profiling

SPACE_IN := $(ELSA_DIR)/in/big/nsUnicodeToTeXCMRt1.i
# SPACE_IN := $(ELSA_DIR)/in/k0010.cc

.PHONY: space-prof
space-prof:
# 	valgrind --tool=memcheck --show-reachable --num-callers=8 ./program -flagforforeground
	time valgrind --tool=memcheck -v --leak-check=full --show-reachable=yes --num-callers=8 --log-file=vprof.log \
          ./qual -q-config ../libqual/config/lattice -o-lang GNU_Cplusplus $(SPACE_IN)
# don't modify the logs afterward
	chmod a-w vprof.log.*

# **** testing

.PHONY: check
check: clean-check check-small check-xfail
	$(ANNOUNCE_TEST_PASS_MAJOR)

# quarl 2006-05-26
#   'check-full' is a superset of 'check'
.PHONY: check-full
check-full: clean-check check-small check-big check-xfail
	$(ANNOUNCE_TEST_PASS_MAJOR)

# quarl 2006-05-26:
#   check-small is for "small" tests (generally test a single feature)
.PHONY: check-small

check-small: lib-check
check-small: oink-check
check-small: cpdinit-check
check-small: staticprint-check
check-small: dfgprint-check
check-small: cfgprint-check
# FIX: turn this on when bullet starts working
# check-small: bullet-check
check-small: qual-check

# quarl 2006-05-26: check-big is for big tests
.PHONY: check-big
check-big:

# quarl 2006-05-26: check that known failures do fail.
.PHONY: check-xfail
check-xfail:
	@echo
	@echo "# ------------------------------------------------------------"
	@echo "# Checking expected failures (known bugs)..."
	@for x in $(XFAIL); do echo; echo "make $$x"; if make --no-print-directory $$x; then \
          echo -e "\n\nExpected 'make $$x' to fail, but it passed.\n"; exit 1; \
        fi; echo "# Failed as expected: $$x"; done
	@echo
	@echo "# All $(words $(XFAIL)) known failures still fail (list with 'make show-xfail')."
	@echo

.PHONY: show-xfail
show-xfail:
	@echo $(XFAIL) | xargs -n 1

XFAIL :=

# include other testing files
include Lib/LibTest.incl.mk
include oink_test.incl.mk
include cpdinit_test.incl.mk
include staticprint_test.incl.mk
include dfgprint_test.incl.mk
include cfgprint_test.incl.mk
include bullet_test.incl.mk
include qual_test.incl.mk
