# $Id$

CHILDREN := $(shell sed 's/\#.*//' StackRepositories.list)

COMMANDS := all clean check check-full distclean
COMMANDS_BASE := $(addsuffix -rec,$(COMMANDS))

.PHONY: $(COMMANDS)
.PHONY: $(COMMANDS_BASE)

$(COMMANDS): %: %-rec

$(COMMANDS_BASE):
	for child in $(CHILDREN); do echo "**** Making ${@:-rec=} in $$child" && \
	  $(MAKE) -C $$child ${@:-rec=} || exit 1; \
	done
	@echo "================================================================"
	@echo "DONE in all directories: ${@:-rec=}"
	@echo "================================================================"

# other handy things we would like to do across all child repositories

DIFF_REC := $(addsuffix -diff-rec,$(CHILDREN))

.PHONY: diff
diff: $(DIFF_REC)
	svn diff stack-scripts
	svn diff

.PHONY: $(DIFF_REC)
$(DIFF_REC):
	svn diff ${@:-diff-rec=}

STATUS_CHILDREN := $(addprefix STATUS_,$(CHILDREN))
.PHONY: status
status: $(STATUS_CHILDREN)
	svn status -u -q stack-scripts
	svn status -u -N -q

.PHONY: $(STATUS_CHILDREN)
$(STATUS_CHILDREN): STATUS_%:
	svn -q status -u $*
