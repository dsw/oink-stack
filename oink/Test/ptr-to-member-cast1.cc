// ERR-MATCH: 18cca471-ba67-40dd-865a-c787b1c65ade

class Foo {};

int main() {
  (int Foo::*)0;
}
