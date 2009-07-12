# $Id: Makefile 3161 2006-06-13 04:26:03Z quarl $

TOPLEVEL = ../..
TARGET_BASE = ../libstdc++

include $(TOPLEVEL)/Makefile.src.incl

all: all-build

# quarl 2006-06-14
#    To check linking the C++ standard library we need the C library.
oz-linkcheck: ../../libc_glibc-2.3.5/libc.oz
qz-linkcheck: ../../libc_glibc-2.3.5/libc.qz
