# Makefile for doing source-to-source transformation on C code.

all:

OINK := ../..
LOCAL_MALLOC := $(OINK)/local_malloc

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

all-xlize:

# the whole xform below is intended to work on one module at a time
ifndef MODULE
MODULE := gronkmod
endif

CLIENT_FILES :=
# FIX: read this from $(MODULE).mod
CLIENT_FILES += gronk.c

.PHONY: all-xlize
all-xlize: $(CLIENT_FILES:.c=.xlize.c)

.PHONY: diff-xlize
diff-xlize: $(CLIENT_FILES:.c=/diff)

.PHONY: clean-xlize
clean-xlize:
	rm -f *.xlize_log *.xlize_spec *.xlize_patched.c *.xlize.c

# * localize client code

$(CLIENT_FILES:.c=/diff): %/diff: %.xlize_patched.c
	diff -u $*.c $*.xlize_patched.c

# .xlize.c: inserted includes of instantiated local_malloc.h
$(CLIENT_FILES:.c=.xlize.c): %.xlize.c: %.xlize_spec %.xlize_patched.c
	$(OINK)/xlize.py $*.xlize_spec < $*.xlize_patched.c > $@

# .xlize_spec: canonical list of class-module pairs to transform
$(CLIENT_FILES:.c=.xlize_spec): %.xlize_spec: %.xlize_log
	egrep '^localize:' < $< | sort | uniq > $@

# .xlize_patched.c: localizations of call sites to malloc
$(CLIENT_FILES:.c=.xlize_patched.c): %.xlize_patched.c: %.xlize_log %.c
# I get 'missing header' warnings from patch, but it seems to work
	patch -p1 -o $@ $*.c $*.xlize_log

# .xlize_log: the (1) patch and (2) log of localized classes
$(CLIENT_FILES:.c=.xlize_log): %.xlize_log: %.i
	$(OINK)/alloctool -fa-localize-heap-alloc $< \
	  -o-mod-spec $(MODULE):$*.c \
	  -o-mod-spec $(MODULE):$*.h \
	  -o-mod-default default \
	  > $@
