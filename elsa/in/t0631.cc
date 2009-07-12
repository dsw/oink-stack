// t0631.cc
// experiment with 10.1p4: qualifier to select subobject


struct A {
  int x;
};

//    A   A    .
//    |   |    .
//    B   C    .
//     \ /     .
//      D      .
struct B : A {};
struct C : A {};
struct D : B, C {};

void dx(D &d)
{
  //ERROR(1): d.x;
  d.C::x;
  d.B::x;
}


//    A    A     .
//    |    |     .
//    E    B     .
//   / \   |     .
//  F   G  |     .
//   \ /   |     .
//    H   /      .
//     \ /       .
//      I        .
struct E : A {};
struct F : virtual E {};
struct G : virtual E {};
struct H : F, G {};
struct I : H, B {};

void ix(I &i)
{
  //ERROR(2): i.x;
  i.E::x;
  i.F::x;
  i.G::x;     // this one demonstrates need for visited 0/1/2 in CompoundType::getSubobjects_helper
  i.H::x;
  i.B::x;
  //ERROR(3): i.I::x;
  //ERROR(4): i.A::x;
}



// EOF
