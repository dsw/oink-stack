// template specialization of array

// originally found in package 'shaketracker_0.4.6-4'

// k0090a.cc:21:13: error: during partial specialization parameter `N' not bound in inferred bindings (inst from k0090a.cc:28:10)
// k0090a.cc:28:10: error: confused by earlier errors, bailing out

// ERR-MATCH: during partial specialization parameter .*? not bound in inferred bindings

template <class T>
struct Traits {
  static void get_data(const T& t) {
    t.dont_use_me();
  }
};

template <class T, int N>
struct Traits<T[N]>  {
  static void get_data(const T* t) {
  }
};

struct SArray {
  template <class T> SArray(const T& t)
  {
    Traits<T>::get_data(t);
  }
};

int main()
{
  int items[] = { 0 };
  SArray a(items);
}
