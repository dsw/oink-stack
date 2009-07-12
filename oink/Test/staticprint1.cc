class A {};
class B : public virtual A {};
class C : public virtual A {};
class D : public A {};
class E : public B, public D {};
