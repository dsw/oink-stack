// ERR-MATCH: 19097cc2-d116-4d6a-b218-7fac6acf45ce|Assertion failed.*file oink_integrity.cc line 166

// oink: Assertion failed: integrity error
// (19097cc2-d116-4d6a-b218-7fac6acf45ce), file oink_integrity.cc line 166

// default argument in inner class of template class

template<typename T>
struct S1 {
  struct S2 {
    void foo(int arg = 5);
  };
};

template<typename T>
void S1<T>::S2::foo(int arg) {
}

int main()
{
  S1<int>::S2 s2;
  s2.foo();
}
