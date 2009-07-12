template<typename C> struct basic_ostream {
  basic_ostream<C>& operator<<(basic_ostream<C>& (*f)(basic_ostream<C>&));
};

template<typename C> basic_ostream<C>& endl(basic_ostream<C>& os) {
}

basic_ostream<char> cout;

int main() {
  cout << endl;
}
