
template<typename T>
struct S1 {
    void foo();
};

template<typename T>
void S1<T>::foo() {
}

extern template struct S1<char>;
template struct S1<char>;

int main() {
    S1<char> s1;
    s1.foo();
}
