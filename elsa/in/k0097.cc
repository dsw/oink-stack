// ERR-MATCH: ed1f952c-dbf5-41bf-9778-d5b0c0bda5af

float foo(float);

template <float (*func)(float)>
struct S1 {
};

int main() {
  S1<foo> s1;
}
