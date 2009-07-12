# see License.txt for copyright and terms of use

# Included makefile for testing staticprint.

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

.PHONY: staticprint-check
staticprint-check:
	./staticprint -fs-no-print-ihg -fs-ben-flag
	./staticprint -fs-print-ihg Test/staticprint1.cc > /dev/null
	./staticprint -fs-print-ast-histogram Test/staticprint1.cc > /dev/null
	./staticprint -fo-pretty-print Test/hello.cc > /dev/null

# put an input file here to generate its control flow graph as a
# post-script file
CHK_STATICPRINT :=
CHK_STATICPRINT += Test/staticprint1.cc

.PHONY: staticprint-check-print/%
staticprint-check-print/%:
	rm -f $*.ihg.dot $*.ihg.dot.ps
	./staticprint -fs-print-ihg $* > $*.ihg.dot
	dot -Tps -o $*.ihg.dot.ps $*.ihg.dot
.PHONY: staticprint-check-print
staticprint-check-print: $(addprefix staticprint-check-print/,$(CHK_STATICPRINT))
