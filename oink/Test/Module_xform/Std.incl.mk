# **** standard build

ifndef TOP_LEVEL_MAKEFILE
$(error This makefile should be included in the top-level Makefile, not used stand-alone)
endif

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
