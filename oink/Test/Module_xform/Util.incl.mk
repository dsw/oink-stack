# **** util

ifndef TOP_LEVEL_MAKEFILE
$(error This makefile should be included in the top-level Makefile, not used stand-alone)
endif

.PHONY: %-splash
%-splash:
	@echo; echo "**** $(@:-splash=)"
