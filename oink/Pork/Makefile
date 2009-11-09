.PHONY: all
all:

# Built stopwatch as a demo of Pork's rewriting capabilities.

OINK_STACK := ../..

# **** configure

EXE := stopwatch

STOPWATCH_O :=
STOPWATCH_O += patcher.o
STOPWATCH_O += piglet.o
STOPWATCH_O += stopwatch.o

stopwatch: $(STOPWATCH_O)

# **** build

all: $(EXE)

L_FLAGS :=
L_FLAGS += -L..
L_FLAGS += -L../../ast
L_FLAGS += -L../../elkhound
L_FLAGS += -L../../smbase
L_FLAGS += -L../../libqual
L_FLAGS += -L../../libregion

l_FLAGS :=
l_FLAGS += -lelsa
l_FLAGS += -last
l_FLAGS += -lelkhound
l_FLAGS += -lsmbase
l_FLAGS += -lqual
l_FLAGS += -lregion

# dataflow functionality
DATAFLOW_LOCAL_CC_O :=

DATAFLOW_LOCAL_CC_O += dataflow_cpdinit_clbk.o
DATAFLOW_LOCAL_CC_O += dataflow_visitor.o
DATAFLOW_LOCAL_CC_O += dataflow_ex.o
DATAFLOW_LOCAL_CC_O += dataflow_ty.o

# additional files that the oink executable depends on
OINK_LOCAL_CC_O :=

OINK_LOCAL_CC_O += oink_lexer.yy.o
OINK_LOCAL_CC_O += oink.gr.gen.o

# built from the LibCpdInit tree
CPDINIT_O :=
CPDINIT_O += cpdinit.o
CPDINIT_O += member_type_iter.o

# All analyses need these
BASIC_LOCAL_CC_O :=

# utilities
BASIC_LOCAL_CC_O += libc_missing.o
BASIC_LOCAL_CC_O += oink_file.o
BASIC_LOCAL_CC_O += oink_util.o
BASIC_LOCAL_CC_O += oink_control.o
# BASIC_LOCAL_CC_O += oink_srz.o
BASIC_LOCAL_CC_O += archive_srz.o
BASIC_LOCAL_CC_O += archive_srz_format.o
BASIC_LOCAL_CC_O += dbg_util.o
BASIC_LOCAL_CC_O += warning_mgr.o

# lexing and parsing
# NOTE: Remove when get separate token files working
BASIC_LOCAL_CC_O += cc_tokens.o
# NOTE: Remove when get separate ast-s working
BASIC_LOCAL_CC_O += cc.ast.gen.o
BASIC_LOCAL_CC_O += oink_ast_aux.o

# typechecking
BASIC_LOCAL_CC_O += oink_type.o
BASIC_LOCAL_CC_O += oink_type_children.o
BASIC_LOCAL_CC_O += oink_tcheck.o
BASIC_LOCAL_CC_O += oink_tcheck_env.o
BASIC_LOCAL_CC_O += oink_tcheck_env_clbk.o
BASIC_LOCAL_CC_O += oink_var.o
BASIC_LOCAL_CC_O += oink_integrity.o

# abstract value
BASIC_LOCAL_CC_O += value.o
BASIC_LOCAL_CC_O += value_ast_visitor.o
BASIC_LOCAL_CC_O += xml_value_reader.o
BASIC_LOCAL_CC_O += xml_value_writer.o

# walks
BASIC_LOCAL_CC_O += cc_print.o
BASIC_LOCAL_CC_O += value_print.o

# whole
BASIC_LOCAL_CC_O += oink_global.o
BASIC_LOCAL_CC_O += oink_cmd.o
BASIC_LOCAL_CC_O += oink.o

# put them all together
OINK_O :=
OINK_O += $(DATAFLOW_LOCAL_CC_O)
OINK_O += $(OINK_LOCAL_CC_O)
OINK_O += $(CPDINIT_O)
OINK_O += $(BASIC_LOCAL_CC_O)

OINK_O := $(addprefix ../,$(OINK_O))

$(EXE):
	g++ -g -o $@ $(L_FLAGS) $^ $(OINK_O) $(l_FLAGS)

W_FLAGS := -Wall -Wno-deprecated

# D_FLAGS := -DOINK -DCPDINIT_LIB_HEADERS='"cpdinit_headers.h"' -DTYPE_CLASS_FILE='"oink_type.h"' -DUSE_SERIAL_NUMBERS=1
D_FLAGS := -D__UNIX__ -DUSE_SERIAL_NUMBERS=1 -DNDEBUG -D__LINUX__

I_FLAGS :=
I_FLAGS += -I$(OINK_STACK)/smbase
I_FLAGS += -I$(OINK_STACK)/elkhound
I_FLAGS += -I$(OINK_STACK)/ast
I_FLAGS += -I$(OINK_STACK)/elsa
I_FLAGS += -I..
I_FLAGS += -I.

OBJS :=
OBJS += $(STOPWATCH_O)

$(OBJS): %.o: %.cc
	g++ -g $(W_FLAGS) $(D_FLAGS) $(I_FLAGS) -o $@ -c $^

# **** clean

.PHONY: clean
clean:
	rm -f *.o $(EXE)
