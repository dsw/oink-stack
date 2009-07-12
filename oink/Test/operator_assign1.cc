// A::operator= is declared and never defined nor used.  It's a common idiom
// to declare operator= private to prevent use of the default copy operator.

struct A {
private:
    A &operator =(A &);
};

struct B : A {
};

int main() {
}
