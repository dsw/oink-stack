// paths.cc            see license.txt for copyright and terms of use
// code for paths.h

#include "paths.h"       // this module

#include "c.ast.gen.h"   // C AST
#include "sobjlist.h"    // SObjList
#include "objlist.h"     // ObjList
#include "c_env.h"       // Env
#include "trace.h"       // tracingSys
#include "treeout.h"     // treeOut


// one thing to note about constness: when an AST node contains a field
// to store the path count, the counting function is declared non-const;
// but when the AST node does not store the count, the counting function
// is declared to accept a const*


// prototypes
void findPathRoots(SObjList<Statement> &list, TF_func const *func);
void findPathRoots(SObjList<Statement> &list, Statement const *stmt);
int countPaths(Env &env, TF_func *func);
int countPathsFrom(Env &env, SObjList<Statement> &path,
                   Statement *node, bool isContinue);
int countExprPaths(Initializer *init);
void printPaths(TF_func const *func);
void printPathFrom(SObjList<Statement /*const*/> &path, int index,
                   Statement const *node, bool isContinue);
void printPath(SObjList<Statement /*const*/> &path, char const *label);


// watch for overflow
static int mult(int a, int b)
{
  int r = a*b;
  if (a>0 && b>0 && (r < a || r < b)) {
    xfailure(stringc << "arithmetic overflow: " << a << " * " << b);
  }
  if (r > 1000) {
    cout << r << " is more than 1000 paths!\n";
  }
  return r;
}


// --------------------- finding roots ----------------------
void findPathRoots(SObjList<Statement> &list, TF_func const *func)
{
  list.reverse();
  list.prepend(func->body);
  findPathRoots(list, func->body);
  list.reverse();
}


void findPathRoots(SObjList<Statement> &list, Statement const *stmt)
{
  ASTSWITCHC(Statement, stmt) {
    ASTCASEC(S_label, l) {
      findPathRoots(list, l->s);
    }
    ASTNEXTC(S_case, c) {
      findPathRoots(list, c->s);
    }
    ASTNEXTC(S_caseRange, c) {
      findPathRoots(list, c->s);
    }
    ASTNEXTC(S_default, d) {
      findPathRoots(list, d->s);
    }
    ASTNEXTC(S_compound, c) {
      FOREACH_ASTLIST(Statement, c->stmts, iter) {
        findPathRoots(list, iter.data());
      }
    }
    ASTNEXTC(S_if, i) {
      findPathRoots(list, i->thenBranch);
      findPathRoots(list, i->elseBranch);
    }
    ASTNEXTC(S_switch, s) {
      findPathRoots(list, s->branches);
    }
    ASTNEXTC(S_while, w) {
      findPathRoots(list, w->body);
    }
    ASTNEXTC(S_doWhile, d) {
      findPathRoots(list, d->body);
    }
    ASTNEXTC(S_for, f) {
      findPathRoots(list, f->init);
      findPathRoots(list, f->body);
    }
    ASTNEXTC(S_invariant, i) {
      list.prepend(const_cast<S_invariant*>(i));       // action!
    }
    ASTENDCASECD
  }
}


// ------------------------ counting paths ------------------
int countPaths(Env &env, TF_func *func)
{
  func->numPaths=0;

  // enumerate the roots
  findPathRoots(func->roots, func);

  // enumerate all paths from each root
  SMUTATE_EACH_OBJLIST(Statement, func->roots, iter) {
    Statement *s = iter.data();

    SObjList<Statement> path;
    countPathsFrom(env, path, s, false /*isContinue*/);
    func->numPaths += s->numPaths;
  }

  return func->numPaths;
}


// need the 'path' to detect circularity;
// this function has similar structure to 'printPathFrom', below
int countPathsFrom(Env &env, SObjList<Statement> &path,
                   Statement *node, bool isContinue)
{
  if (node->kind() != Statement::S_INVARIANT &&
      path.contains(node)) {
    env.warnLoc(node->loc, "circular path");
    if (tracingSys("circular")) {
      // print the circular path
      SFOREACH_OBJLIST(Statement, path, iter) {
        cout << "  " << toString(iter.data()->loc) << endl;
      }
    }
    return 1;
  }

  path.prepend(node);
  int ret;

  if (node->kind() == Statement::S_INVARIANT &&
      path.count() > 1) {
    // we've reached an invariant point, so the path stops here;
    // but we don't change node->paths since that is for the #
    // of paths from 'node' as a path *start*, not end
    ret = 1;
  }

  else {
    // retrieve all successors of this node
    VoidList successors;
    node->getSuccessors(successors, isContinue);

    if (successors.isEmpty()) {
      // this is a return statement (or otherwise end of function)
      ret = 1;
    }
    else {
      // initially, there are 0 paths from here
      ret = 0;

      // consider each choice
      for (VoidListIter iter(successors); !iter.isDone(); iter.adv()) {
        void *np = iter.data();
        // unfortunately I can't easily parameterize a voidlist by whether
        // my interpretation of a field of its contents is 'const' ...
        Statement *s = const_cast<Statement*>(nextPtrStmt(np));
        int ct = countPathsFrom(env, path, s, nextPtrContinue(np));

        // add all paths from this successor to our total
        ret += ct;
      }
    }

    // now, every path through expressions in this statement will be
    // (conservatively) considered to possibly be followed by any control
    // flow path *from* this statement
    ret = mult(ret, countExprPaths(node, isContinue));

    // in this branch (only), we write the # of paths into the statement
    node->numPaths = ret;
  }

  path.removeFirst();
  return ret;
}


// ---------------------- printing paths ---------------------
void printPaths(TF_func const *func)
{
  // enumerate all paths from each root
  SFOREACH_OBJLIST(Statement, func->roots, iter) {
    Statement const *s = iter.data();
    cout << "root at " << toString(iter.data()->loc) << ":\n";

    // the whole point of counting the paths was so I could
    // so easily get a handle on all of them, to be able to
    // write a nice loop like this:
    for (int i=0; i < s->numPaths; i++) {
      cout << "  path " << i << ":\n";
      SObjList<Statement /*const*/> path;
      printPathFrom(path, i, s, false /*isContinue*/);
    }
  }
}


int numPathsThrough(Statement const *stmt)
{
  // how many paths lead from 'stmt'?  usually just s->numPaths, but
  // if it is a path cutpoint then there's exactly one path from 'stmt'
  int ret = stmt->isS_invariant()? 1 : stmt->numPaths;
  return ret;
}


// abstract the "4" slightly..
//#define PATHOUT treeOut(4)
// actually, I didn't want these as headings at all..
#define PATHOUT cout << "  "

// want 'path' to detect circularity (for debugging);
// this function has similar structure to 'countPathsFrom', above
void printPathFrom(SObjList<Statement /*const*/> &path, int index,
                   Statement const *node, bool isContinue)
{
  // validate 'index'
  int exprPaths = countExprPaths(node, isContinue);
  xassert(exprPaths >= 1);
  xassert(0 <= index && index < node->numPaths);
    // this used to say node->numPaths * exprPaths, but it seems clear
    // that numPaths *already* includes the contribution from exprPaths

  // print this node
  PATHOUT << toString(node->loc) << ": "
          << node->kindName() << endl;

  // debugging check
  if (path.contains(node)) {
    PATHOUT << "CIRCULAR path\n";
    return;
  }
  path.prepend(const_cast<Statement*>(node));

  // follow one expression path in this statement
  printExprPath(index % exprPaths, node, isContinue);
  index = index / exprPaths;

  // retrieve all successors of this node
  VoidList successors;
  node->getSuccessors(successors, isContinue);

  if (successors.isEmpty()) {
    // this is a return statement (or otherwise end of function)
    xassert(index == 0);
    PATHOUT << "path ends at a return\n";
  }
  else {
    // consider each choice
    // largely COPIED to vcgen.cc:Statement::vcgenPath
    for (VoidListIter iter(successors); !iter.isDone(); iter.adv()) {
      void *np = iter.data();
      Statement const *s = nextPtrStmt(np);
      int pathsFromS = numPathsThrough(s);

      // are we going to follow 's'?
      if (index < pathsFromS) {
        // yes; is 's' an invariant?
        if (s->isS_invariant()) {
          // terminate the path; print final node
          PATHOUT << toString(s->loc) << ": "
                  << s->kindName() << endl;
          PATHOUT << "path ends at an invariant\n";
        }
        else {
          // continue the path
          printPathFrom(path, index, s, nextPtrContinue(np));
        }
        index = 0;       // remember that we found a path to follow
        break;
      }

      else {
        // no; factor out s's contribution to the path index
        index -= pathsFromS;
      }
    }

    // make sure we followed *some* path
    xassert(index == 0);
  }

  path.removeFirst();
}


// --------- counting/print expression paths in statements ------------
int countExprPaths(Statement const *stmt, bool isContinue)
{
  ASTSWITCHC(Statement, stmt) {
    ASTCASEC(S_expr, e) {
      return e->expr->numPaths1();
    }
    ASTNEXTC(S_if, i) {
      return i->cond->numPaths1();
    }
    ASTNEXTC(S_switch, s) {
      return s->expr->numPaths1();
    }
    ASTNEXTC(S_while, w) {
      return w->cond->numPaths1();
    }
    ASTNEXTC(S_doWhile, d) {
      if (isContinue) {
        // enter at the guard, so as many paths as paths through guard
        return d->cond->numPaths1();
      }
      else {
        // enter at top, no expr is evaluated
        return 1;
      }
    }
    ASTNEXTC(S_for, f) {
      if (isContinue) {
        // enter just before inc and test
        return mult(f->after->numPaths1(), f->cond->numPaths1());
      }
      else {
        // enter at init, immediately apply guard
        //return mult(countExprPaths(f->init, false), f->cond->numPaths1());
        // UPDATE: my CFG does the init before the 'for'
        return f->cond->numPaths1();
      }
    }
    ASTNEXTC(S_return, r) {
      if (r->expr) {
        return r->expr->numPaths1();
      }
      else {
        return 1;
      }
    }
    ASTNEXTC(S_decl, d) {
      // somewhat complicated because we need to dig around in the
      // declaration for paths through initializing expressions
      int ret = 1;
      FOREACH_ASTLIST(Declarator, d->decl->decllist, dcltr) {
        Initializer const *init = dcltr.data()->init;
        if (init) {
          ret = mult(ret, countExprPaths(init));
        }
      }
      return ret;
    }
    ASTDEFAULTC {
      // no expressions in the statement, only 1 path
      return 1;
    }
    ASTENDCASEC
  }
}


void printExprPath(int index, Statement const *stmt, bool isContinue)
{
  xassert(index >= 0);

  ASTSWITCHC(Statement, stmt) {
    ASTCASEC(S_expr, e) {
      printPath(index, e->expr);
    }
    ASTNEXTC(S_if, i) {
      printPath(index, i->cond);
    }
    ASTNEXTC(S_switch, s) {
      printPath(index, s->expr);
    }
    ASTNEXTC(S_while, w) {
      printPath(index, w->cond);
    }
    ASTNEXTC(S_doWhile, d) {
      if (isContinue) {
        // enter at the guard, so as many paths as paths through guard
        printPath(index, d->cond);
      }
      else {
        // enter at top, no expr is evaluated
        xassert(index==0);
      }
    }
    ASTNEXTC(S_for, f) {
      int modulus = f->cond->numPaths1();
      if (isContinue) {
        // enter just before inc and test
        printPath(index / modulus, f->after);
        printPath(index % modulus, f->cond);
      }
      else {
        // enter at init, immediately apply guard
        //printExprPath(index / modulus, f->init, false);
        // UPDATE: CFG does 'init' before 'for'
        xassert(index < modulus);
        printPath(index /* % modulus*/, f->cond);
      }
    }
    ASTNEXTC(S_return, r) {
      if (r->expr) {
        printPath(index, r->expr);
      }
      else {
        xassert(index==0);
      }
    }
    ASTNEXTC(S_decl, d) {
      // somewhat complicated because we need to dig around in the
      // declaration for paths through initializing expressions
      int paths = 1;
      FOREACH_ASTLIST(Declarator, d->decl->decllist, dcltr) {
        Initializer const *init = dcltr.data()->init;
        if (init) {
          paths = countExprPaths(init);
          printExprPath(index % paths, init);
          index = index / paths;
        }
      }

      // if we entered the loop at all, this ensures the last iteration
      // consumed all of 'index'; if we never entered the loop, this
      // checks that index was 0
      xassert(index < paths);
    }
    ASTDEFAULTC {
      // no expressions in the statement, only 1 path
      xassert(index==0);
    }
    ASTENDCASEC
  }
}


// --------------- count/print paths through initializers --------------
int countExprPaths(Initializer const *init)
{
  ASTSWITCHC(Initializer, init) {
    ASTCASEC(IN_expr, ie) {
      return ie->e->numPaths1();
    }
    ASTNEXTC(IN_compound, ic) {
      int ret = 1;
      FOREACH_ASTLIST(Initializer, ic->inits, iter) {
        ret = mult(ret, countExprPaths(iter.data()));
      }
      return ret;
    }
    ASTDEFAULTC {
      xfailure("bad code");
      return 0;
    }
    ASTENDCASEC
  }
}


void printExprPath(int index, Initializer const *init)
{
  ASTSWITCHC(Initializer, init) {
    ASTCASEC(IN_expr, ie) {
      printPath(index, ie->e);
    }
    ASTNEXTC(IN_compound, ic) {
      // this loop is very similar to the one above for S_decl
      int paths = 1;
      FOREACH_ASTLIST(Initializer, ic->inits, iter) {
        Initializer const *i = iter.data();
        paths = countExprPaths(i);
        printExprPath(index % paths, i);
        index = index / paths;
      }
      xassert(index < paths);
    }
    ASTENDCASECD
  }
}


// -------------- count/print paths through expressions --------------
int countPaths(Env &env, Expression *ths)
{
  // default: 1 path, no side effects, is already set: 0
  xassert(ths->numPaths == 0);
  int numPaths = 0;

  // don't bother counting paths in predicates, where everything
  // is side-effect-free
  if (env.inPredicate) {
    return 0;
  }

  #define SIDE_EFFECT() numPaths = max(numPaths,1) /* user ; */

  ASTSWITCH(Expression, ths) {
    ASTCASE(E_funCall, ths) {
      numPaths = ths->func->numPaths;

      FOREACH_ASTLIST_NC(Expression, ths->args, iter) {
        // compute # of paths
        int argPaths = iter.data()->numPaths;
        if (argPaths > 0) {
          if (numPaths > 0) {
            env.warn("more than one argument expression has side effects");
            numPaths = mult(numPaths, argPaths);
          }
          else {
            numPaths = argPaths;
          }
        }
      }

      // the call itself counts as a side-effecting expression (unless I
      // add an annotation to declare something as "functional" ...)
      SIDE_EFFECT();
    }
    ASTNEXT(E_fieldAcc, ths) {
      numPaths = ths->obj->numPaths;
    }
    ASTNEXT(E_sizeof, ths) {
      PRETEND_USED(ths);     // silence warning
      numPaths = 0;
    }
    ASTNEXT(E_unary, ths) {
      numPaths = ths->expr->numPaths;
    }
    ASTNEXT(E_effect, ths) {
      numPaths = ths->expr->numPaths;
      SIDE_EFFECT();
    }
    ASTNEXT(E_binary, ths) {
      // there are at least as many paths as paths through left-hand side
      numPaths = ths->e1->numPaths;

      if (ths->e2->numPaths == 0) {
        // if the RHS has no side effects, it contributes nothing to the
        // paths computation, regardless of whether 'op' is short-circuit,
        // so we just propagate the LHS's numPaths
      }

      else {
        // if the RHS has a side effect, this expression as a whole might
        // have a side effect
        SIDE_EFFECT();

        if (ths->op==BIN_AND || ths->op==BIN_OR) {
          // path computation with short-circuit evaluation: for each LHS
          // path, we could take any one of the RHS paths, *or* skip
          // evaluating the RHS altogether
          numPaths = mult(numPaths, (1 + ths->e2->numPaths));
        }

        else {
          // path computation without short-circuit evaluation: we can
          // take any combination of LHS and RHS paths
          numPaths = mult(numPaths, ths->e2->numPaths);
        }
      }
    }
    ASTNEXT(E_addrOf, ths) {
      numPaths = ths->expr->numPaths;
    }
    ASTNEXT(E_deref, ths) {
      numPaths = ths->ptr->numPaths;
    }
    ASTNEXT(E_cast, ths) {
      numPaths = ths->expr->numPaths;
    }
    ASTNEXT(E_cond, ths) {
      // start with # of paths through conditional
      numPaths = ths->cond->numPaths;

      if (ths->th->numPaths == 0 && ths->el->numPaths == 0) {
        // no side effects in either branch; we're good to go
      }

      else {
        // since at least one branch as a side effect, let's say
        // all three components have at least 1 path
        SIDE_EFFECT();
        int thenPaths = ths->th->numPaths1();
        int elsePaths = ths->el->numPaths1();

        // for every path through the conditional, we could either
        // go through a 'then' path or an 'else' path
        numPaths = mult(numPaths, thenPaths + elsePaths);
      }
    }
    #if 0
    ASTNEXT(E_gnuCond, ths) {
      // degenerate form of E_cond
      numPaths = ths->cond->numPaths;
      if (ths->el->numPaths > 0) {
        ths->recordSideEffect();
        numPaths = mult(numPaths, 1 + ths->el->numPaths);
      }
    }    
    #endif // 0
    ASTNEXT(E_comma, ths) {
      numPaths = ths->e1->numPaths;
      if (ths->e2->numPaths > 0) {
        numPaths = mult(max(numPaths,1), ths->e2->numPaths);
      }
    }
    ASTNEXT(E_assign, ths) {
      numPaths = ths->src->numPaths;              // start with paths through src
      SIDE_EFFECT();                              // clearly this expr has a side effect
      numPaths = mult(numPaths, ths->target->numPaths1());       // add paths through target
    }
    ASTNEXT(E_quantifier, ths) {
      xfailure("shouldn't get here because only allowed in predicates");
      PRETEND_USED(ths);
    }
    ASTENDCASED
  }

  return ths->numPaths = numPaths;

  #undef SIDE_EFFECT
}


// print choice point decisions, and side effects
void printPath(int index, Expression const *ths)
{
  xassert(0 <= index && index < ths->numPaths1());

  ASTSWITCHC(Expression, ths) {
    ASTCASEC(E_funCall, ths) {
      PATHOUT << "call to " << ths->func->toString() << endl;

      // 'func'
      int subexpPaths = ths->func->numPaths1();
      printPath(index % subexpPaths, ths->func);
      index = index / subexpPaths;

      // args
      FOREACH_ASTLIST(Expression, ths->args, iter) {
        subexpPaths = iter.data()->numPaths1();
        printPath(index % subexpPaths, iter.data());
        index = index / subexpPaths;
      }
      xassert(index < subexpPaths);
    }
    ASTNEXTC(E_fieldAcc, ths) {
      printPath(index, ths->obj);
    }
    ASTNEXTC(E_sizeof, ths) {
      PRETEND_USED(ths);     // silence warning
    }
    ASTNEXTC(E_unary, ths) {
      printPath(index, ths->expr);
    }
    ASTNEXTC(E_effect, ths) {
      PATHOUT << "side effect: " << ths->toString() << endl;
      printPath(index, ths->expr);
    }
    ASTNEXTC(E_binary, ths) {
      if (ths->e2->numPaths == 0) {
        // if the RHS has no side effects, the encoding is more compact,
        // and only the LHS is relevant
        printPath(index, ths->e1);
      }

      else {
        // print LHS
        int modulus = ths->e1->numPaths1();
        printPath(index % modulus, ths->e1);
        index = index / modulus;

        // consider operator
        if (ths->op==BIN_AND || ths->op==BIN_OR) {
          // print RHS *if* it's followed
          if (index > 0) {
            PATHOUT << "traversing into rhs of " << toString(ths->op) << endl;
            printPath(index-1, ths->e2);
          }
          else {
            PATHOUT << "short-circuiting rhs of " << toString(ths->op) << endl;
          }
        }

        else {
          // non-short-circuit: always evaluate RHS
          printPath(index, ths->e2);
        }
      }
    }
    ASTNEXTC(E_addrOf, ths) {
      printPath(index, ths->expr);
    }
    ASTNEXTC(E_deref, ths) {
      printPath(index, ths->ptr);
    }
    ASTNEXTC(E_cast, ths) {
      printPath(index, ths->expr);
    }
    ASTNEXTC(E_cond, ths) {
      // condition
      int modulus = ths->cond->numPaths1();
      printPath(index % modulus, ths->cond);
      index = index / modulus;

      if (ths->th->numPaths == 0 && ths->el->numPaths == 0) {
        // no side effects in either branch; we're good to go
      }

      else {
        int thenPaths = ths->th->numPaths1();
        int elsePaths = ths->el->numPaths1();

        if (index < thenPaths) {
          PATHOUT << "taking 'then' path through ?:\n";
          printPath(index, ths->th);
        }
        else {
          PATHOUT << "taking 'else' path through ?:\n";
          printPath(index - elsePaths, ths->el);
        }
      }
    }
    #if 0
    ASTNEXTC(E_gnuCond, ths) {
      // degenerate form of E_cond
      PATHOUT << "gnuCond unhandled for now: " << (void*)ths << "\n";
    }
    #endif // 0
    ASTNEXTC(E_comma, ths) {
      int modulus = ths->e1->numPaths1();
      printPath(index % modulus, ths->e1);
      printPath(index / modulus, ths->e2);
    }
    ASTNEXTC(E_assign, ths) {
      PATHOUT << "side effect: " << ths->toString() << endl;
      int modulus = ths->src->numPaths1();
      printPath(index % modulus, ths->src);
      printPath(index / modulus, ths->target);
    }
    ASTENDCASECD
  }
}
