// typedef struct {
//   int a, b;
// } S1;
// S1 s1 = { { 0, 0 } , 0 };

// dsw: here is a simpler verison that exhibits the same problem.  Gcc
// accepts this with a warning but I think it is buggy code, so I
// think we should reject
struct S1 {
  int a, b;
};

struct S1 s1 = { { 0, 0 } };
