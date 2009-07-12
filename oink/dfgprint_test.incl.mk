# see License.txt for copyright and terms of use

# Included makefile for testing.

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

.PHONY: dfgprint-check
dfgprint-check:
	./dfgprint -fd-no-print-dfg -fd-ben-flag
	./dfgprint -fd-print-dfg Test/dfgprint1.c > /dev/null
# check that warnings on inline assembly work
	./dfgprint -fd-no-print-dfg Test/inline_asm1.cc 2>&1 | \
          grep -q 'inline assembly is not analyzed in the dataflow'
	./dfgprint -fd-no-print-dfg Test/inline_asm2.cc 2>&1 | \
          grep -q 'inline assembly is not analyzed in the dataflow'
	./dfgprint -fo-pretty-print Test/hello.cc > /dev/null

# put an input file here to generate its control flow graph as a
# post-script file
CHK_DFGPRINT :=
CHK_DFGPRINT += Test/dfgprint1.c

.PHONY: dfgprint-check-print/%
dfgprint-check-print/%:
	rm -f $*.dfg.dot $*.dfg.dot.ps
	./dfgprint -fd-print-dfg $* > $*.dfg.dot
	dot -Tps -o $*.dfg.dot.ps $*.dfg.dot
.PHONY: dfgprint-check-print
dfgprint-check-print: $(addprefix dfgprint-check-print/,$(CHK_DFGPRINT))
