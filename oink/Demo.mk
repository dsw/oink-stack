# see License.txt for copyright and terms of use

# a makefile for giving a demo of oink

all:
	@echo legal targets are:; echo print-dfg print-cfg taint horrible lower lower-xml

# FIX: broken
# print inheritance graph
.PHONY: print-ihg
print-ihg:
	$(MAKE) staticprint-check-print/Test/staticprint1.cc
	cat Test/staticprint1.cc
	gv Test/staticprint1.cc.ihg.dot.ps

# FIX: broken
# print dataflow graph
.PHONY: print-dfg
print-dfg:
	$(MAKE) dfgprint-check-print/Test/dfgprint1.c
	cat Test/dfgprint1.c
	gv Test/dfgprint1.c.dfg.dot.ps

# print controlflow graph (provided by Elsa)
.PHONY: print-cfg
print-cfg:
	$(MAKE) cfgprint-check-print/Test/cfgprint1.c
	cat Test/cfgprint1.c
	gv Test/cfgprint1.c.cfg.dot.ps

# standard cqual C demo
.PHONY: taint
taint:
	cat Test/taint1.c
	./qual -q-config ../libqual/config/lattice Test/taint1.c

# horrible C++ demo
.PHONY: horrible
horrible:
	cat Test/horrible1.cc
	./qual -q-config ../libqual/config/lattice Test/horrible1.cc

# make explicit syntax and pretty-print back out
.PHONY: lower
lower:
	./qual -fq-no-inference -fo-pretty-print Test/horrible1.cc
# Note the implicit call to A's ctor in B's ctor, the implicit
# generation of ctors and operator assigns and dtors, the template
# instantiation of A.

# Subtlties: the dtor of A is called in the dtor of B, but you can't
# write that.  Here it is in XML-like format; search for ~A.
# .PHONY: lower2
# lower2:
# 	./qual -fq-no-inference -fo-print-elaborated-ast Test/horrible1.cc

.PHONY: lower-xml
lower-xml:
	../elsa/ccparse -tr xmlPrintAST -tr xmlPrintAST-indent -tr xmlPrintAST-types -tr xmlPrintAST-lowered Test/horrible1_noqual.cc

# demo -fq-print-trans-qual
trans-qual:
	./qual -fq-no-explain-errors -fo-pretty-print -fq-print-trans-qual -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint0.c
