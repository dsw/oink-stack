struct A
{
    virtual int operator () (int x) const = 0;
};

struct B: A {
    virtual int operator () (int x) const;
};

int main() {}
