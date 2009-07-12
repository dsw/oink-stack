// exprequal.cc            see license.txt for copyright and terms of use
// compare expressions for structural equality

#include "exprequal.h"         // this module

#include "c.ast.gen.h"         // C AST stuff
#include "ohashtbl.h"          // OwnerHashTable
#include "c_type.h"            // Type::equals


// ------------------- VariablePair ---------------
struct VariablePair {
public:    // data
  Variable const *varL;      // hash key
  Variable const *varR;      // variable equivalent to varL

public:
  VariablePair(Variable const *vL, Variable const *vR)
    : varL(vL), varR(vR) {}
};

void const *variablePairKey(VariablePair *vp)
{
  return vp->varL;
}


// ---------------- expression comparison --------------
bool equalExpr(OwnerHashTable<VariablePair> &equiv,
               Expression const *left, Expression const *right);

bool equalExpressions(Expression const *left, Expression const *right)
{
  // map of variable equivalences, for determining equality of expressions
  // that locally declare variables (forall); initially empty
  OwnerHashTable<VariablePair> equiv(variablePairKey,
    HashTable::lcprngHashFn, HashTable::pointerEqualKeyFn);

  return equalExpr(equiv, left, right);
}


// simultaneous deconstruction..
bool equalExpr(OwnerHashTable<VariablePair> &equiv,
               Expression const *left, Expression const *right)
{
  // unlike in ML, I can do toplevel tag comparisons easily here
  if (left->kind() != right->kind()) {
    return false;
  }

  // I had been using _L as a variable name, but that causes problems
  // on some cygwin and solaris installations, so I changed it to LLL
  #define DOUBLECASEC(type)                       \
    ASTNEXTC(type, LLL)                           \
    type const &L = *LLL;                         \
    type const &R = *(right->as##type##C());

  // performance note: in general, I try to discover that the nodes are
  // not equal by examining data within the nodes, and only when that
  // data matches do I invoke the recursive call to equalExpr

  // since the tags are equal, a switch on the left's type also
  // analyzes right's type
  ASTSWITCHC(Expression, left) {
    ASTCASEC(E_intLit, LLL)     // note this expands to an "{" among other things
      E_intLit const &L = *LLL;
      E_intLit const &R = *(right->asE_intLitC());
      return L.i == R.i;

    DOUBLECASEC(E_floatLit)       return L.f == R.f;
    DOUBLECASEC(E_stringLit)      return L.s == R.s;
    DOUBLECASEC(E_charLit)        return L.c == R.c;

    DOUBLECASEC(E_variable)
      if (L.var == R.var) {
        return true;
      }

      // check the equivalence map
      VariablePair *vp = equiv.get(L.var);
      if (vp && vp->varR == R.var) {
        // this is the equivalent variable in the right expression,
        // so we say these variable references *are* equal
        return true;
      }
      else {
        // either L.var has no equivalence mapping, or else it does
        // but it's not equivalent to R.var
        return false;
      }

    DOUBLECASEC(E_funCall)
      if (L.args.count() != R.args.count() ||
          !equalExpr(equiv, L.func, R.func)) {
        return false;
      }
      ASTListIter<Expression> iterL(L.args);
      ASTListIter<Expression> iterR(R.args);
      for (; !iterL.isDone(); iterL.adv(), iterR.adv()) {
        if (!equalExpr(equiv, iterL.data(), iterR.data())) {
          return false;
        }
      }
      return true;

    DOUBLECASEC(E_fieldAcc)
      return L.fieldName == R.fieldName &&
             equalExpr(equiv, L.obj, R.obj);

    DOUBLECASEC(E_sizeof)
      return L.size == R.size;

    DOUBLECASEC(E_unary)
      return L.op == R.op &&
             equalExpr(equiv, L.expr, R.expr);

    DOUBLECASEC(E_effect)
      return L.op == R.op &&
             equalExpr(equiv, L.expr, R.expr);

    DOUBLECASEC(E_binary)
      // for now I don't consider associativity and commutativity;
      // I'll rethink this if I encounter code where it's helpful
      return L.op == R.op &&
             equalExpr(equiv, L.e1, R.e1) &&
             equalExpr(equiv, L.e2, R.e2);

    DOUBLECASEC(E_addrOf)
      return equalExpr(equiv, L.expr, R.expr);

    DOUBLECASEC(E_deref)
      return equalExpr(equiv, L.ptr, R.ptr);

    DOUBLECASEC(E_cast)
      return L.type->equals(R.type) &&
             equalExpr(equiv, L.expr, R.expr);

    DOUBLECASEC(E_cond)
      return equalExpr(equiv, L.cond, R.cond) &&
             equalExpr(equiv, L.th, R.th) &&
             equalExpr(equiv, L.el, R.el);

    DOUBLECASEC(E_comma)
      // I don't expect comma exprs in among those I'm comparing, but
      // may as well do the full comparison..
      return equalExpr(equiv, L.e1, R.e1) &&
             equalExpr(equiv, L.e2, R.e2);

    DOUBLECASEC(E_sizeofType)
      return L.size == R.size;

    DOUBLECASEC(E_assign)
      return L.op == R.op &&
             equalExpr(equiv, L.target, R.target) &&
             equalExpr(equiv, L.src, R.src);

    DOUBLECASEC(E_quantifier)
      if (L.forall != R.forall ||
          L.decls.count() != R.decls.count()) {
        return false;
      }

      // verify the declarations are structurally identical
      // (i.e. "int x,y;" != "int x; int y;") and all declared
      // variables have the same type
      bool ret;
      SObjList<Variable /*const*/> addedEquiv;
      ASTListIter<Declaration> outerL(L.decls);
      ASTListIter<Declaration> outerR(R.decls);
      for (; !outerL.isDone(); outerL.adv(), outerR.adv()) {
        if (outerL.data()->decllist.count() !=
            outerR.data()->decllist.count()) {
          ret = false;
          goto cleanup;
        }

        ASTListIter<Declarator> innerL(outerL.data()->decllist);
        ASTListIter<Declarator> innerR(outerR.data()->decllist);
        for (; !innerL.isDone(); innerL.adv(), innerR.adv()) {
          Variable const *varL = innerL.data()->var;
          Variable const *varR = innerR.data()->var;

          if (!varL->type->equals(varR->type)) {
            ret = false;     // different types
            goto cleanup;
          }

          // in expectation of all variables being of same type,
          // add these variables to the equivalence map
          equiv.add(varL, new VariablePair(varL, varR));       
          
          // keep track of what gets added
          addedEquiv.prepend(const_cast<Variable*>(varL));
        }
      }

      // when we get here, we know all the variables have the
      // same types, and the equiv map has been extended with the
      // equivalent pairs; so check equality of the bodies
      ret = equalExpr(equiv, L.pred, R.pred);

    cleanup:
      // remove the equivalences we added
      while (addedEquiv.isNotEmpty()) {
        delete equiv.remove(addedEquiv.removeFirst());
      }

      return ret;
             
    ASTDEFAULTC
      xfailure("bad expr tag");
      return false;   // silence warning

    ASTENDCASEC
  }
  
  #undef DOUBLECASEC
}


