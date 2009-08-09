.PHONY: all clean check

TOCLEAN =

# ****
# this is all the change we need to make to intercept the .i files
TOCLEAN += *.i *.s
CFLAGS += -save-temps
# ****

all: hello

clean:
	rm -f $(TOCLEAN)

check:
	@echo Testing.
	HELLO='good data' ./hello
	HELLO='evil data' ./hello

TOCLEAN += *.o
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

TOCLEAN += hello
hello: main.o g.o
	$(CC) -o $@ $^

# ****
# this is all we have to do for a simple analysis
OINK_STACK := ../../..
LATTICE := $(OINK_STACK)/libqual/config/lattice
.PHONY: analyze
analyze: all
	$(OINK_STACK)/oink/qual -q-config $(LATTICE) main.i g.i mini_libc.c
# this isn't working but is a more general way to do it
# 	$(OINK_STACK)/oink-scripts/bin/qualx main.i g.i -lc
# ****
