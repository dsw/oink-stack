# Perform source-to-source transformation for localization on C code

xlz-go:

# **** configuration

OINK_STACK := ../../..
OINK := $(OINK_STACK)/oink
ELSA := $(OINK_STACK)/elsa
LOCAL_MALLOC := $(OINK_STACK)/local_malloc

# * module configuration

# the whole xform below is intended to work on one module at a time
ifndef MODULE
MODULE := gronkmod
endif

EXE :=
EXE += gronk.exe

CLFLS :=
CLFLS += gronk.c
CLFLS += gronk_b.c
CLFLS += gronk_b_tor.c

MOD_FILES := $(CLFLS) 
MOD_FILES += gronk.h
MOD_FILES += gronk_b.h

gronk.exe: $(CLFLS:.c=.o)
gronk.xlz_exe: $(CLFLS:.c=.c.xlz.o)

# **** main

# run the whole process from start to finish
.PHONY: xlz-go
xlz-go: xlz-go-clean xlz-go-all xlz-go-test

.PHONY: xlz-go-all
xlz-go-all: all xlz-inst xlz-all

.PHONY: xlz-go-clean
xlz-go-clean: clean xlz-inst-clean xlz-clean

.PHONY: xlz-go-test
xlz-go-test: test xlz-test

# **** libraries

# this variable allows us to check in other makefiles that they are
# not being run stand-alone
TOP_LEVEL_MAKEFILE := something-other-than-empty-string

# utilities
include Util.incl.mk

# standard build
include Std.incl.mk

# xlz build
include Xlz.incl.mk
