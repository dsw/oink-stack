# see License.txt for copyright and terms of use

# Included makefile for testing bullet.

LLVMBIN=../../llvm/Release/bin
ASSEMBLER=as
CC=gcc

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

BULLET_TESTS=\
bullet_test_binop \
bullet_test_if \
bullet_test_if_else_2 \
bullet_test_if_else \
bullet_test_trivial \
bullet_test_variables \
bullet_test_while \
bullet_test_while_2 \
bullet_test_while_3 \
bullet_test_do_while \
bullet_test_do_while_2 \
bullet_test_do_while_3 \


.PHONY: bullet-check
bullet-check: $(BULLET_TESTS)

BULLET_CLEAN =
BULLET_CLEAN += $(foreach bullet_test,$(BULLET_TESTS),$(bullet_test).ll)
BULLET_CLEAN += $(foreach bullet_test,$(BULLET_TESTS),$(bullet_test).bc)
BULLET_CLEAN += $(foreach bullet_test,$(BULLET_TESTS),$(bullet_test).s)
BULLET_CLEAN += $(foreach bullet_test,$(BULLET_TESTS),$(bullet_test))
BULLET_CLEAN += $(foreach bullet_test,$(BULLET_TESTS),$(bullet_test).opt.ll)
BULLET_CLEAN += $(foreach bullet_test,$(BULLET_TESTS),$(bullet_test).opt.bc)
BULLET_CLEAN += $(foreach bullet_test,$(BULLET_TESTS),$(bullet_test).opt.s)
BULLET_CLEAN += $(foreach bullet_test,$(BULLET_TESTS),$(bullet_test).opt)

TOCLEAN += $(BULLET_CLEAN)

clean-bullet-check:
	rm -f $(BULLET_CLEAN)

.SECONDEXPANSION:
$(BULLET_TESTS): ./bullet bullet_test.incl.mk Test/bullet/$$@.c
	rm -f $@.ll $@.bc $@.opt.ll $@.opt.bc 
	./bullet Test/bullet/$@.c
	mv out.ll $@.ll
	cat $@.ll
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
