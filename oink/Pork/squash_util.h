#ifndef SQUASH_UTIL_H
#define SQUASH_UTIL_H

// See License.txt for copyright and terms of use

// Provides useful utility functions

#include "cc_print.h"

// Convenient func to print out arbitrary AST nodes
template<class T> inline void print(std::ostream &ostream, T *m) {
  OStreamOutStream out0(ostream);
  CodeOutStream codeOut(out0);
  CTypePrinter typePrinter;
  PrintEnv env(typePrinter, &codeOut);
  m->print(env);
  codeOut.finish();
}

// Gets rid of implicit class members
inline void killImplicitMembers(TS_classSpec *c) {
  ASTListMutator<Member> m(c->members->list);
  while (!m.isDone()) {
    Member *mem = m.data();

    if (!mem->isMR_func()) {
      m.adv();
      continue;
    }

    Function *f = mem->asMR_func()->f;
    if (f->dflags & DF_IMPLICIT) {
      m.remove();
    } else {
      m.adv();
    }
  }
}

// don't worry about efficiency here
inline std::string replace(std::string const &strin,
                           std::string const &what, 
                           std::string const &with)
{
  std::string str = strin;
  std::string::size_type pos = str.find(what, 0);
  while (pos != std::string::npos) {
    str.replace(pos, what.size(), with);
    pos = str.find(what, pos + with.size());
  }
  return str;
}

inline Expression* cond2expr(Condition *cond) {
  if (cond->isCN_expr()) {
    return cond->asCN_expr()->expr->expr;
  } else {
    return cond->asCN_decl()->typeId->decl->init->asIN_expr()->e;
  }
}

// returns the underlying "target" objected
inline Expression *expr2obj(Expression *e) {
  switch (e->kind()) {

  case Expression::E_VARIABLE:
  case Expression::E_FIELDACC:
    return e;

  case Expression::E_DEREF:
    return expr2obj(e->asE_deref()->ptr);

  case Expression::E_CAST:
    return expr2obj(e->asE_cast()->expr);

  case Expression::E_BINARY://cast(..)->foo..look at foo
    {
      E_binary *b = e->asE_binary();
       switch(b->op) {
      case BIN_ARROW_STAR:
	return expr2obj(e->asE_binary()->e2);
	case BIN_PLUS:
	if (b->e1->type->isReferenceType() 
	    && b->e1->type->getAtType()->isArrayType()) {
	  Type *t2 = b->e2->type;
	  if (t2->isReferenceType()) t2 = t2->getAtType();
	  
	  if (t2->isCVAtomicType()
	      && t2->asCVAtomicType()->atomic->isSimpleType()
	      && t2->asCVAtomicType()->atomic->asSimpleType()->type == ST_INT) {
	    return expr2obj(b->e1);
	  }
	}
      default:
	break;
      }
    }

  default:
    return NULL;
  }
}

inline Variable *expr2var(Expression *eIn) {
  Expression *e = expr2obj(eIn);
  if (!e) return NULL;
  switch (e->kind()) {
  case Expression::E_VARIABLE:
    return e->asE_variable()->var;
  case Expression::E_FIELDACC:
    return e->asE_fieldAccC()->field;
  default:
    return NULL;
  }
}

inline Type* skipPtrOrRef(Type *t) {
  switch(t->getTag()) {
  case BaseType::T_POINTER:
  case BaseType::T_REFERENCE:
  case BaseType::T_ARRAY:
    return skipPtrOrRef(t->getAtType());
  case BaseType::T_POINTERTOMEMBER:
    return skipPtrOrRef(t->asPointerToMemberType()->atType);
  default:
    return t;
  }
}

inline std::string resolveAbsolutePath(std::string const &dir,
                                       std::string const &inPath)
{
 std::string path = inPath;
  if (path[0] != '/') {
    path = dir + path;
  }
  std::string::size_type p = 0;
  // symlink resolution code
  char linkbuf[4096];
  int n = readlink(path.c_str(), linkbuf, sizeof(linkbuf)-1);
  if (n != -1) {
    linkbuf[n] = 0;
    std::string strlink = linkbuf;
    if(strlink[0] != '/') {
      strlink = path.substr(0, path.rfind('/') + 1) + strlink;
    }
    return resolveAbsolutePath(dir, strlink);
  }
  if (dir.empty()) return path;
  
  while ((p = path.find("/..", p)) != std::string::npos) {
    if (!p) break;
    std::string::size_type p2 = path.rfind('/', p - 1);
    if (p2 == std::string::npos) break;
    path = path.substr(0, p2) + path.substr(p + 3, path.size() - 3 - p);
    p = p2;
  }
  return path;
}

#endif // SQUASH_UTIL_H
