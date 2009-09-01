# see License.txt for copyright and terms of use

# Included makefile for testing bullet.

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

.PHONY: bullet-check
bullet-check:
	./bullet
	./bullet Test/helloA.c

# FIX:
#	./bullet Test/horrible1.cc

#	./bullet -fs-no-print-ihg -fs-ben-flag
#	./bullet -fs-print-ihg Test/bullet1.cc > /dev/null
#	./bullet -fs-print-ast-histogram Test/bullet1.cc > /dev/null
#	./bullet -fo-pretty-print Test/hello.cc > /dev/null

# put an input file here to generate its control flow graph as a
# post-script file
CHK_BULLET :=
# CHK_BULLET += Test/bullet1.cc

.PHONY: bullet-check-print/%
bullet-check-print/%:
	rm -f $*.ihg.dot $*.ihg.dot.ps
	./bullet -fs-print-ihg $* > $*.ihg.dot
	dot -Tps -o $*.ihg.dot.ps $*.ihg.dot
.PHONY: bullet-check-print
# bullet-check-print: $(addprefix bullet-check-print/,$(CHK_BULLET))
