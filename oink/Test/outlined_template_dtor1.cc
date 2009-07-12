// non-inlined template class destructor

template <class T>
struct A {
    ~A();
};

template <class T>
A<T>::~A()
{
}

int main()
{
    A<int> a;
}
