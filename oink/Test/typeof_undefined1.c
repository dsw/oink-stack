// 'typeof' should not make a variable usedInDataflow.

// from glib/gtk/etc

extern int foo(void); // undefined

typeof(foo) bar;
int bar(void) {}

int main() { return bar(); }
