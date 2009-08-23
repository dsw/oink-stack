# -*-makefile-*-
# Makefile to do module analysis of C/C++ files

ifndef TOP_LEVEL_MAKEFILE
$(error This makefile should be included in the top-level Makefile, not used stand-alone)
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
%.i: %.c
	gcc -E $(CC_FLAGS) -o $@ $<

.PRECIOUS: %.o
%.o: %.i
	gcc -c $(CC_FLAGS) -o $@ $<

$(EXE): %.exe: %.o
	gcc $(CC_FLAGS) -o $@ $^

# **** cleaning

.PHONY: clean
clean:
	@echo; echo "**** $@"
	rm -f *.i *.o *.exe
	rm -f *.lattice

# **** testing

.PHONY: test test/%
test: $(addprefix test/,$(EXE))
$(addprefix test/,$(EXE)): test/%:
	@echo; echo "**** $@"
	./$*

# **** analyzing

# cqual++ and mklattice flags
QUALCC_FLAGS :=
MKLATTICE_FLAGS :=

# do a polymorphic analysis
QUALCC_FLAGS += -fq-poly

# do not do an instance-sensitive analysis
QUALCC_FLAGS += -fo-no-instance-sensitive

# FIX: We need to do a global qualifier analysis, so turn this back on
# QUALCC_FLAGS += -fo-report-link-errors

# it is more conservative to turn this off, so I do; FIX: we could
# turn it on if there is no const casting and we are trusting the
# compiler
QUALCC_FLAGS += -fq-no-use-const-subtyping

# FIX: do we want these?  see comment in the body of
# DataFlowTy_Qual::tDataFlow_FuncTgt
# QUALCC_FLAGS += -fq-casts-preserve
# QUALCC_FLAGS += -fq-casts-preserve-below-functions

# Output formatting
#
# dump out all the flags etc.
# QUALCC_FLAGS += -fo-verbose
# print each stage
# QUALCC_FLAGS += -fo-print-stages
#
# these determine how much information gets attached to a qualfier
# variable: the less the faster the analysis, the more the slower but
# the more helpful the output; if your analysis is slow, try
# -fq-no-names to turn off naming altogether; if name output is on and
# you are reading qualifier output and are having trouble locating an
# important qualifier variable, turn on -fq-name-with-loc
#
# you can turn these off for speed
#
#  -fq-name-expressions     : name expressions
#  -fq-name-vars            : name variables
#  -fq-name-if-missing      : name otherwise unnamed objects
#  -fq-names                : control all other -fq-name flags
#
#  -fq-explain-errors       : print bad dataflow path when one is found
#
# alternatively, turn this on for even more verbose names
#
#  -fq-name-with-loc        : put location info on qualifier variable names
#
# don't look for the original source files
# QUALCC_FLAGS += -tr no-orig-offset

# Use these if you want to see the qualifiers actually annotated onto
# the variables; I think we have to turn off -fq-poly to do that, so I
# do
#
# QUALCC_FLAGS += -fq-print-trans-qual
# QUALCC_FLAGS += -fo-pretty-print
# QUALCC_FLAGS += -fq-no-poly

# Tell the tools about the modules
MKLATTICE_FLAGS += $(addprefix --mod ,$(MODS))
QUALCC_FLAGS += $(addprefix -o-mod-spec ,$(MOD_SPECS))

MKLATTICE := $(OINK_STACK)/oink/module_make_lattice
QUAL := $(OINK_STACK)/oink/qual

# pick exactly one of these
ifeq ($(ANALYSIS),write)
  QUALCC_AN += -fq-module-write
  MKLATTICE_AN += --write
else
ifeq ($(ANALYSIS),access)
  QUALCC_AN += -fq-module-access
  MKLATTICE_AN += --access
else
# this will cause an error if used
  QUALCC_AN += --error-no-such-flag
  MKLATTICE_AN += --error-no-such-flag
# I had to turn this off because the trust analysis uses the same
# makefile but just doesn't use these variables
#   $(error Variable ANALYSIS must have value 'access' or 'write')
endif
endif

.PRECIOUS: %.lattice
.PHONY: analyze-data-priv analyze-data-priv/%
analyze-data-priv: $(addprefix analyze-data-priv/,$(EXE))
$(addprefix analyze-data-priv/,$(EXE)): analyze-data-priv/%:
	@echo; echo "**** $@"
	$(MKLATTICE) $(MKLATTICE_AN) $(MKLATTICE_FLAGS) > ho.lattice
	$(QUAL) -q-config ho.lattice $(QUALCC_AN) $(QUALCC_FLAGS) $^

.PRECIOUS: %.lattice
.PHONY: analyze-data-trust analyze-data-trust/%
analyze-data-trust: $(addprefix analyze-data-trust/,$(EXE))
$(addprefix analyze-data-trust/,$(EXE)): analyze-data-trust/%:
	@echo; echo "**** $@"
	$(MKLATTICE) --trust $(MKLATTICE_FLAGS) > ho2.lattice
	$(QUAL) -fq-module-trust -q-config ho2.lattice $(QUALCC_FLAGS) $^
