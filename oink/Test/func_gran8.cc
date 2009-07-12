typedef void T1(int);
typedef void *T2(void);
typedef int T3;
typedef struct { } T4;
typedef struct { } *T5;
struct T6 {};

static T1 *foo1a() {}
static void foo1b(T1*) {}

static T2 foo2a() {}
static void foo2a(T2*) {}

static T3 *foo3a() {}
static void foo3b(T3*) {}

static T4 *foo4a() {}
static void foo4b(T4*) {}

static T5 *foo5a() {}
static void foo5b(T5*) {}

static T6 *foo6a() {}
static void foo6b(T6*) {}

int main()
{
  foo1a();
}



