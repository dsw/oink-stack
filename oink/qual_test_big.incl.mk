# see License.txt for copyright and terms of use

# Set OINK_TEST_BIG to use it.
#   TODO: auto config this.

$(error FIX: variable OINK_TEST_BIG is undefined)
OINK_TEST_BIG :=

check-big: qual-check-big

QUALCFGT_TWOPASS = $(QUALCFGT) $(1) -o-srz $(1).1.qdir && $(QUALCFGT) $(1).1.qdir -o-srz $(1).2.qdir

TEST_TOCLEAN_DIR += $(OINK_TEST_BIG)/*.qdir
qual-check-big:
	if [ ! -d '$(OINK_TEST_BIG)' ] ; then echo 'directory missing: $(OINK_TEST_BIG)'; exit 1; fi
	$(call QUALCFGT_TWOPASS,$(OINK_TEST_BIG)/a.ii)
	$(call QUALCFGT_TWOPASS,$(OINK_TEST_BIG)/b.ii)
	$(call QUALCFGT_TWOPASS,$(OINK_TEST_BIG)/c.ii)
	$(call QUALCFGT_TWOPASS,$(OINK_TEST_BIG)/d.ii)
	$(call QUALCFGT_TWOPASS,$(OINK_TEST_BIG)/e.ii)
	$(call QUALCFGT_TWOPASS,$(OINK_TEST_BIG)/f.ii)
	$(call QUALCFGT_TWOPASS,$(OINK_TEST_BIG)/g.ii)
	$(call QUALCFGT_TWOPASS,$(OINK_TEST_BIG)/h.ii)

# TODO: add regression tests for known format string bugs found.
