struct A {
    int a;
};

struct B : public A {
    B(int b) { this->b = b; }
    int b;
};

void foo(const A& arg) {
    arg.a;
}

int main(int argc, char** argv)
{
    A a;
    foo((0 == argc) ? B(1) : a);
    return 0;
}
