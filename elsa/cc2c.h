// cc2c.h
// Given a C++ TranslationUnit, produce a C TranslationUnit.
// That is, compile C++ down into C.

// See cc2c.txt for some design notes.

#ifndef CC2C_H
#define CC2C_H

// Elsa
#include "cc_ast.h"          // C++ AST

// smbase
#include "okhashtbl.h"       // OwnerKHashTable
#include "ptrmap.h"          // PtrMap
#include "strtable.h"        // StringTable


// Main translator entry point.  Output tree is completely disjoint
// from the input tree, with the (optional) exception of the string
// table.  Also, the output tree is just (unambiguous) syntax; it is
// not decorated with types.  In contrast, the input tree must already
// be typechecked *and* elaborated (with cc_elaborate.{h,cc}, using
// all elaboration mechanisms).
TranslationUnit *cc_to_c(StringTable &str, TranslationUnit const &input);


// Translation environment.  Among other things, holds the output
// TranslationUnit while it is being built.
class CC2CEnv {
public:      // types 
  // bind a type to a name
  class TypedefEntry {
  public:    // data
    // Pointer to a Type object that appears in the input AST.
    Type *type;
    
    // Name generated for that type, in CC2CEnv::str.
    StringRef name;
    
  public:    // funcs
    // for OwnerKHashTable
    static Type const *getType(TypedefEntry *entry);
  };

public:      // data
  // StringTable for use by the created AST nodes.  Can be the
  // same as the input tree's table, but can also be different.
  StringTable &str;

  // Set of typedefs created so far, identified by the Type they
  // denote.
  OwnerKHashTable<TypedefEntry, Type> typedefs;
  
  // Set of structs/unions created so far.  The value is a
  // StringRef pointing into 'str'.
  PtrMap<CompoundType, char const> compoundTypes;

  // Current S_compound into which statements are being inserted.
  // Translation of expressions may need to introduce temporaries and
  // other pre-evaluation steps, and those go here.  This is NULL if
  // we're not in the middle of translating a function body.
  S_compound *curCompoundStmt;         // (nullable serf)

  // Entire in-progress output.
  TranslationUnit *dest;               // (owner)

public:      // funcs
  CC2CEnv(StringTable &str);
  ~CC2CEnv();

  // Return and nullify 'dest'.  
  TranslationUnit *takeDest();
  
  // Append a new topform.
  void addTopForm(TopForm *tf);

  // Append a new statement to the current compound statement.
  void addStatement(Statement *s);

  // Add a new declaration to either the current compound statement,
  // or as a topform if we're not inside a statement.
  void addDeclaration(Declaration *d);

  // Get a name for a type, creating a typedef if one has not
  // already been created.
  StringRef getTypeName(Type *t);
  StringRef getCompoundTypeName(CompoundType *ct);

  // Create a new TypeSpecifier that names type 't'.  This may
  // involve creating typedefs.
  TypeSpecifier *makeTypeSpecifier(Type *t);
  TypeSpecifier *makeAtomicTypeSpecifier(AtomicType *t);

  // Get a name for a variable.  Employs the name mangler.
  StringRef getVariableName(Variable const *v);

  // Make a PQ_name for a variable.
  PQ_name *makeName(Variable const *v);

  // Create a PQ_name.  Just encapsulates an allocation.
  PQ_name *makePQ_name(StringRef name);

  // Create a list of parameters suitable for inclusion in a D_func,
  // based on 'ft'.
  FakeList<ASTTypeId> *makeParameterTypes(FunctionType *ft);
};


// TODO: Add a visitor to check that a TranslationUnit only uses
// things that are legal in C.


#endif // CC2C_H
