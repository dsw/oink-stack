# -*-makefile-*-
# Analyze C/C++ files for Hard Object

all:
.SUFFIXES:

# this variable allows us to check in other makefiles that they are
# not being run stand-alone
TOP_LEVEL_MAKEFILE := something-other-than-empty-string


# **** configuration

OINK_STACK := ../../..


# **** parameterization

EXE :=
EXE += hello2.exe

ANALYSIS := trust

# The MODS .mod files are just files full of filenames: every file is
# a module and every filename in the corresponding .mod file is the
# list of files in that module.
MODS :=
MODS += hello2
MODS += bar

# For now for simplicity we assume that each module is in a file
# called module.mod; however this need not be the case in general.
MOD_SPECS := $(foreach MOD,$(MODS),$(MOD):$(MOD).mod)

hello2.exe: lib_bar.o

# this currently fails as the header files have variables that we
# don't give a module for
analyze/hello2.exe: hello2.i lib_bar.i

analyze-func-iface/hello2.exe: hello2.i lib_bar.i


# **** include generic functionality

include ModuleAnalysis.incl.mk
