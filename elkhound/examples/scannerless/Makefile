# Makefile for scannerless example grammar

# directories
ELKHOUND := ../..
SMBASE   := ../../../smbase

# libraries
LIBELKHOUND := $(ELKHOUND)/libelkhound.a
LIBSMBASE   := $(SMBASE)/libsmbase.a

# compilation flags
CCFLAGS := $(shell $(SMBASE)/config.summary | grep CCFLAGS | sed 's/^.*: *//')
CCFLAGS += -I$(ELKHOUND) -I$(SMBASE)
LDFLAGS := -g -Wall $(LIBELKHOUND) $(LIBSMBASE)

# default make targets
all: sless


%.o: %.cc
	g++ -c -o $@ $(CCFLAGS) $*.cc

sless.h sless.cc: sless.gr
	$(ELKHOUND)/elkhound sless.gr

main.o: sless.h

SLESS_OBJS := main.o sless.o
sless: $(SLESS_OBJS)
	g++ -o $@ $(SLESS_OBJS) $(LDFLAGS)

check:
	echo "a aa a aaa" | ./sless

clean:
	rm -f sless *.o sless.h sless.cc

