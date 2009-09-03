#ifndef LIB_BAR_H
#define LIB_BAR_H

// class
struct Bar {
  int q;
};

struct Bar *new_Bar(int q0);
int get_q_Bar(struct Bar *b);
int weird_Bar(struct Bar **b);

#endif // LIB_BAR_H
