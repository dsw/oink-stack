# Makefile for doing source-to-source transformation on C code.

all:

OINK_STACK := ../../..
OINK := $(OINK_STACK)/oink
LOCAL_MALLOC := $(OINK_STACK)/local_malloc

EXE :=
EXE += gronk.exe

# FIX: read this from $(MODULE).mod
CLFLS :=
CLFLS += gronk.c

gronk.exe: $(CLFLS:.c=.o)

# the whole xform below is intended to work on one module at a time
ifndef MODULE
MODULE := gronkmod
endif

# **** all

.PHONY: all all-splash
all: all-splash $(EXE)
all-splash:
	@echo; echo "**** $(@:-splash=)"

# **** building

CC_FLAGS :=
CC_FLAGS += -Wall

.PRECIOUS: %.i
$(CLFLS:.c=.i): %.i: %.c
	gcc -E $(CC_FLAGS) -o $@ $<

.PRECIOUS: %.o
$(CLFLS:.c=.o): %.o: %.i
	gcc -c $(CC_FLAGS) -o $@ $<

$(EXE): %.exe:
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

gronk.xlz_exe: $(CLFLS:.c=.xlz.o)

XLZ_CLEAN =

.PHONY: xlz
xlz: xlz-client-files xlz-local-malloc

.PHONY: xlz-test
xlz-test:
	@echo; echo "**** $@"
	./$(EXE:.exe=.xlz_exe)

.PHONY: xlz-all
xlz-all: $(EXE:.exe=.xlz_exe)

# FIX: generate the local malloc instantiations and then use makefile
# include to get them here as dependencies

# link the xlz-ed client files and instantiated local malloc
XLZ_CLEAN += $(EXE:.exe=.xlz_exe)
# move this below when implemented there
XLZ_CLEAN += local_malloc_%.o
$(EXE:.exe=.xlz_exe): %.xlz_exe:
	@echo; echo "**** $@"
	gcc $(CC_FLAGS) -o $@ $^ local_malloc_*.c

# # build the localized malloc .o files
# local_malloc_%.o: local_malloc_%.c
# XLZ_CLEAN += local_malloc_%.o
# 	gcc -c $(CC_FLAGS) -o $@ $<

# build xlz-ed client .o files
XLZ_CLEAN += $(CLFLS:.c=.xlz.o)
$(CLFLS:.c=.xlz.o): %.xlz.o: %.c
	@echo; echo "**** $@"
	gcc -c $(CC_FLAGS) -o $@ $<

.PHONY: xlz-diff
xlz-diff: $(CLFLS:=/xlz_diff)

.PHONY: xlz-clean
xlz-clean:
	@echo; echo "**** $@"
	rm -f $(XLZ_CLEAN)

# * instantiate the local malloc template

.PHONY: xlz-local-malloc
xlz-local-malloc: local_malloc_$(MODULE)_clsids.h
xlz-local-malloc: xlz-inst-lm
xlz-local-malloc: xlz-grep-clsdef

# instantiate local malloc: for each line in malloc spec, if the class
# is statically-sized, then expand local_malloc_HOMgld.c to
# local_malloc_<alnum_id>.c; for dynamically-size classes print a
# warning
XLZ_CLEAN += local_malloc_*.c local_malloc_*.h
.PHONY: xlz-inst-lm
xlz-inst-lm: $(MODULE).xlz_ml_spec
	@echo; echo "**** $@"
	$(OINK)/xlz_inst_lm.py \
          --module $(MODULE) \
          --local-malloc $(LOCAL_MALLOC) \
          --ml-spec $<

# make the enum of module class ids for this module
XLZ_CLEAN += local_malloc_$(MODULE)_clsids.h
local_malloc_$(MODULE)_clsids.h: $(MODULE).xlz_ml_spec
	@echo; echo "**** $@"
	$(OINK)/xlz_inst_clsids.py \
          --module $(MODULE) \
          --local-malloc $(LOCAL_MALLOC) \
          --ml-spec $<

# module local_malloc-s instantiation specification
XLZ_CLEAN += $(MODULE).xlz_ml_spec
$(MODULE).xlz_ml_spec: %.xlz_ml_spec: $(CLFLS:=.xlz_xf_spec)
	@echo; echo "**** $@"
	sort $^ | uniq > $@

# make the local_malloc_clsdef_HOMgld.h file for each class: scan for
# the first mention of the class in the module class definition spec,
# if the class is statically-sized, then grep out the #-lines from
# that source file and instantiate the clsdef header file; for
# dynamically-size classes print a warning
XLZ_CLEAN += local_malloc_clsdef_*.h
.PHONY: xlz-grep-clsdef
xlz-grep-clsdef: $(MODULE).xlz_which_cls_where
	@echo; echo "**** $@"
	$(OINK)/xlz_grep_clsdef.py $<

# make the module class definition spec: foreach xformspec, insert the
# source filename into each line after the mangled name; concatenate;
# sort; print
XLZ_CLEAN += $(MODULE).xlz_which_cls_where
$(MODULE).xlz_which_cls_where: %.xlz_which_cls_where: $(CLFLS:=.xlz_xf_spec)
	@echo; echo "**** $@"
	$(OINK)/xlz_which_cls_where.py $^ > $@

# * localize client code

# Note that we assume that the source file suffix is .c when we need
# to reconstruct it from these suffixes

# print diff of source and transformed code
.PHONY: $(CLFLS:=/xlz_diff)
$(CLFLS:=/xlz_diff): %/xlz_diff: %.xlz.c
	diff -u $* $*.xlz.c

# xlz all the client source files
.PHONY: xlz-client-files
xlz-client-files: $(CLFLS:=.xlz.c)

# .xlz.c: insert includes of instantiated local_malloc.h to patched src
XLZ_CLEAN += $(CLFLS:=.xlz.c)
$(CLFLS:=.xlz.c): %.xlz.c: %.xlz_xf_spec %.xlz_patched.c
	@echo; echo "**** $@"
	$(OINK)/xlz_incl_lm.py $*.xlz_xf_spec < $*.xlz_patched.c > $@

# .xlz_xf_spec: canonical list of class-module pairs to transform
XLZ_CLEAN += $(CLFLS:=.xlz_xf_spec)
$(CLFLS:=.xlz_xf_spec): %.xlz_xf_spec: %.xlz_log
	@echo; echo "**** $@"
	egrep '^localize:' < $< | sort | uniq > $@

# .xlz_patched.c: localizations of call sites to malloc
XLZ_CLEAN += $(CLFLS:=.xlz_patched.c)
$(CLFLS:.c=.c.xlz_patched.c): %.c.xlz_patched.c: %.c.xlz_log %.c
# I get 'missing header' warnings from patch, but it seems to work
	@echo; echo "**** $@"
	patch -p1 -o $@ $*.c $*.c.xlz_log

# .xlz_log: the (1) patch and (2) log of localized classes
XLZ_CLEAN += $(CLFLS:=.xlz_log)
# FIX: this has to be modified if you use an input file suffix other
# than ".c"
$(CLFLS:.c=.c.xlz_log): %.c.xlz_log: %.i
	@echo; echo "**** $@"
	$(OINK)/alloctool -fa-localize-heap-alloc $< \
	  -o-mod-spec $(MODULE):$*.c \
	  -o-mod-spec $(MODULE):$*.h \
	  -o-mod-default default \
	  > $@
