# see License.txt for copyright and terms of use

# Included makefile for testing alloctool.

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

.PHONY: alloctool-check
alloctool-check: alloctool-check-basic
alloctool-check: alloctool-check-heapify
alloctool-check: alloctool-check-verify

.PHONY: alloctool-check-basic
alloctool-check-basic:
# check that it can parse C/C++ that has qualifiers in it
	./alloctool -fa-print-stack-alloc-addr-taken \
           Test/stack_alloc_parse_qual.cc \
           > /dev/null
# check -fa-print-stack-alloc
	./alloctool -fa-print-stack-alloc Test/stack_alloc1.cc \
           > Test/stack_alloc1.cc.out
	diff Test/stack_alloc1.cc.cor Test/stack_alloc1.cc.out
# check -fa-print-stack-alloc-addr-taken
	./alloctool -fa-print-stack-alloc-addr-taken Test/stack_alloc2.cc \
           > Test/stack_alloc2.cc.out
	diff Test/stack_alloc2.cc.cor Test/stack_alloc2.cc.out
# check -fa-heapify-stack-alloc-addr-taken rejects C++
	./alloctool -fa-heapify-stack-alloc-addr-taken Test/stack_alloc2.cc \
           2>&1 | grep "Can't heapify C++ with alloctool yet." > /dev/null

# check -fa-heapify-stack-alloc-addr-taken
.PHONY: alloctool-check-heapify
alloctool-check-heapify:
# check handles declarators
	./alloctool -fa-heapify-stack-alloc-addr-taken Test/heapify1.c \
           > Test/heapify1.c.patch.out
	diff Test/heapify1.c.patch.cor Test/heapify1.c.patch.out
# check handles return
	./alloctool -fa-heapify-stack-alloc-addr-taken \
           -a-free-func free2 -a-xmalloc-func xmalloc2 \
           Test/heapify2.c \
           > Test/heapify2.c.patch.out
	diff Test/heapify2.c.patch.cor Test/heapify2.c.patch.out

# check -fa-verify-cross-module-params
.PHONY: alloctool-check-verify
alloctool-check-verify: Test/verify1_foo.i Test/verify1_bar.i
	./alloctool -fa-verify-cross-module-params $^ \
	  -a-verify-func "verify2" \
	  -o-mod-spec bar:Test/verify1_bar.mod \
	  -o-mod-spec foo:Test/verify1_foo.mod \
	  -o-mod-default default \
	  > Test/verify1.c.patch.out
	diff Test/verify1.c.patch.cor Test/verify1.c.patch.out

Test/verify1_foo.i Test/verify1_bar.i: Test/verify1_%.i: Test/verify1_%.c
	gcc -Wall -E -o $@ $<
