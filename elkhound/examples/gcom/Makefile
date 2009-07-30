# Makefile for guarded-command example grammar

# directories
ELKHOUND := ../..
AST      := ../../../ast
SMBASE   := ../../../smbase

# libraries
LIBELKHOUND := $(ELKHOUND)/libelkhound.a
LIBAST      := $(AST)/libast.a
LIBSMBASE   := $(SMBASE)/libsmbase.a

# compilation flags
CCFLAGS := $(shell $(SMBASE)/config.summary | grep CCFLAGS | sed 's/^.*: *//')
CCFLAGS += -I$(ELKHOUND) -I$(AST) -I$(SMBASE)
LDFLAGS := -g -Wall $(LIBELKHOUND) $(LIBAST) $(LIBSMBASE)

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

ast.h ast.cc: gcom.ast
	$(AST)/astgen -o ast gcom.ast

parser.o: gcom.h ast.h

PARSER_OBJS := lexer.o parser.o gcom.o ast.o eval.o
parser: $(PARSER_OBJS)
	g++ -o $@ $(PARSER_OBJS) $(LDFLAGS)

check:
	echo "x := 2 + 3 + 4; print x" | ./lexer
	echo "x := 2 + 3 + 4; print x" | ./parser -ast
	echo "x := 2 + 3 + 4; print x" | ./parser -tree

clean:
	rm -f lexer parser *.o tokens.tok gcom.h gcom.cc ast.h ast.cc gmon.out

