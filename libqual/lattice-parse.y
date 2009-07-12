/* see License.txt for copyright and terms of use */

%token<str> QUALIFIER STRING
%token COLOR EFFECT CASTS_PRESERVE FLOW_INSENSITIVE FLOW_SENSITIVE EQ LEVEL NEG NONPROP ORDER
%token PARTIAL POS REF SIGN VALUE PTRFLOW FIELDFLOW FIELDPTRFLOW
%token UP DOWN ALL NONE

%{
  #include "libqual/quals.h"
  #include "libregion/regions.h"
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  int lyylex(void);
  void lyyerror(char *);
  extern char *lyytext;
  int lattice_line = 0;
  char *lattice_file;
  qual current_qual = NULL;
  region lattice_strings_region = NULL;
%}

%union {
  char *str;
}

%start po

%%

po:     /* empty */
        | PARTIAL ORDER { begin_po_qual(); }
	    po_attr_opt '{' descr '}' { end_po_qual(); } po
	;

po_attr_opt:  /* empty */
	    | '[' po_attrs ']'
	    ;

po_attrs: po_attr
        | po_attrs ',' po_attr
        ;

po_attr: FLOW_INSENSITIVE { }
       | FLOW_SENSITIVE { set_po_flow_sensitive(); }
       | NONPROP { set_po_nonprop(); }
       | EFFECT { set_po_effect(); }
       | CASTS_PRESERVE { set_po_casts_preserve(); }
       ;

descr:	  /* empty */
	| qual_attr descr
	| qual_order descr
	;

qual_attr:
	  QUALIFIER
	  { add_qual($1); }
	| QUALIFIER { current_qual = add_qual($1); } '[' attributes ']'
	  { current_qual = NULL; }
        ;

attributes:
	  attribute
	| attributes ',' attribute
	;

attribute:
	  COLOR '=' STRING
	  { add_color_qual(current_qual, $3); }
	| level_attribute
	| sign_attribute
	| ptrflow_attribute
	| fieldflow_attribute
	| fieldptrflow_attribute
	;

level_attribute:
	  LEVEL '=' REF
	  { add_level_qual(current_qual, level_ref); }
	| LEVEL '=' VALUE
	  { add_level_qual(current_qual, level_value); }
        ;

sign_attribute:
	  SIGN '=' POS
	  { add_sign_qual(current_qual, sign_pos); }
	| SIGN '=' NEG
	  { add_sign_qual(current_qual, sign_neg); }
	| SIGN '=' EQ
	  { add_sign_qual(current_qual, sign_eq); }
        ;

ptrflow_attribute:
	  PTRFLOW '=' UP
          { add_ptrflow_qual(current_qual, flow_up); }
        | PTRFLOW '=' DOWN
          { add_ptrflow_qual(current_qual, flow_down); }
	| PTRFLOW '=' ALL
          { add_ptrflow_qual(current_qual, flow_up);
            add_ptrflow_qual(current_qual, flow_down); }
        ;

fieldflow_attribute:
	  FIELDFLOW '=' UP
          { add_fieldflow_qual(current_qual, flow_up); }
	| FIELDFLOW '=' DOWN
          { add_fieldflow_qual(current_qual, flow_down); }
	| FIELDFLOW '=' ALL
          { add_fieldflow_qual(current_qual, flow_up);
            add_fieldflow_qual(current_qual, flow_down); }
        ;

fieldptrflow_attribute:
	  FIELDPTRFLOW '=' UP
          { add_fieldptrflow_qual(current_qual, flow_up); }
	| FIELDPTRFLOW '=' DOWN
          { add_fieldptrflow_qual(current_qual, flow_down); }
	| FIELDPTRFLOW '=' ALL
          { add_fieldptrflow_qual(current_qual, flow_up);
            add_fieldptrflow_qual(current_qual, flow_down); }
        ;

qual_order:
	  QUALIFIER '<' QUALIFIER
	  { qual q1, q2;

	    q1 = add_qual($1);
	    q2 = add_qual($3);
	    add_qual_lt(q1, q2);
	  }
        ;

%%

void lyyerror(char *string)
{
  fprintf(stderr, "%s:%d: %s\n", lattice_file, lattice_line, string);
}

void load_config_file_quals(const char *filename)
{
  extern void lyyrestart(FILE *);
  FILE *file;

  // region for lexer strings
  lattice_strings_region = newregion();

  file = fopen(filename, "r");
  if (!file)
    {
      fprintf(stderr, "Unable to open ``%s'': ", filename);
      perror("");
      exit(EXIT_FAILURE);
    }
  lattice_file = rstrdup(lattice_strings_region, filename);
  lattice_line = 1;
  lyyrestart(file);
  lyyparse();
  fclose(file);
  deleteregion(lattice_strings_region);
  lattice_file = NULL;
  extern void cleanup_lattice_parse(void);
  cleanup_lattice_parse();
}
