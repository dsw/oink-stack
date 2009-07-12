# $Id$

TOPLEVEL = ../..
TARGET_BASE = ../libc

C_FILES_EXTRA = builtin-definitions.c

include $(TOPLEVEL)/Makefile.src.incl

all: all-build

builtin-definitions.c builtin-declarations.h: $(C_FILES1) ../tools/builtin-maker
	mv builtin-declarations.h builtin-declarations.h.old
	../tools/builtin-maker Manifest builtin-t.h builtin-definitions.c builtin-declarations.h
	@if ! diff builtin-declarations.h builtin-declarations.h.old >/dev/null; then             \
	  echo "*** builtin-declarations.h has changed.  Please update the version in Elsa." >&2; \
	fi
	rm -f builtin-declarations.h.old
