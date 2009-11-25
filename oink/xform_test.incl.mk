# see License.txt for copyright and terms of use

# Included makefile for testing xform.

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

.PHONY: xform-check
xform-check: xform-check-basic
xform-check: xform-check-heapify
xform-check: xform-check-heapify3
xform-check: xform-check-verify
xform-check: xform-check-localize

.PHONY: xform-check-basic
xform-check-basic:
	@echo; echo $@
# check that it can parse C/C++ that has qualifiers in it
	./xform -fx-print-stack-alloc-addr-taken \
           Test/stack_alloc_parse_qual.cc \
           > /dev/null
# check -fx-print-stack-alloc
	./xform -fx-print-stack-alloc Test/stack_alloc1.cc \
           > Test/stack_alloc1.cc.out
	diff Test/stack_alloc1.cc.cor Test/stack_alloc1.cc.out
# check -fx-print-stack-alloc-addr-taken
	./xform -fx-print-stack-alloc-addr-taken Test/stack_alloc2.cc \
           > Test/stack_alloc2.cc.out
	diff Test/stack_alloc2.cc.cor Test/stack_alloc2.cc.out
# check -fx-heapify-stack-alloc-addr-taken rejects C++
	./xform -fx-heapify-stack-alloc-addr-taken Test/stack_alloc2.cc \
           2>&1 | grep "Can't heapify C++ with xform yet." > /dev/null

# check -fx-heapify-stack-alloc-addr-taken
.PHONY: xform-check-heapify
xform-check-heapify:
	@echo; echo $@
# check handles declarators
	./xform -fx-heapify-stack-alloc-addr-taken \
           Test/heapify1.c \
           -o-mod-spec foo:Test/heapify1.c -o-mod-default default \
           > Test/heapify1.c.patch.out
	diff Test/heapify1.c.patch.cor Test/heapify1.c.patch.out
# check handles return
	./xform -fx-heapify-stack-alloc-addr-taken \
           -x-free-func free2 -x-xmalloc-func xmalloc2 \
           Test/heapify2.c \
           -o-mod-spec foo:Test/heapify2.c -o-mod-default default \
           > Test/heapify2.c.patch.out
	diff Test/heapify2.c.patch.cor Test/heapify2.c.patch.out

.PHONY: xform-check-heapify3
xform-check-heapify3: Test/heapify3.i
	@echo; echo $@
# check handles Derrick's concerns
	./xform -fx-heapify-stack-alloc-addr-taken $< \
           -o-mod-spec foo:$(<:.i=.c) -o-mod-default default \
            > Test/heapify3.c.patch.out
	diff Test/heapify3.c.patch.cor Test/heapify3.c.patch.out

# check -fx-verify-cross-module-params
.PHONY: xform-check-verify
xform-check-verify: Test/verify1_foo.i Test/verify1_bar.i
	@echo; echo $@
	./xform -fx-verify-cross-module-params $^ \
	  -x-verify-func "verify2" \
	  -o-mod-spec bar@Test/verify1_bar.mod \
	  -o-mod-spec foo@Test/verify1_foo.mod \
	  -o-mod-default default \
	  > Test/verify1.c.patch.out
	diff Test/verify1.c.patch.cor Test/verify1.c.patch.out

# check -fx-localize-heap-alloc
.PHONY: xform-check-localize
xform-check-localize: Test/verify1_foo.i Test/verify1_bar.i
	@echo; echo $@
	./xform -fx-localize-heap-alloc $^ \
	  -o-mod-spec bar@Test/verify1_bar.mod \
	  -o-mod-spec foo@Test/verify1_foo.mod \
	  -o-mod-default default \
	  > Test/localize1.c.patch.out
	diff Test/localize1.c.patch.cor Test/localize1.c.patch.out

# **** preprocessing

TO_CPP :=
TO_CPP += Test/verify1_foo.c
TO_CPP += Test/verify1_bar.c
TO_CPP += Test/heapify3.c

TO_CPP_I := $(TO_CPP:.c=.i)

$(TO_CPP_I): Test/%.i: Test/%.c
	gcc -Wall -E -o $@ $<
