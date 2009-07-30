# Makefile for guarded-command example grammar

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
all: lexer parser


%.o: %.cc
	g++ -c -o $@ $(CCFLAGS) $*.cc

tokens.tok: lexer.h $(ELKHOUND)/make-tok
	perl $(ELKHOUND)/make-tok TokenCode <lexer.h >$@

lexer: lexer.cc
	g++ -o $@ $(CCFLAGS) -DTEST_LEXER lexer.cc $(LDFLAGS)

gcom.h gcom.cc: gcom.gr tokens.tok
	$(ELKHOUND)/elkhound gcom.gr

parser.o: gcom.h

PARSER_OBJS := lexer.o parser.o gcom.o
parser: $(PARSER_OBJS)
	g++ -o $@ $(PARSER_OBJS) $(LDFLAGS)

check:
	echo "2 + 3" | ./lexer
	echo "2 + 3" | ./parser

clean:
	rm -f lexer parser *.o tokens.tok gcom.h gcom.cc gmon.out

