# Makefile for doing source-to-source transformation on C code.

all:

EXE :=
EXE += gronk.exe

# **** all

.PHONY: all all-splash
all: all-splash $(EXE)
all-splash:
	@echo; echo "**** $(@:-splash=)"

# **** building

CC_FLAGS :=
CC_FLAGS += -Wall

.PRECIOUS: %.i
%.i: %.c
	gcc -E $(CC_FLAGS) -o $@ $<

.PRECIOUS: %.o
%.o: %.i
	gcc -c $(CC_FLAGS) -o $@ $<

$(EXE): %.exe: %.o
	gcc $(CC_FLAGS) -o $@ $^

# **** cleaning

.PHONY: clean
clean:
	@echo; echo "**** $@"
	rm -f *.i *.o *.exe

# **** testing

.PHONY: test test/%
test: $(addprefix test/,$(EXE))
$(addprefix test/,$(EXE)): test/%:
	@echo; echo "**** $@"
	./$*

# **** transforming

OINK := ../..

ifndef MODULE
MODULE := gronk
endif

# .xlize.c: inserted includes of instantiated local_malloc.h
gronk.xlize.c: %.xlize.c: %.xlize_spec %.xlize_patched.c
	$(OINK)/xlize.py $*.xlize_spec < $*.xlize_patched.c > $@

# .xlize_spec: canonical list of class-module pairs to transform
gronk.xlize_spec: %.xlize_spec: %.xlize_log
	egrep '^localize:' < $< | sort | uniq > $@

# .xlize_patched.c: localizations of call sites to malloc
gronk.xlize_patched.c: %.xlize_patched.c: %.xlize_log %.c
# I get 'missing header' warnings from patch, but it seems to work
	patch -p1 -o $@ $*.c $*.xlize_log

# .xlize_log: the (1) patch and (2) log of localized classes
gronk.xlize_log: %.xlize_log: %.i
	$(OINK)/alloctool -fa-localize-heap-alloc $< \
	  -o-mod-spec $(MODULE):$*.c \
	  -o-mod-spec $(MODULE):$*.h \
	  -o-mod-default default \
	  > $@

.PHONY: clean-xlize
clean-xlize:
	rm -f *.xlize_log *.xlize_spec *.xlize_patched.c *.xlize.c
