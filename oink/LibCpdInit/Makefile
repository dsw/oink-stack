# see License.txt for copyright and terms of use

# For testing purposes, build a vacuous LibCpdInit against its own
# cpdinit_lib.h which specifies what the compiler writer would have to
# provide.

VERSION := 2003.5.27

.SUFFIXES:

LOCAL_CC_O :=
LOCAL_CC_O += cpdinit.o
LOCAL_CC_O += member_type_iter.o

.PHONY: all
all: $(LOCAL_CC_O)
# turn this on if you want etags built for you by default; FIX: make
# this a configure option
#all: TAGS

# using '+=', add to TOCLEAN things you want to get cleaned by the
# clean: target; Note that this is an '=' variable, not a ':=' one
TOCLEAN =
TOCLEAN += Test/*.cpdout_c Test/*.cilout_c
.PHONY: clean
clean:; rm -f $(TOCLEAN)

.PHONY: check
check:; @echo "The checks are done by a different makefile in a different repository."

CCFLAGS :=
CCFLAGS += -g -Wall
# don't turn this on by default as it makes the code too unportable
# CFLAGS += -Werror

CCFLAGS += -D__LINUX__ -D__UNIX__
CCFLAGS += -Wcast-align
# cc1plus: warning: command line option "-Wmissing-prototypes" is valid for C/ObjC but not for C++
#CCFLAGS += -Wmissing-prototypes
# cc1plus: warning: command line option "-Wmissing-declarations" is valid for C/ObjC but not for C++
# CCFLAGS += -Wmissing-declarations
# cc1plus: warning: command line option "-Wnested-externs" is valid for C/ObjC but not for C++
# CCFLAGS += -Wnested-externs
CCFLAGS += -Winline
CCFLAGS += -Wno-char-subscripts -Wno-deprecated
#CCFLAGS += -Wshadow
CCFLAGS += -Wpointer-arith
CCFLAGS += -Wredundant-decls
CCFLAGS += -Wconversion
CCFLAGS += -Wconversion
CCFLAGS += -Wpointer-arith
CCFLAGS += -Woverloaded-virtual

# You need to replace cpdinit_lib.h with a header file that
# provides the declarations for your classes.  Note: the outer quotes
# are eaten by the shell, the inner ones by the preprocessor, and make
# doesn't eat anything.
CCFLAGS += -DCPDINIT_LIB_HEADERS='"cpdinit_lib.h"'

TOCLEAN += $(LOCAL_CC_O)
$(LOCAL_CC_O): %.o: %.cc
	g++ -c -o $@ $(CCFLAGS) $<

TOCLEAN += TAGS
TAGS:; etags --members --typedefs *.cc *.h
