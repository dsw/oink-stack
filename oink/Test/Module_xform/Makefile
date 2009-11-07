# Makefile for doing source-to-source transformation on C code.

all:

OINK_STACK := ../../..
OINK := $(OINK_STACK)/oink
LOCAL_MALLOC := $(OINK_STACK)/local_malloc

EXE :=
EXE += gronk.exe

# FIX: read this from $(MODULE).mod
CLIENT_FILES :=
CLIENT_FILES += gronk.c

gronk.exe: $(CLIENT_FILES:.c=.o)

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
$(CLIENT_FILES:.c=.i): %.i: %.c
	gcc -E $(CC_FLAGS) -o $@ $<

.PRECIOUS: %.o
$(CLIENT_FILES:.c=.o): %.o: %.i
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

gronk.xlize_exe: $(CLIENT_FILES:.c=.xlize.o)

XLIZE_CLEAN =

.PHONY: xlize
xlize: xlize-files xlize-local-malloc 

.PHONY: xlize-test
xlize-test:
	@echo; echo "**** $@"
	./$(EXE:.exe=.xlize_exe)

.PHONY: xlize-all
xlize-all: $(EXE:.exe=.xlize_exe)

# FIX: generate the local malloc instantiations and then use makefile
# include to get them here as dependencies

# link the xlize-ed client files and instantiated local malloc
XLIZE_CLEAN += $(EXE:.exe=.xlize_exe)
# move this below when implemented there
XLIZE_CLEAN += local_malloc_%.o
$(EXE:.exe=.xlize_exe): %.xlize_exe:
	@echo; echo "**** $@"
	gcc $(CC_FLAGS) -o $@ $^ local_malloc_*.c

# # build the localized malloc .o files
# local_malloc_%.o: local_malloc_%.c
# XLIZE_CLEAN += local_malloc_%.o
# 	gcc -c $(CC_FLAGS) -o $@ $<

# build xlize-ed client .o files
XLIZE_CLEAN += $(CLIENT_FILES:.c=.xlize.o)
$(CLIENT_FILES:.c=.xlize.o): %.xlize.o: %.c
	@echo; echo "**** $@"
	gcc -c $(CC_FLAGS) -o $@ $<

.PHONY: xlize-diff
xlize-diff: $(CLIENT_FILES:=/xlize_diff)

.PHONY: xlize-clean
xlize-clean:
	@echo; echo "**** $@"
	rm -f $(XLIZE_CLEAN)

# * instantiate local_malloc_HOMgld.c

# instantiate local malloc: for each line in malloc spec, if the class
# is statically-sized, then expand local_malloc_HOMgld.c to
# local_malloc_<alnum_name>.c; for dynamically-size classes print a
# warning
XLIZE_CLEAN += local_malloc_*.c local_malloc_*.h
.PHONY: xlize-local-malloc
xlize-local-malloc: xlize-class-defn $(MODULE).xlize_ml_spec
	@echo; echo "**** $@"
	$(OINK)/xlize_local_malloc.py \
          --local-malloc $(LOCAL_MALLOC) \
          --ml-spec $(filter-out xlize-class-defn,$^)

# module local_malloc-s instantiation specification
XLIZE_CLEAN += $(MODULE).xlize_ml_spec
$(MODULE).xlize_ml_spec: %.xlize_ml_spec: $(CLIENT_FILES:=.xlize_xf_spec)
	@echo; echo "**** $@"
	sort $^ | uniq > $@

# make the local_malloc_clsdef_HOMgld.h file for each class: scan for
# the first mention of the class in the module class definition spec,
# if the class is statically-sized, then grep out the #-lines from
# that source file and instantiate the clsdef header file; for
# dynamically-size classes print a warning
XLIZE_CLEAN += local_malloc_clsdef_*.h
.PHONY: xlize-class-defn
xlize-class-defn: $(MODULE).xlize_cl_spec
	@echo; echo "**** $@"
	$(OINK)/xlize_class_defn.py $<

# module class definition spec: for each xform spec, insert the source
# filename into each line after the mangled name; concatenate them;
# sort that file
XLIZE_CLEAN += $(MODULE).xlize_cl_spec
$(MODULE).xlize_cl_spec: %.xlize_cl_spec: $(CLIENT_FILES:=.xlize_xf_spec)
	@echo; echo "**** $@"
	$(OINK)/xlize_cl_spec.py $^ > $@

# * localize client code

# Note that we assume that the source file suffix is .c when we need
# to reconstruct it from these suffixes

# print diff of source and transformed code
.PHONY: $(CLIENT_FILES:=/xlize_diff)
$(CLIENT_FILES:=/xlize_diff): %/xlize_diff: %.xlize.c
	diff -u $* $*.xlize.c

# xlize all the client source files
.PHONY: xlize-files
xlize-files: $(CLIENT_FILES:=.xlize.c)

# .xlize.c: insert includes of instantiated local_malloc.h to patched src
XLIZE_CLEAN += $(CLIENT_FILES:=.xlize.c)
$(CLIENT_FILES:=.xlize.c): %.xlize.c: %.xlize_xf_spec %.xlize_patched.c
	@echo; echo "**** $@"
	$(OINK)/xlize_incl_malloc.py $*.xlize_xf_spec < $*.xlize_patched.c > $@

# .xlize_xf_spec: canonical list of class-module pairs to transform
XLIZE_CLEAN += $(CLIENT_FILES:=.xlize_xf_spec)
$(CLIENT_FILES:=.xlize_xf_spec): %.xlize_xf_spec: %.xlize_log
	@echo; echo "**** $@"
	egrep '^localize:' < $< | sort | uniq > $@

# .xlize_patched.c: localizations of call sites to malloc
XLIZE_CLEAN += $(CLIENT_FILES:=.xlize_patched.c)
$(CLIENT_FILES:.c=.c.xlize_patched.c): %.c.xlize_patched.c: %.c.xlize_log %.c
# I get 'missing header' warnings from patch, but it seems to work
	@echo; echo "**** $@"
	patch -p1 -o $@ $*.c $*.c.xlize_log

# .xlize_log: the (1) patch and (2) log of localized classes
XLIZE_CLEAN += $(CLIENT_FILES:=.xlize_log)
# FIX: this has to be modified if you use an input file suffix other
# than ".c"
$(CLIENT_FILES:.c=.c.xlize_log): %.c.xlize_log: %.i
	@echo; echo "**** $@"
	$(OINK)/alloctool -fa-localize-heap-alloc $< \
	  -o-mod-spec $(MODULE):$*.c \
	  -o-mod-spec $(MODULE):$*.h \
	  -o-mod-default default \
	  > $@
