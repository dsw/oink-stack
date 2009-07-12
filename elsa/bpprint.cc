// bbprint.cc
// code for bbprint.h and bpprint.ast

#include "bpprint.h"         // this module

// elsa
#include "cc_ast.h"          // AST


// Expands to a 'for' loop that inserts a box begin and end, and
// executes the body of the loop exactly once.  Should be followed by
// a brace-enclosed block that prints the things inside the box.
//
// Candidate for promotion to boxprint.h.
#define MAKE_BOX(boxKind)                                            \
  for (int pass=1;                                                   \
       pass==1? (bp << bp.boxKind, true) : (bp << bp.end, false);    \
       pass++)

#define VBOX MAKE_BOX(vert)
#define SEQ MAKE_BOX(seq)
#define HV MAKE_BOX(hv)


static void printCVFlags(BPEnv &bp, CVFlags cv)
{
  #define PRINT_FLAG(flag, name)   \
    if (cv & flag) {               \
      bp << bp.sp << name;         \
    }
  PRINT_FLAG(CV_CONST, "const");
  PRINT_FLAG(CV_VOLATILE, "volatile");
  PRINT_FLAG(CV_RESTRICT, "restrict");
  #undef PRINT_FLAG
}


static void printArgExpressions(BPEnv &bp, FakeList<ArgExpression> *args)
{
  bp << "(";
  SEQ {
    int ct=0;
    FAKELIST_FOREACH(ArgExpression, args, iter) {
      if (ct++ > 0) {
        bp << "," << bp.br;
      }
      iter->expr->printExpPA(bp, 8 /*function argument list "," prec/assoc*/);
    }
  }
  bp << ")";
}


static void printDeclFlags(BPEnv &bp, DeclFlags dflags, 
                           DeclFlags suppress = DF_NONE)
{
  dflags &= ~suppress;

  SEQ {
    #define HANDLE_FLAG(DF_FLAG, flagString)  \
      if (dflags & DF_FLAG) {                 \
        bp << flagString << bp.sp;            \
      }
    HANDLE_FLAG(DF_AUTO, "auto")
    HANDLE_FLAG(DF_REGISTER, "register")
    HANDLE_FLAG(DF_STATIC, "static")
    HANDLE_FLAG(DF_EXTERN, "extern")
    HANDLE_FLAG(DF_MUTABLE, "mutable")
    HANDLE_FLAG(DF_INLINE, "inline")
    HANDLE_FLAG(DF_VIRTUAL, "virtual")
    HANDLE_FLAG(DF_EXPLICIT, "explicit")
    HANDLE_FLAG(DF_FRIEND, "friend")
    HANDLE_FLAG(DF_TYPEDEF, "typedef")
    #undef HANDLE_FLAG
  }
}

 
// --------------------- TranslationUnit ----------------------
static void printTopForms(BPEnv &bp, ASTList<TopForm> const &topForms)
{
  bp << bp.br;
  FOREACH_ASTLIST(TopForm, topForms, iter) {
    iter.data()->print(bp);
    bp << bp.br << bp.br;
  }
}

void TranslationUnit::print(BPEnv &bp) const
{
  printTopForms(bp, topForms);
}


// ------------------------ TopForm ---------------------------
void TF_decl::print(BPEnv &bp) const
{
  decl->print(bp);
}

void TF_func::print(BPEnv &bp) const
{
  f->print(bp);
}

void TF_template::print(BPEnv &bp) const
{
  td->print(bp);
}

void TF_explicitInst::print(BPEnv &bp) const
{
  SEQ {
    printDeclFlags(bp, instFlags);
    d->print(bp);
  }
}

void TF_linkage::print(BPEnv &bp) const
{
  VBOX {
    bp << "extern " << linkageType << bp.sp << "{";
    bp << bp.ind;
    forms->print(bp);
    bp << bp.und;
    bp << "}";
  }
}

void TF_one_linkage::print(BPEnv &bp) const
{
  bp << "extern " << linkageType << bp.sp;
  form->print(bp);
}

void TF_asm::print(BPEnv &bp) const
{
  bp << "asm(";
  text->print(bp);
  bp << ")";
}

void TF_namespaceDefn::print(BPEnv &bp) const
{
  VBOX {
    bp << "namespace ";
    if (name) {
      bp << name << " ";
    }
    bp << "{" << bp.br;

    // it is deliberate to not print these with indentation
    printTopForms(bp, forms);
    
    bp << "} // namespace " << (name? name : "(anonymous)");
  }
}

void TF_namespaceDecl::print(BPEnv &bp) const
{
  decl->print(bp);
}


// ------------------------ Function -------------------------
// print 'spec' and a space, unless it's an odd case where
// we don't want to print anything
static void printTypeSpecifier(BPEnv &bp, TypeSpecifier const *spec)
{
  if (spec->isTS_simple() &&
      spec->asTS_simpleC()->id == ST_CDTOR) {
    // do not print it
  }
  else {
    spec->print(bp);
    bp << bp.sp;
  }
}


static void printHandlers(BPEnv &bp, FakeList<Handler> *handlers)
{
  FAKELIST_FOREACH(Handler, handlers, iter) {
    bp << bp.br;
    iter->print(bp);
  }
}


void Function::print(BPEnv &bp, bool inClassDefn) const
{
  // whole function
  VBOX {
    // header
    SEQ {
      printDeclFlags(bp, dflags, (inClassDefn? DF_INLINE : DF_NONE));
      printTypeSpecifier(bp, retspec);
      nameAndParams->print(bp);
    }

    if (inits) {
      bp << bp.ind;
      bp << ":" << bp.sp;
      VBOX {
        int ct=0;
        FAKELIST_FOREACH(MemberInit, inits, iter) {
          if (ct++ > 0) {
            bp << "," << bp.br;
          }
          iter->print(bp);
        }
      }
      bp << bp.und;
    }
    else {
      bp << bp.br;
    }

    if (handlers) {
      bp << "try" << bp.br;
    }

    body->print(bp);

    if (handlers) {
      printHandlers(bp, handlers);
    }
  }
}


// --------------------- MemberInit ------------------------
void MemberInit::print(BPEnv &bp) const
{
  SEQ {
    name->print(bp);
    printArgExpressions(bp, args);
  }
}


// --------------------- Declaration -----------------------
void Declaration::print(BPEnv &bp) const
{
  SEQ {
    printDeclFlags(bp, dflags);
    if (!decllist) {
      // don't print the space after the TS
      spec->print(bp);
    }
    else {
      printTypeSpecifier(bp, spec);
    }

    int ct=0;
    FAKELIST_FOREACH(Declarator, decllist, iter) {
      if (ct++ > 0) {
        bp << "," << bp.br;
      }
      iter->print(bp);
    }

    bp << ";";
  }
}


// --------------------- ASTTypeId --------------------------
void ASTTypeId::print(BPEnv &bp) const
{
  SEQ {
    spec->print(bp);
    bp << bp.br;
    decl->print(bp);
  }
}


// ---------------------- PQName ---------------------------
static void printOpenAngleBracket(BPEnv &bp)
{
  char c = bp.getLastChar();
  if (c == '<' || c == '>') {
    // prevent '<'s from coalescing, e.g.:
    //   int operator<< <>(...);
    // also space for '>' just to avoid general confusion:
    //   int operator>> <>(...);
    bp << bp.sp;
  }

  bp << "<";
}

static void printCloseAngleBracket(BPEnv &bp)
{
  if (bp.getLastChar() == '>') {
    // prevent '>'s from coalescing
    bp << bp.sp;
  }

  bp << ">";
}

static void maybeTemplateUsed(BPEnv &bp, TemplateArgument const *templArgs)
{
  if (templArgs && templArgs->isTA_templateUsed()) {
    bp << "template ";
  }
}

static void printTemplateArguments(BPEnv &bp, TemplateArgument const *templArgs)
{
  if (templArgs && templArgs->isTA_templateUsed()) {
    templArgs = templArgs->next;
  }
  
  printOpenAngleBracket(bp);

  SEQ {
    for (TemplateArgument const *ta = templArgs;
         ta;
         ta = ta->next) {
      if (ta != templArgs) {
        bp << "," << bp.br;
      }
      ta->print(bp);
    }
  }

  printCloseAngleBracket(bp);
}

void PQ_qualifier::print(BPEnv &bp) const
{
  SEQ {
    maybeTemplateUsed(bp, templArgs);
    if (qualifier) {
      bp << qualifier;
    }
    if (templArgs) {
      printTemplateArguments(bp, templArgs);
    }
    bp << "::";
    rest->print(bp);
  }
}

void PQ_name::print(BPEnv &bp) const
{
  bp << name;
}

void PQ_operator::print(BPEnv &bp) const
{
  SEQ {
    bp << "operator";
    if (!o->isON_operator()) {
      // I like the operators next to "operator", but for converions
      // and new/delete there must be a space.
      bp << bp.sp;
    }
    o->print(bp);
  
    // I normally like a space before the "()" of an operator
    // function name.
    bp << bp.sp;
  }
}

void PQ_template::print(BPEnv &bp) const
{
  SEQ {
    maybeTemplateUsed(bp, templArgs);
    bp << name;
    printTemplateArguments(bp, templArgs);
  }
}

void PQ_variable::print(BPEnv &bp) const
{
  // hmmm...
  bp << var->fullyQualifiedName0();
}


// -------------------- ASTTypeof ---------------------------
void TS_typeof_expr::print(BPEnv &bp) const
{
  expr->print(bp);
}

void TS_typeof_type::print(BPEnv &bp) const
{
  atype->print(bp);
}


// ------------------- TypeSpecifier ------------------------
void TypeSpecifier::print(BPEnv &bp) const
{
  iprint(bp);
  printCVFlags(bp, cv);
}

void TS_name::iprint(BPEnv &bp) const
{
  if (typenameUsed) {
    bp << "typename" << bp.sp;
  }
  name->print(bp);
}


void TS_simple::iprint(BPEnv &bp) const
{
  bp << simpleTypeName(id);
}


void TS_elaborated::iprint(BPEnv &bp) const
{
  bp << toString(keyword) << bp.sp;
  name->print(bp);
}

void TS_classSpec::iprint(BPEnv &bp) const
{
  // This doesn't use a VBOX because I want the members' indents to
  // not necessary be relative to the 'struct' keyword, for examples
  // like in/t0010.cc where there is a typedef keyword in front.
  SEQ {
    bp << toString(keyword);
    if (name) {
      bp << bp.sp;
      name->print(bp);
    }

    if (bases->isNotEmpty()) {
      bp << bp.sp << ":" << bp.sp;
      HV {
        int ct=0;
        FAKELIST_FOREACH(BaseClassSpec, bases, iter) {
          if (ct++ > 0) {
            bp << "," << bp.br;
          }
          iter->print(bp);
        }
      }
    }

    bp << bp.sp << "{";
  }

  bp << bp.ind;

  if (members->list.isEmpty() ||
      members->list.firstC()->isMR_access()) {
    // little hack: if no members , or the first member is MR_access,
    // don't force the newline, b/c it makes an extra one
  }
  else {
    bp << bp.fbr;
  }

  int ct=0;
  FOREACH_ASTLIST(Member, members->list, iter) {
    if (ct++ > 0) {
      bp << bp.fbr;
    }
    iter.data()->print(bp);
  }
  bp << bp.und << bp.fbr << "}";
}


void TS_enumSpec::iprint(BPEnv &bp) const
{
  VBOX {
    SEQ {
      bp << "enum" << bp.sp;
      if (name) {
        bp << name << bp.sp;
      }
      bp << "{";
    }
    bp << bp.ind;
    int ct=0;
    FAKELIST_FOREACH(Enumerator, elts, iter) {
      if (ct++ > 0) {
        bp << "," << bp.br;
      }
      iter->print(bp);
    }
    bp << bp.und << "}";
  }
}

void TS_typeof::iprint(BPEnv &bp) const
{
  bp << "typeof(";
  atype->print(bp);
  bp << ")";
}

void TS_type::iprint(BPEnv &bp) const
{
  // hmmm...
  bp << type->toString();
}


// ------------------- BaseClassSpec ------------------------
void BaseClassSpec::print(BPEnv &bp) const
{
  SEQ {
    if (isVirtual) {
      bp << "virtual ";
    }
    bp << toString(access) << bp.sp;
    name->print(bp);
  }
}


// --------------------- Member -----------------------------
void MR_decl::print(BPEnv &bp) const
{
  d->print(bp);
}

void MR_func::print(BPEnv &bp) const
{
  f->print(bp, true /*inClassDefn*/);
}

void MR_access::print(BPEnv &bp) const
{
  bp << bp.und << bp.fbr << toString(k) << ":" << bp.ind;
}

void MR_usingDecl::print(BPEnv &bp) const
{
  decl->print(bp);
}

void MR_template::print(BPEnv &bp) const
{
  d->print(bp);
}


// ------------------- Enumerator ---------------------------
void Enumerator::print(BPEnv &bp) const
{
  SEQ {
    bp << name;
    if (expr) {
      bp << bp.sp << "=" << bp.sp;
      expr->print(bp);
    }
  }
}


// ------------------- Declarator ---------------------------
void Declarator::print(BPEnv &bp) const
{
  decl->print(bp);
  
  if (init) {
    if (init->isIN_ctor()) {
      if (init->asIN_ctor()->args->isEmpty()) {
        // proper syntax for a call to a ctor with no arguments
        // in a declarator is no parens, because with parens it
        // looks like a function prototype
      }
      else {            
        // no "="
        init->print(bp);
      }
    }
    else {
      if (bp.getLastChar() == ' ') {
        // e.g.: int f(int = 5);
      }
      else {
        bp << bp.sp;
      }
      bp << "=" << bp.sp;
      init->print(bp);
    }
  }
}


// ------------------- IDeclarator --------------------------
void D_name::print(BPEnv &bp) const
{
  if (name) {
    name->print(bp);
  }
}


void D_pointer::print(BPEnv &bp) const
{
  bp << "*";
  if (cv) {
    printCVFlags(bp, cv);
    bp << bp.sp;
  }
  base->print(bp);
}


void D_reference::print(BPEnv &bp) const
{
  bp << "&";
  base->print(bp);
}


void D_func::print(BPEnv &bp) const
{
  SEQ {
    base->print(bp);
    bp << "(";
    SEQ {
      int ct=0;
      FAKELIST_FOREACH(ASTTypeId, params, iter) {
        if (ct++ > 0) {
          bp << "," << bp.br;
        }
        iter->print(bp);
      }
    }
    bp << ")";

    printCVFlags(bp, cv);

    if (exnSpec) {
      bp << bp.sp;
      exnSpec->print(bp);
    }
  }
}


void D_array::print(BPEnv &bp) const
{
  SEQ {
    base->print(bp);
    bp << "[";
    if (size) {
      size->print(bp);
    }
    bp << "]";
  }
}


void D_bitfield::print(BPEnv &bp) const
{
  SEQ {
    if (name) {
      name->print(bp);
      bp << bp.sp;
    }   
    bp << ":" << bp.sp;
    bits->print(bp);
  }
}

void D_ptrToMember::print(BPEnv &bp) const
{      
  SEQ {
    nestedName->print(bp);
    bp << "::*";
    printCVFlags(bp, cv);
    if (cv) {
      bp << bp.sp;
    }
    base->print(bp);
  }
}

void D_grouping::print(BPEnv &bp) const
{
  bp << "(";
  base->print(bp);
  bp << ")";
}


// ----------------- ExceptionSpec -------------------------
void ExceptionSpec::print(BPEnv &bp) const
{
  SEQ {
    bp << "throw(";
    SEQ {
      int ct=0;
      FAKELIST_FOREACH(ASTTypeId, types, iter) {
        if (ct++ > 0) {
          bp << "," << bp.br;
        }
        iter->print(bp);
      }
    }
    bp << ")";
  }
}


// ------------------ OperatorName -------------------------
void ON_newDel::print(BPEnv &bp) const
{
  SEQ {
    if (isNew) {
      bp << "new";
    }
    else {
      bp << "delete";
    }
    if (isArray) {
      bp << "[]";
    }
  }
}

void ON_operator::print(BPEnv &bp) const
{
  bp << toString(op);
}

void ON_conversion::print(BPEnv &bp) const
{
  type->print(bp);
}


// ------------------- Statement ---------------------------
void Statement::print(BPEnv &bp) const
{
  iprint(bp);
}

void S_skip::iprint(BPEnv &bp) const
{
  bp << ";";
}

void S_label::iprint(BPEnv &bp) const
{
  // pull labels out to the left
  bp << bp.und;
  SEQ {
    bp << name << ":";
  }
  bp << bp.ind;

  s->print(bp);
}

void S_case::iprint(BPEnv &bp) const
{
  bp << bp.und;
  SEQ {
    bp << "case" << bp.sp;
    expr->print(bp);
    bp << ":";
  }
  bp << bp.ind;

  s->print(bp);
}

void S_default::iprint(BPEnv &bp) const
{
  bp << bp.und;
  bp << "default:";
  bp << bp.ind;

  s->print(bp);
}

void S_expr::iprint(BPEnv &bp) const
{
  SEQ {
    expr->print(bp);
    bp << ";";
  }
}

void S_compound::iprint(BPEnv &bp) const
{
  if (stmts.isEmpty()) {
    bp << "{}";
  }
  else {
    VBOX {
      bp << "{" << bp.ind;
      printNoBraces(bp);
      bp << bp.und << "}";
    }
  }
}

void S_compound::printNoBraces(BPEnv &bp) const
{
  int ct=0;
  FOREACH_ASTLIST(Statement, stmts, iter) {
    if (ct++ > 0) {
      bp << bp.br;
    }
    iter.data()->print(bp);
  }
}

static void printNoBraces(BPEnv &bp, Statement *s)
{
  if (s->isS_compound()) {
    s->asS_compound()->printNoBraces(bp);
  }
  else {
    s->print(bp);
  }
}

// Since the type checker ends up inserting S_compounds into both
// branches of if/else etc., format the printing on the assumption
// that they are S_compund.  It won't be semantically wrong for
// non-S_compound, but it may print unneeded braces.  Extending the
// printer to print non-S_compounds w/o the braces is possible but not
// worth it right now.
void S_if::iprint(BPEnv &bp) const
{
  VBOX {
    SEQ {
      bp << "if (";
      cond->print(bp);
      bp << ") {";
    }
    bp << bp.ind;
    printNoBraces(bp, thenBranch);
    bp << bp.und;
    bp << "}" << bp.br;
    bp << "else {" << bp.ind;
    printNoBraces(bp, elseBranch);
    bp << bp.und << "}";
  }
}

void S_switch::iprint(BPEnv &bp) const
{
  VBOX {
    SEQ {
      bp << "switch (";
      cond->print(bp);
      bp << ") {";
    }
    
    // indent the body 2x so cases come out one level
    // but are still under the switch
    bp << bp.ibr(bp.levelIndent * 2);
    printNoBraces(bp, branches);
    bp << bp.ibr(-bp.levelIndent * 2) << "}";
  }
}

void S_while::iprint(BPEnv &bp) const
{
  VBOX {
    SEQ {
      bp << "while (";
      cond->print(bp);
      bp << ") {";
    }
    bp << bp.ind;
    printNoBraces(bp, body);
    bp << bp.und << "}";
  }
}

void S_doWhile::iprint(BPEnv &bp) const
{
  VBOX {
    bp << "do {" << bp.ind;
    printNoBraces(bp, body);
    bp << bp.und;
    SEQ {
      bp << "} while (";
      expr->print(bp);
      bp << ");";
    }
  }
}

void S_for::iprint(BPEnv &bp) const
{
  VBOX {
    SEQ {
      bp << "for (";
      HV {
        SEQ {
          init->print(bp);
          // note that 'init' prints its own semicolon
        }
        bp << bp.br;
        SEQ {
          cond->print(bp);
          bp << ";";
        }
        bp << bp.br;
        SEQ {
          after->print(bp);
        }
      }
      bp << ") {";
    }
    bp << bp.ind;
    printNoBraces(bp, body);
    bp << bp.und << "}";
  }
}

void S_break::iprint(BPEnv &bp) const
{
  bp << "break;";
}

void S_continue::iprint(BPEnv &bp) const
{
  bp << "continue;";
}

void S_return::iprint(BPEnv &bp) const
{
  SEQ {
    bp << "return";
    if (expr) {
      bp << bp.sp;
      expr->print(bp);
    }
    bp << ";";
  }
}

void S_goto::iprint(BPEnv &bp) const
{
  SEQ {
    bp << "goto" << bp.sp << target << ";";
  }
}

void S_decl::iprint(BPEnv &bp) const
{
  decl->print(bp);
}

void S_try::iprint(BPEnv &bp) const
{
  VBOX {
    bp << "try {" << bp.ind;
    printNoBraces(bp, body);
    bp << bp.und << "}";    
    printHandlers(bp, handlers);
  }
}

void S_asm::iprint(BPEnv &bp) const
{
  SEQ {
    bp << "asm" << bp.br;
    text->print(bp);
    bp << ";";
  }
}

void S_namespaceDecl::iprint(BPEnv &bp) const
{
  decl->print(bp);
}

void S_computedGoto::iprint(BPEnv &bp) const
{
  bp << "goto *";
  target->printExpPA(bp, 135 /* just above unary '*' in precedence */);
}

void S_rangeCase::iprint(BPEnv &bp) const
{
  bp << "case ";
  exprLo->print(bp);
  bp << " ... ";
  exprHi->print(bp);
  bp << ":";

  bp << bp.br;
  s->print(bp);
}

void S_function::iprint(BPEnv &bp) const
{
  f->print(bp);
}


// ------------------- Condition --------------------
void CN_expr::print(BPEnv &bp) const
{
  expr->print(bp);
}

void CN_decl::print(BPEnv &bp) const
{
  typeId->print(bp);
}


// ------------------- Handler ----------------------
void Handler::print(BPEnv &bp) const
{
  VBOX {
    SEQ {
      bp << "catch (";
      typeId->print(bp);
      bp << ") {";
    }
    bp << bp.ind;
    printNoBraces(bp, body);
    bp << bp.und << "}";
  }
}


// ---------------- FullExpression ------------------
void FullExpression::print(BPEnv &bp) const
{
  expr->print(bp);
}


// ---------------- ArgExpression ------------------
void ArgExpression::print(BPEnv &bp) const
{
  expr->print(bp);
}


// ----------------- Expression ---------------------
static int getEffectPrecAssoc(EffectOp op)
{
  switch (op) {
    case EFF_POSTINC:
    case EFF_POSTDEC:
      return 140;

    case EFF_PREINC:
    case EFF_PREDEC:
      return -130;

    case NUM_EFFECTOPS:
      break;
  }

  xfailure("bad effect op");
  return 0;
}

static int getBinaryPrecAssoc(BinaryOp op)
{
  switch (op) {
    case BIN_EQUAL:
    case BIN_NOTEQUAL:
      return 70;

    case BIN_LESS:
    case BIN_GREATER:
    case BIN_LESSEQ:
    case BIN_GREATEREQ:
      return 80;

    case BIN_MULT:
    case BIN_DIV:
    case BIN_MOD:
      return 110;

    case BIN_PLUS:
    case BIN_MINUS:
      return 100;

    case BIN_LSHIFT:
    case BIN_RSHIFT:
      return 90;

    case BIN_BITAND:
      return 60;

    case BIN_BITXOR:
      return 50;

    case BIN_BITOR:
      return 40;

    case BIN_AND:
      return 30;

    case BIN_OR:
      return 20;

    case BIN_COMMA:
      return 7;

    case BIN_MINIMUM:
    case BIN_MAXIMUM:
      return 80;    // from gnu.gr

    case BIN_BRACKETS:
      return 140;

    case BIN_ASSIGN:
      return -9;

    case BIN_DOT_STAR:
    case BIN_ARROW_STAR:
      return 120;

    case BIN_IMPLIES:
    case BIN_EQUIVALENT:
      return 15;   // random

    case NUM_BINARYOPS:
      break;
  }        

  xfailure("bad binary op");
  return 0;
}

// Return the prec/assoc of the given Expression, where a negative
// number means right-assoc and precedence is given by the absolute
// value.  The actual values come from the prec/assoc table in cc.gr.
static int getPrecAssoc(Expression const *e)
{
  switch (e->kind()) {
    case Expression::E_FUNCALL:
    case Expression::E_ARROW:
    case Expression::E_FIELDACC:
      return 140;

    case Expression::E_EFFECT:
      return getEffectPrecAssoc(e->asE_effectC()->op);

    case Expression::E_UNARY:
    case Expression::E_DEREF:
    case Expression::E_ADDROF:
    case Expression::E_CAST:
    case Expression::E_SIZEOF:
      return -130;

    case Expression::E_BINARY:
      return getBinaryPrecAssoc(e->asE_binaryC()->op);

    default:
      // remaining cases shouldn't have prec/assoc consequences
      return 0;
  }
}


void Expression::printSubexp(BPEnv &bp, Expression const *subexp,
                             bool isRightChild) const
{
  subexp->printExpPA(bp, getPrecAssoc(this), isRightChild);
}

void Expression::printExpPA(BPEnv &bp, int parentPA,
                            bool isRightChild) const
{
  int childPA = getPrecAssoc(this);

  int parentPrec = abs(parentPA);
  int childPrec = abs(childPA);

  bool needsParens = false;

  if (parentPrec == 0 || childPrec == 0) {
    // no parens should be necessary
  }
  else if (parentPrec < childPrec) {
    // child has higher prec, again no parens
  }
  else if (parentPrec > childPrec) {
    // parent has higher prec, child needs parens
    needsParens = true;
  }
  else {
    // equal precedence, check the associativity
    if ((parentPA < 0) == isRightChild) {
      // parent associates to the right and it's a right child,
      // or parent associates to the left and it's a left child;
      // either way, no parens should be necessary
    }
    else {
      // mismatch; parens needed
      needsParens = true;
    }
  }

  if (needsParens) {
    SEQ {
      bp << "(";
      print(bp);
      bp << ")";
    }
  }
  else {
    print(bp);
  }
}


void Expression::print(BPEnv &bp) const
{
  iprint(bp);
}


void E_boolLit::iprint(BPEnv &bp) const
{
  bp << (b? "true" : "false");
}

void E_intLit::iprint(BPEnv &bp) const
{
  bp << text;
}

void E_floatLit::iprint(BPEnv &bp) const
{
  bp << text;
}

void E_stringLit::iprint(BPEnv &bp) const
{
  SEQ {
    bp << text;
    for (E_stringLit const *c = continuation;
         c;
         c = c->continuation) {
      bp << bp.br << c->text;
    }
  }
}

void E_charLit::iprint(BPEnv &bp) const
{
  bp << text;
}

void E_this::iprint(BPEnv &bp) const
{
  bp << "this";
}

void E_variable::iprint(BPEnv &bp) const
{
  name->print(bp);
}

void E_funCall::iprint(BPEnv &bp) const
{
  SEQ {
    printSubexpL(bp, func);
    printArgExpressions(bp, args);
  }
}

void E_constructor::iprint(BPEnv &bp) const
{
  SEQ {
    spec->print(bp);
    printArgExpressions(bp, args);
  }
}

void E_fieldAcc::iprint(BPEnv &bp) const
{
  SEQ {
    printSubexpL(bp, obj);
    bp << ".";
    fieldName->print(bp);
  }
}

void E_sizeof::iprint(BPEnv &bp) const
{
  SEQ {
    bp << "sizeof(";
    expr->print(bp);
    bp << ")";
  }
}

void E_unary::iprint(BPEnv &bp) const
{
  SEQ {
    bp << toString(op);
    printSubexpR(bp, expr);
  }
}

void E_effect::iprint(BPEnv &bp) const
{
  SEQ {
    bool postfix = isPostfix(op);
    if (!postfix) {
      bp << toString(op);
    }
    printSubexp(bp, expr, !postfix /*isRightChild*/);
    if (postfix) {
      bp << toString(op);
    }
  }
}

void E_binary::iprint(BPEnv &bp) const
{
  SEQ {
    printSubexpL(bp, e1);
    if (op == BIN_COMMA) {
      // comma usually printed differently
      bp << toString(op) << bp.br;
    }
    else if (op == BIN_BRACKETS) {
      bp << "[";
      printSubexpR(bp, e2);
      bp << "]";
      continue;    // get the final part of the SEQ (hack...)
    }
    else {
      bp << bp.op(toString(op));
    }
    printSubexpR(bp, e2);
  }
}

void E_addrOf::iprint(BPEnv &bp) const
{
  SEQ {
    bp << "&";
    printSubexpR(bp, expr);
  }
}

void E_deref::iprint(BPEnv &bp) const
{
  SEQ {
    bp << "*";
    printSubexpR(bp, ptr);
  }
}

void E_cast::iprint(BPEnv &bp) const
{
  bp << "(";
  ctype->print(bp);
  bp << ")" << bp.br;
  printSubexpR(bp, expr);
}

void E_cond::iprint(BPEnv &bp) const
{
  SEQ {
    printSubexpL(bp, cond);
    bp << bp.op("?");
    SEQ {
      printSubexpL(bp, th);    // L or R?  not sure ...
      bp << bp.op(":");
      printSubexpR(bp, el);
    }
  }
}

void E_sizeofType::iprint(BPEnv &bp) const
{
  SEQ {
    bp << "sizeof(";
    atype->print(bp);
    bp << ")";
  }
}

void E_assign::iprint(BPEnv &bp) const
{
  SEQ {
    printSubexpL(bp, target);
    bp << bp.br;
    if (op == BIN_ASSIGN) {
      bp << "=";
    }
    else {
      bp << toString(op) << "=";
    }
    bp << bp.br;
    printSubexpR(bp, src);
  }
}


void E_new::iprint(BPEnv &bp) const
{
  SEQ {
    if (colonColon) {
      bp << "::";
    }
    bp << "new" << bp.br;
    if (placementArgs->isNotEmpty()) {
      printArgExpressions(bp, placementArgs);
      bp << bp.br;
    }
    atype->print(bp);
    if (ctorArgs) {
      printArgExpressions(bp, ctorArgs->list);
      bp << bp.br;
    }
  }
}


void E_delete::iprint(BPEnv &bp) const
{
  SEQ {
    if (colonColon) {
      bp << "::";
    }
    bp << "delete";
    if (array) {
      bp << "[]";
    }
    bp << bp.sp;
    printSubexpR(bp, expr);
  }
}


void E_throw::iprint(BPEnv &bp) const
{
  if (expr) {
    SEQ {
      bp << "throw" << bp.br;
      printSubexpR(bp, expr);
    }
  }
  else {
    bp << "throw";
  }
}

void E_keywordCast::iprint(BPEnv &bp) const
{
  SEQ {
    bp << toString(key) << "<";
    ctype->print(bp);
    bp << ">(";
    expr->print(bp);
    bp << ")";
  }
}

void E_typeidExpr::iprint(BPEnv &bp) const
{
  SEQ {
    bp << "typeid(";
    expr->print(bp);
    bp << ")";
  }
}

void E_typeidType::iprint(BPEnv &bp) const
{
  SEQ {
    bp << "typeid(";
    ttype->print(bp);
    bp << ")";
  }
}

void E_grouping::iprint(BPEnv &bp) const
{
  SEQ {
    bp << "(";
    expr->print(bp);
    bp << ")";
  }
}

void E_arrow::iprint(BPEnv &bp) const
{
  SEQ {
    printSubexpL(bp, obj);
    bp << "->";
    fieldName->print(bp);
  }
}

void E_addrOfLabel::iprint(BPEnv &bp) const
{
  bp << "&&" << labelName;
}

void E_gnuCond::iprint(BPEnv &bp) const
{
  printSubexpL(bp, cond);
  printSubexpR(bp, el);
}

void E_alignofExpr::iprint(BPEnv &bp) const
{
  bp << "alignof(";
  expr->print(bp);
  bp << ")";
}

void E_alignofType::iprint(BPEnv &bp) const
{
  bp << "alignof(";
  atype->print(bp);
  bp << ")";
}

void E___builtin_va_arg::iprint(BPEnv &bp) const
{
  bp << "__builtin_va_arg(";
  SEQ {
    expr->print(bp);
    bp << "," << bp.br;
    atype->print(bp);
  }
  bp << ")";
}

void E___builtin_constant_p::iprint(BPEnv &bp) const
{
  bp << "__builtin_constant_p(";
  expr->print(bp);
  bp << ")";
}

void E_compoundLit::iprint(BPEnv &bp) const
{
  bp << "(";
  stype->print(bp);
  bp << ")";
  init->print(bp);
}

void E_statement::iprint(BPEnv &bp) const
{
  bp << "({";
  HV {
    bp << bp.ind;
    s->printNoBraces(bp);
    bp << bp.und;
  }
  bp << "})";
}


// ----------------- Initializer -----------------------
void IN_expr::print(BPEnv &bp) const
{
  e->print(bp);
}

void IN_compound::print(BPEnv &bp) const
{
  bp << "{";
  HV {
    bp << bp.ind;
    int ct=0;
    FOREACH_ASTLIST(Initializer, inits, iter) {
      if (ct++ > 0) {
        bp << "," << bp.br;
      }
      iter.data()->print(bp);
    }
    bp << bp.und;
  }
  bp << "}";
}

void IN_ctor::print(BPEnv &bp) const
{
  SEQ {
    printArgExpressions(bp, args);
  }
}

void IN_designated::print(BPEnv &bp) const
{
  SEQ {               
    int ct=0;
    FAKELIST_FOREACH(Designator, designator_list, iter) {
      if (ct++ > 0) {
        bp << "." << bp.br;
      }
      iter->print(bp);
    }
    bp << bp.op("=");
    init->print(bp);
  }
}


// ------------------ Designator -----------------------
void FieldDesignator::print(BPEnv &bp) const
{
  bp << id;
}

void SubscriptDesignator::print(BPEnv &bp) const
{
  bp << "[";
  idx_expr->print(bp);
  if (idx_expr2) {
    bp << " ... ";
    idx_expr2->print(bp);
  }
  bp << "]";
}


// -------------- TemplateDeclaration ------------------
static void printTemplateParameters(BPEnv &bp, TemplateParameter const *params)
{
  bp << "template <";
  HV {
    for (TemplateParameter const *tp = params;
         tp;
         tp = tp->next) {
      if (tp != params) {
        bp << "," << bp.br;
      }
      tp->print(bp);
    }
  }
  printCloseAngleBracket(bp);
}


void TemplateDeclaration::print(BPEnv &bp) const
{
  VBOX {
    printTemplateParameters(bp, params);
    bp << bp.br;
    iprint(bp);
  }
}

void TD_func::iprint(BPEnv &bp) const
{
  f->print(bp);
}

void TD_decl::iprint(BPEnv &bp) const
{
  d->print(bp);
}

void TD_tmember::iprint(BPEnv &bp) const
{
  d->print(bp);
}


// --------------- TemplateParameter -------------------
void TP_type::print(BPEnv &bp) const
{
  SEQ {
    // I realize that "typename" is equivalent and arguably more
    // correct b/c a type argument need not be a class.  However,
    // "class" is the most common syntax in use, and hence has more
    // idiomatic recognition, so I prefer to use it.
    bp << "class";

    if (name) {
      bp << bp.br << name;
    }
    if (defaultType) {
      bp << bp.op("=");
      defaultType->print(bp);
    }
  }
}

void TP_nontype::print(BPEnv &bp) const
{
  param->print(bp);
}

void TP_template::print(BPEnv &bp) const
{
  SEQ {
    printTemplateParameters(bp, parameters->firstC());

    if (name) {
      bp << bp.br << name;
    }
    
    if (defaultTemplate) {
      bp << bp.op("=");
      defaultTemplate->print(bp);
    }
  }
}


// --------------- TemplateArgument --------------------
void TA_type::print(BPEnv &bp) const
{
  type->print(bp);
}

void TA_nontype::print(BPEnv &bp) const
{                          
  // 95 is just above the precedence of '>>' and '>'.  This means that
  // 'expr' will get parentheses if it uses either operator, or any
  // operators with same or lower precedence.  This is slightly more
  // restrictive than what the language allows, but most of the time
  // will not print extra parens.  (Incidentally, IMO, the language
  // designers *should* have included a rule like this, as it would
  // significantly simplify parsing and not be a burden to users.  Oh
  // well.)
  expr->printExpPA(bp, 95);
}

void TA_templateUsed::print(BPEnv &bp) const
{                                                        
  // The caller should have handled it.
  xfailure("should not print TA_templateUsed directly");
}


// ---------------- NamespaceDecl ----------------------
void ND_alias::print(BPEnv &bp) const
{
  bp << "namespace " << alias << " = ";
  original->print(bp);
  bp << ";";
}

void ND_usingDecl::print(BPEnv &bp) const
{
  bp << "using ";
  name->print(bp);
  bp << ";";
}

void ND_usingDir::print(BPEnv &bp) const
{
  bp << "using namespace ";
  name->print(bp);
  bp << ";";
}


// ---------------- global funcs -----------------------
void bppTranslationUnit(ostream &out, TranslationUnit const &unit)
{
  BPEnv bp;
  unit.print(bp);
  BPRender render;
  out << render.takeAndRender(bp).c_str();
}


// EOF
