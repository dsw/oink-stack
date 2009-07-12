// this tests that parameter variables are cloned correctly;
// Variable::value wasn't being cloned

void clear(int state = 0);
void rdbuf() {
  clear();
}
