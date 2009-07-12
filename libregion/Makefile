# see License.txt for copyright and terms of use

.SUFFIXES:

SMBASE_DIR := ../smbase
STACK_DIR  := ..

CC    := gcc
AR     := ar
RANLIB := ranlib
DEP    := $(SMBASE_DIR)/depend.pl

# FIX: These should be set at configure time
CFLAGS :=
CFLAGS += -DNMEMDEBUG -DSIZEOF_VOIDP=4

CFLAGS += -Wall

# turn on debugging symbols
CFLAGS += -g

# turn on gcov
# CFLAGS += -fprofile-arcs -ftest-coverage

# turn on gprof
# CFLAGS += -pg

# turn on optimizations
CFLAGS += -O2
## Note: -O3 compiles incorrectly!
#CFLAGS += -O3

OBJ :=
OBJ += regions.o
OBJ += profile.o

LIB :=
LIB += libregion.a

.PHONY: all
all: $(LIB)

.PHONY: clean
clean:
# generated files
	rm -f $(OBJ) $(LIB)
# artifacts of building with the dependencies system
	rm -f *.d

# for now, check-full is just check
.PHONY: check-full
check-full: check

.PHONY: check
check:
	@echo "No check in libregion; checking done by the oink/qual tests."

# compile
$(OBJ): %.o: %.c
	$(CC) -c -o $@ -I$(STACK_DIR) -I- $(CFLAGS) $<
	$(DEP)   -o $@ -I$(STACK_DIR) -I- $(CFLAGS) $< > $*.d
-include $(OBJ:.o=.d)

# archive
libregion.a: $(OBJ)

$(LIB): %.a:
	$(AR) cru $@ $^
	$(RANLIB) $@
