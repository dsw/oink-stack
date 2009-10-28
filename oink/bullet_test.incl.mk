# see License.txt for copyright and terms of use

# Included makefile for testing bullet.

LLVMBIN=../../llvm/Release/bin
ASSEMBLER=as
CC=gcc

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

.PHONY: bullet-check
bullet-check: startup.o hello_1 hello_2 hello_variables

.SECONDEXPANSION:
hello_1 hello_2 hello_variables: ./bullet bullet_test.incl.mk Test/bullet/$$@.c
	rm -f $@.ll $@.bc $@.opt.ll $@.opt.bc 
	./bullet Test/bullet/$@.c
	mv out.ll $@.ll
	$(LLVMBIN)/llvm-as $@.ll -f -o $@.bc
	$(LLVMBIN)/llc $@.bc -f -o $@.s
	$(ASSEMBLER) $@.s -o $@.o
	$(CC) $@.o -o $@
	./$@
	$(LLVMBIN)/opt -std-compile-opts $@.bc -f -o $@.opt.bc
	$(LLVMBIN)/llvm-dis $@.opt.bc -f -o $@.opt.ll
	$(LLVMBIN)/llc $@.opt.bc -f -o $@.opt.s
	$(ASSEMBLER) $@.opt.s -o $@.opt.o
	$(CC) $@.opt.o -o $@.opt
	./$@.opt
