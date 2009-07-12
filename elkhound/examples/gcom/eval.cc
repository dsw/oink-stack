// eval.cc
// implementation of the 'eval' methods declared in gcom.ast

#include "eval.h"        // this module
#include "ast.h"         // AST declarations

#include <stdlib.h>      // exit
#include <assert.h>      // assert
#include <stdio.h>       // printf


// ------------------- Binding -----------------
Binding::~Binding()
{}


// --------------------- Env -------------------
static char const *bindToName(Binding *b)
{
  return b->name.c_str();
}

Env::Env()
  : map(bindToName)
{}

Env::~Env()
{}


int Env::get(char const *x)
{
  Binding *b = map.get(x);
  if (!b) {
    return 0;     // unset variables default to 0 value
  }
  else {
    return b->value;
  }
}

void Env::set(char const *x, int val)
{
  Binding *b = map.get(x);
  if (!b) {
    // add new binding
    map.add(x, new Binding(x, val));
  }
  else {
    b->value = val;
  }
}


// -------------------- AExp -------------------
int A_lit::eval(Env &env)
{
  return n;
}

int A_var::eval(Env &env)
{
  return env.get(x.c_str());
}

int A_bin::eval(Env &env)
{
  switch (op) {
    default:       assert(!"bad code");
    case AO_PLUS:  return a1->eval(env) + a2->eval(env);
    case AO_MINUS: return a1->eval(env) - a2->eval(env);
    case AO_TIMES: return a1->eval(env) * a2->eval(env);
  }
}


// -------------------- BExp -------------------
bool B_lit::eval(Env &env)
{
  return b;
}

bool B_pred::eval(Env &env)
{
  switch (op) {
    default:       assert(!"bad code");
    case BP_EQUAL: return a1->eval(env) == a2->eval(env);
    case BP_LESS:  return a1->eval(env) < a2->eval(env);
  }
}

bool B_not::eval(Env &env)
{
  return !b->eval(env);
}

bool B_bin::eval(Env &env)
{
  switch (op) {
    default:       assert(!"bad code");
    case BO_AND: return b1->eval(env) && b2->eval(env);
    case BO_OR:  return b1->eval(env) && b2->eval(env);
  }
}


// -------------------- Stmt -------------------
void S_skip::eval(Env &env)
{}

void S_abort::eval(Env &env)
{
  printf("abort command executed\n");
  exit(0);
}

void S_print::eval(Env &env)
{
  printf("%s is %d\n", x.c_str(), env.get(x.c_str()));
}

void S_assign::eval(Env &env)
{
  env.set(x.c_str(), a->eval(env));
}

void S_seq::eval(Env &env)
{
  s1->eval(env);
  s2->eval(env);
}

void S_if::eval(Env &env)
{
  if (!g->eval(env)) {
    printf("'if' command had no enabled alternatives; aborting\n");
    exit(0);
  }
}

void S_do::eval(Env &env)
{
  while (g->eval(env))
    {}
}


// -------------------- GCom -------------------
bool G_stmt::eval(Env &env)
{ 
  if (b->eval(env)) {
    s->eval(env);
    return true;
  }
  else {
    return false;
  }
}

bool G_seq::eval(Env &env)
{
  // The usual semantics for guarded commands say that if both guards
  // are true, then we can execute either one.  Here, I'm going to
  // always execute the *first* one with an enabled guard; that
  // behavior is allowed, but not required.  I leave it as an exercise
  // to modify this code so that the executions it models will cover a
  // larger fraction of the state space reachable under the
  // traditional semantics.
  
  if (g1->eval(env)) { return true; }
  if (g2->eval(env)) { return true; }
  return false;
}
