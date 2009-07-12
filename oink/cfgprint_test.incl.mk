# see License.txt for copyright and terms of use

# Included makefile for testing.

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

.PHONY: cfgprint-check
cfgprint-check:
	./cfgprint -fc-no-print-cfg -fc-ben-flag
	./cfgprint -fc-print-cfg Test/cfgprint1.c > /dev/null
	./cfgprint -fo-pretty-print Test/hello.cc > /dev/null

# put an input file here to generate its control flow graph as a
# post-script file
CHK_CFGPRINT :=
CHK_CFGPRINT += Test/cfgprint1.c

.PHONY: cfgprint-check-print/%
cfgprint-check-print/%:
	rm -f $*.cfg.dot $*.cfg.dot.ps
	./cfgprint -fc-print-cfg $* > $*.cfg.dot
	dot -Tps -o $*.cfg.dot.ps $*.cfg.dot
.PHONY: cfgprint-check-print
cfgprint-check-print: $(addprefix cfgprint-check-print/,$(CHK_CFGPRINT))
