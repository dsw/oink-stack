# Perform source-to-source transformation for localization on C code

xlz-go:

# **** configuration

OINK_STACK := ../../..
OINK := $(OINK_STACK)/oink
ELSA := $(OINK_STACK)/elsa
LOCAL_MALLOC := $(OINK_STACK)/local_malloc

# * module configuration

# the whole xform below is intended to work on one module at a time
ifndef MODULE
MODULE := gronkmod
endif

EXE :=
EXE += gronk.exe

CLFLS :=
CLFLS += gronk.c
CLFLS += gronk_b.c
CLFLS += gronk_b_tor.c

MOD_FILES := $(CLFLS) 
MOD_FILES += gronk.h
MOD_FILES += gronk_b.h

gronk.exe: $(CLFLS:.c=.o)
gronk.xlz_exe: $(CLFLS:.c=.c.xlz.o)

# * util

.PHONY: %-splash
%-splash:
	@echo; echo "**** $(@:-splash=)"

# **** main

# run the whole process from start to finish
.PHONY: xlz-go
xlz-go: xlz-go-clean xlz-go-all xlz-go-test

.PHONY: xlz-go-all
xlz-go-all: all xlz-inst xlz-all

.PHONY: xlz-go-clean
xlz-go-clean: clean xlz-inst-clean xlz-clean

.PHONY: xlz-go-test
xlz-go-test: test xlz-test

# **** standard build

# * all

.PHONY: all
all: all-splash $(EXE)

# * clean

.PHONY: clean
clean:
	@echo; echo "**** $@"
	rm -f *.i *.o *.exe

# * test

.PHONY: test test/%
test: $(addprefix test/,$(EXE))
$(addprefix test/,$(EXE)): test/%:
	@echo; echo "**** $@"
	./$*

# *

CC_FLAGS :=
CC_FLAGS += -Wall

$(EXE): %.exe:
	gcc $(CC_FLAGS) -o $@ $^

.PRECIOUS: %.o
$(CLFLS:.c=.o): %.o: %.i
	gcc -c $(CC_FLAGS) -o $@ $<

.PRECIOUS: %.i
$(CLFLS:.c=.i): %.i: %.c
	gcc -E $(CC_FLAGS) -o $@ $<

# **** transform/localize build

# * all

.PHONY: xlz-all
xlz-all: xlz-all-splash $(EXE:.exe=.xlz_exe)

# * clean

XLZ_CLEAN =
.PHONY: xlz-clean
xlz-clean:
	@echo; echo "**** $@"
	rm -f $(XLZ_CLEAN)

# * test

.PHONY: xlz-test
xlz-test:
	@echo; echo "**** $@"
	./$(EXE:.exe=.xlz_exe)

# *

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
XLZ_CLEAN += $(CLFLS:.c=.c.xlz.o)
$(CLFLS:.c=.c.xlz.o): %.c.xlz.o: %.c.xlz.c
	@echo; echo "**** $@"
	gcc -c $(CC_FLAGS) -o $@ $<

# **** transform/localize instantiation

.PHONY: xlz-inst
xlz-inst: xlz-inst-splash xlz-client-files xlz-local-malloc

XLZ_INST_CLEAN =
.PHONY: xlz-inst-clean
xlz-inst-clean:
	@echo; echo "**** $@"
	rm -f $(XLZ_INST_CLEAN)

.PHONY: xlz-inst-diff
xlz-inst-diff: $(CLFLS:=/xlz-inst-diff)

# * instantiate the local malloc template

.PHONY: xlz-local-malloc
xlz-local-malloc: local_malloc_$(MODULE)_clsids.h
xlz-local-malloc: xlz-inst-lm
xlz-local-malloc: xlz-grep-clsdef

# instantiate local malloc: for each line in malloc spec, if the class
# is statically-sized, then expand local_malloc_HOMgld.c to
# local_malloc_<alnum_id>.c; for dynamically-size classes print a
# warning
XLZ_INST_CLEAN += local_malloc_*.c local_malloc_*.h
.PHONY: xlz-inst-lm
xlz-inst-lm: $(MODULE).xlz_ml_spec
	@echo; echo "**** $@"
	$(OINK)/xlz_inst_lm.py \
          --module $(MODULE) \
          --local-malloc $(LOCAL_MALLOC) \
          --ml-spec $<

# make the enum of module class ids for this module
XLZ_INST_CLEAN += local_malloc_$(MODULE)_clsids.h
local_malloc_$(MODULE)_clsids.h: $(MODULE).xlz_ml_spec
	@echo; echo "**** $@"
	$(OINK)/xlz_inst_clsids.py \
          --module $(MODULE) \
          --local-malloc $(LOCAL_MALLOC) \
          --ml-spec $<

# module local_malloc-s instantiation specification
XLZ_INST_CLEAN += $(MODULE).xlz_ml_spec
$(MODULE).xlz_ml_spec: %.xlz_ml_spec: $(CLFLS:=.xlz_xf_spec)
	@echo; echo "**** $@"
	sort $^ | uniq > $@

# make the local_malloc_clsdef_HOMgld.h file for each class: scan for
# the first mention of the class in the module class definition spec,
# if the class is statically-sized, then grep out the #-lines from
# that source file and instantiate the clsdef header file; for
# dynamically-size classes print a warning
XLZ_INST_CLEAN += local_malloc_clsdef_*.h
.PHONY: xlz-grep-clsdef
xlz-grep-clsdef: $(MODULE).xlz_which_cls_where
	@echo; echo "**** $@"
	$(OINK)/xlz_grep_clsdef.py $<

# make the module class definition spec: foreach xformspec, insert the
# source filename into each line after the mangled name; concatenate;
# sort; print
XLZ_INST_CLEAN += $(MODULE).xlz_which_cls_where
$(MODULE).xlz_which_cls_where: %.xlz_which_cls_where: $(CLFLS:=.xlz_xf_spec)
	@echo; echo "**** $@"
	$(OINK)/xlz_which_cls_where.py $^ > $@

# * localize client code

# print diff of source and transformed code
.PHONY: $(CLFLS:=/xlz-inst-diff)
$(CLFLS:=/xlz-inst-diff): %/xlz-inst-diff: %.xlz.c
	diff -u $* $*.xlz.c

# transform/localize all the client source files
.PHONY: xlz-client-files
xlz-client-files: $(CLFLS:=.xlz.c)

# .xlz.c: insert includes of instantiated local_malloc.h to patched src
XLZ_INST_CLEAN += $(CLFLS:=.xlz.c)
$(CLFLS:=.xlz.c): %.xlz.c: %.xlz_xf_spec %.xlz_patched.c
	@echo; echo "**** $@"
	$(OINK)/xlz_incl_lm.py $*.xlz_xf_spec < $*.xlz_patched.c > $@

# .xlz_xf_spec: canonical list of class-module pairs to transform
XLZ_INST_CLEAN += $(CLFLS:=.xlz_xf_spec)
$(CLFLS:=.xlz_xf_spec): %.xlz_xf_spec: %.xlz_log
	@echo; echo "**** $@"
	egrep '^localize:' < $< | sort | uniq > $@

# .xlz_patched.c: localizations of call sites to malloc
XLZ_INST_CLEAN += $(CLFLS:=.xlz_patched.c) $(CLFLS:=.xlz_log.just_patch)
$(CLFLS:.c=.c.xlz_patched.c): %.c.xlz_patched.c: %.c.xlz_log %.c
# I get 'missing header' warnings from patch, but it seems to work
	@echo; echo "**** $@"
# damn it Larry, learn what orthogonality means already!
#	$(ELSA)/chop_out < $*.c.xlz_log | patch -p1 -o $@ -i - $*.c
	$(ELSA)/chop_out < $*.c.xlz_log > $*.c.xlz_log.just_patch
	if test -s $*.c.xlz_log.just_patch; \
          then patch -p1 -o $@ -i $*.c.xlz_log.just_patch $*.c; \
          else cp $*.c $@; \
        fi

# .xlz_log: the (1) patch and (2) log of localized classes
XLZ_INST_CLEAN += $(CLFLS:=.xlz_log)
# FIX: this has to be modified if you use an input file suffix other
# than ".c"
$(CLFLS:.c=.c.xlz_log): %.c.xlz_log: %.i
	@echo; echo "**** $@"
	$(OINK)/xform -fx-localize-heap-alloc $< \
	  $(addprefix -o-mod-spec $(MODULE):, $(MOD_FILES)) \
	  -o-mod-default default \
	  > $@
