# see License.txt for copyright and terms of use

# Included makefile for testing LibCpdInit.

ifndef TEST_MAKEFILE
$(error This makefile should be included in Test.incl.mk, not used stand-alone)
endif

.PHONY: cpdinit-check
cpdinit-check: cpd-regr
	./oink Test/compoundinit_ptrToMem.cc

CIL_DIR := ../cil

CILLY := $(CIL_DIR)/obj/x86_LINUX/cilly.asm.exe

CPD_INIT_TEST :=

CPD_INIT_TEST += compoundinit1A.c
CPD_INIT_TEST += compoundinit1B.c
CPD_INIT_TEST += compoundinit1C.c
CPD_INIT_TEST += compoundinit1D.c
CPD_INIT_TEST += compoundinit1E.c
CPD_INIT_TEST += compoundinit1F.c
CPD_INIT_TEST += compoundinit1G.c
CPD_INIT_TEST += compoundinit1H.c
CPD_INIT_TEST += compoundinit1I.c
CPD_INIT_TEST += compoundinit2.c
CPD_INIT_TEST += compoundinit2_1.c
CPD_INIT_TEST += compoundinit3A.c
CPD_INIT_TEST += compoundinit3B.c
CPD_INIT_TEST += compoundinit3C.c
CPD_INIT_TEST += compoundinit3D.c
CPD_INIT_TEST += compoundinit4A.c
CPD_INIT_TEST += compoundinit4B.c
CPD_INIT_TEST += compoundinit5A.c

# FIX: won't parse wchar_t in C mode, which is the only mode in which
# we do wchar_t; Ah, but maybe if I put it in C99 mode it will work
#CPD_INIT_TEST += compoundinit5Aw.c
#CPD_INIT_TEST += compoundinit5Bw.c

# FIX: this doesn't work because I now assume that character arrays
# are points, not containers.  I need some sort of state attached to
# an array to say if it has been assigned to with a char and therefore
# it is a container, or with a string, and therefore it is a point.
# It is probably sufficient to always treat it as a container but if
# the first element is a string, pop it off the stack of containers.
#CPD_INIT_TEST += compoundinit5B.c

CPD_INIT_TEST += compoundinit6A.c
CPD_INIT_TEST += compoundinit6B.c
# E_compoundLit
CPD_INIT_TEST += compoundinit7_e_cpdlit.c
# Initializing char array members using a string
CPD_INIT_TEST += compoundinit8_chararraymem.c
CPD_INIT_TEST += compoundinit9_chararr.c

# FAILS
# CPD_INIT_TEST += compoundinit10.c

CPD_INIT_TEST += cil_const-array-init.c
# CPD_INIT_TEST += cil_extern_init.c
CPD_INIT_TEST += cil_init.c
CPD_INIT_TEST += cil_init1.c
#CPD_INIT_TEST += cil_init2.c
CPD_INIT_TEST += cil_init3.c
CPD_INIT_TEST += cil_init4.c
#CPD_INIT_TEST += cil_init5.c
#CPD_INIT_TEST += cil_init6.c
#CPD_INIT_TEST += cil_init7.c
CPD_INIT_TEST += cil_init8.c
#CPD_INIT_TEST += cil_init9.c
CPD_INIT_TEST += cil_init10.c
CPD_INIT_TEST += cil_init11.c
#CPD_INIT_TEST += cil_init12.c
#CPD_INIT_TEST += cil_init13.c
CPD_INIT_TEST += cil_init14.c
CPD_INIT_TEST += cil_init15.c
#CPD_INIT_TEST += cil_init16.c
CPD_INIT_TEST += cil_initial.c
CPD_INIT_TEST += cil_struct_init.c


# CIL_INIT_TEST :=

# CIL_INIT_TEST += compoundinit1A.c
# CIL_INIT_TEST += compoundinit1B.c
# CIL_INIT_TEST += compoundinit1C.c
# CIL_INIT_TEST += compoundinit1D.c
# CIL_INIT_TEST += compoundinit1E.c
# CIL_INIT_TEST += compoundinit1F.c
# CIL_INIT_TEST += compoundinit1G.c

# # behavior chosen in an unspecified instance by CIL: C99 6.7.8
# # paragraph 10: "If an object that has automatic storage duration is
# # not initialized explicitly, its value is indeterminate.  If an
# # object that has static storage duration is not initialized
# # explicitly, then: ... if it has arithmetic type, it is initialized
# # to ... zero;"
# CIL_INIT_TEST += compoundinit1H.c
# CIL_INIT_TEST += compoundinit1I.c

# # Bad error reporting in CIL: Unimplemented: makeZeroCompoundInit:
# # TArray(TComp(struct foo, Should just say that an array with no size
# # in a struct is an incomplete type.
# #CIL_INIT_TEST += compoundinit2.c

# # same initialization bug above
# CIL_INIT_TEST += compoundinit2_1.c

# # same initialization bug above
# CIL_INIT_TEST += compoundinit3A.c
# CIL_INIT_TEST += compoundinit3B.c
# CIL_INIT_TEST += compoundinit3C.c
# CIL_INIT_TEST += compoundinit3D.c

# CIL_INIT_TEST += compoundinit4A.c

# # same initialization bug above
# CIL_INIT_TEST += compoundinit4B.c

# # difference in assignment of string constant; they don't re-use the
# # string
# #CIL_INIT_TEST += compoundinit5A.c
# # won't parse wchar_t
# # ../cil/obj/x86_LINUX/cilly.asm.exe ../LibCpdInit/Test/compoundinit5Aw.c --out ../LibCpdInit/Test/compoundinit5Aw.tmp
# # ../LibCpdInit/Test/compoundinit5Aw.c[7:0-0] : syntax error
# # Error: There were parsing errors in ../LibCpdInit/Test/compoundinit5Aw.c
# #CIL_INIT_TEST += compoundinit5Aw.c

# # same initialization bug above
# CIL_INIT_TEST += compoundinit5B.c
# # won't parse wchar_t as above
# #CIL_INIT_TEST += compoundinit5Bw.c

# # same initialization bug above
# CIL_INIT_TEST += compoundinit6A.c
# CIL_INIT_TEST += compoundinit6B.c

# CIL_INIT_TEST += cil_const-array-init.c

# # ../LibCpdInit/Test/cil_extern_init.c:11: Error: GUID_PARALLEL_DEVICE is extern and with initializer
# # error in createGlobal(GUID_PARALLEL_DEVICE): Errormsg.Error
# # CIL_INIT_TEST += cil_extern_init.c
# CIL_INIT_TEST += cil_init.c
# CIL_INIT_TEST += cil_init1.c
# CIL_INIT_TEST += cil_init10.c
# CIL_INIT_TEST += cil_init11.c
# # output bug when assigning to an empty struct
# #CIL_INIT_TEST += cil_init12.c
# # ../LibCpdInit/Test/cil_init13.c:20:31: error: cannot take address of non-lvalue `struct Anon_struct_2'
# # CIL_INIT_TEST += cil_init13.c
# CIL_INIT_TEST += cil_init14.c
# CIL_INIT_TEST += cil_init15.c
# # Not a compound init test
# #CIL_INIT_TEST += cil_init16.c
# # Not a compound init test
# #CIL_INIT_TEST += cil_init2.c
# CIL_INIT_TEST += cil_init3.c
# CIL_INIT_TEST += cil_init4.c
# # Not sure CIL does the right thing here.
# #CIL_INIT_TEST += cil_init5.c
# # I throw a user error.
# #CIL_INIT_TEST += cil_init6.c
# # Is only checking char arrays inited by strings.
# #CIL_INIT_TEST += cil_init7.c
# CIL_INIT_TEST += cil_init8.c
# # gcc-ism we don't parse
# #CIL_INIT_TEST += cil_init9.c
# CIL_INIT_TEST += cil_initial.c
# CIL_INIT_TEST += cil_struct_init.c

# CIL_REGR_OUT := $(patsubst %.c,LibCpdInit/Test/%.cilout_c,$(CIL_INIT_TEST))
# .PHONY: cil-regr
# cil-regr: cpd-regr/clean $(patsubst %.c,cil-regr/LibCpdInit/Test/%,$(CIL_INIT_TEST))

# .PHONY: cil-regr/clean
# cil-regr/clean:
# 	rm -f LibCpdInit/Test/*.cilout_c LibCpdInit/Test/*.cpdout_c
# 	rm -f LibCpdInit/Test/*.tmp LibCpdInit/Test/*.o

# ifndef SORT
#   $(error You must define SORT)
# endif

# .PHONY: cil-regr/%
# cil-regr/%:
# 	rm -f $*.cilout_c
# 	$(CILLY) $*.c --out $*.tmp
# # the grep -v for zero is to remove the initialization bug
# 	cat $*.tmp | grep '='  | egrep -v '0[U\.]?;$$' | perl -lpe 's/^\s*//' | $(SORT) > $*.cilout_c
# 	diff -u $*.cor $*.cilout_c

# # 	cat $*.tmp | grep '='  | grep '[]\.]' | egrep -v '0\.?;$$' | perl -lpe 's/^\s*//' | $(SORT) > $*.cilout_c
# #	cp $*.cilout_c $*.cor

# # cilly.asm.exe t0133.c --out result.c
# $(CIL_REGR_OUT): %.cilout_c: %.c
# 	$(CILLY) $< --out $@.tmp
# 	grep '=' < $@.tmp | perl -lpe 's/^\s*//' | $(SORT) > $@
# 	rm $@.tmp

CPD_REGR_OUT := $(patsubst %.c,LibCpdInit/Test/%.cpdout_c,$(CPD_INIT_TEST))
.PHONY: cpd-regr
cpd-regr: cpd-regr/clean $(patsubst %.c,cpd-regr/LibCpdInit/Test/%,$(CPD_INIT_TEST))

.PHONY: cpd-regr/%
cpd-regr/%:
	rm -f $*.cpdout_c
	./cpdinit_test -o-lang ANSI_C99 $*.c > Test/temp0.out
	$(SORT) < Test/temp0.out > $*.cpdout_c
	-grep -v 'warning' $*.cor > Test/temp1.out
	-grep -v 'warning\|error\|typechecking results' $*.cpdout_c > Test/temp2.out
	diff -u Test/temp1.out Test/temp2.out

.PHONY: cpd-regr/clean
cpd-regr/clean:
	rm -f LibCpdInit/Test/*.cilout_c LibCpdInit/Test/*.cpdout_c
	rm -f LibCpdInit/Test/*.tmp LibCpdInit/Test/*.o

# $(CPD_REGR_OUT): %.cpdout_c: %.c
# 	./cpdinit_test -o-lang ANSI_C99 $< | $(SORT) > $@
