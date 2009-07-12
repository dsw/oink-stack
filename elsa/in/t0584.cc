// t0584.cc
// problem with non-concrete types showing up in instantiations

template <typename T_> struct S1 {
 typedef T_ T;
};

template <typename T_> struct S2 : S1<T_>
{
 typedef typename S1<T_>::T T;

 void foo (T t = T()) {}
 void bar (T_ t = T_()) {}
};

int main()
{
 S2<int> x;
 x.foo();
 x.bar();
}
