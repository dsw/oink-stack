# see License.txt for copyright and terms of use

# Included makefile for testing alloctool.

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

.PHONY: alloctool-check
alloctool-check:
	./alloctool -fa-print-stack-alloc-addr-taken Test/stack_alloc1.cc > Test/stack_alloc1.cc.out
	diff Test/stack_alloc1.cc.cor Test/stack_alloc1.cc.out

# put an input file here to generate its control flow graph as a
# post-script file
CHK_ALLOCTOOL :=
# CHK_ALLOCTOOL += Test/alloctool1.cc

# .PHONY: alloctool-check-print/%
# alloctool-check-print/%:
# 	rm -f $*.ihg.dot $*.ihg.dot.ps
# 	./alloctool -fs-print-ihg $* > $*.ihg.dot
# 	dot -Tps -o $*.ihg.dot.ps $*.ihg.dot
# .PHONY: alloctool-check-print
# alloctool-check-print: $(addprefix alloctool-check-print/,$(CHK_ALLOCTOOL))
