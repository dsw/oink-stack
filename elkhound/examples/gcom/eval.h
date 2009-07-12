// eval.h
// evaluation environment for gcom

#ifndef EVAL_H
#define EVAL_H

#include "strhash.h"      // TStringHash
#include "str.h"          // string

class Binding {
public:
  string name;
  int value;
  
public:
  Binding(char const *n, int v)
    : name(n),
      value(v)
  {}
  ~Binding();
};

class Env {
private:
  // map: name -> value
  TStringHash<Binding> map;

public:
  Env();
  ~Env();

  int get(char const *x);
  void set(char const *x, int val);
};

#endif // ENV_H
