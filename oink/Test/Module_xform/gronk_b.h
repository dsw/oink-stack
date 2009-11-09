#ifndef GRONK_B_H
#define GRONK_B_H

struct Gronk_B {
  // public
  int y;

  // private
  int z;
};

struct Gronk_B *new_Gronk_B(int y0, int z0);
void del_Gronk_B(struct Gronk_B *g);

int get_y(struct Gronk_B *g);
void set_y(struct Gronk_B *g, int y0);

#endif  // GRONK_B_H
