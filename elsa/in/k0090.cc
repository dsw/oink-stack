// template specialization of array

// originally found in package 'shaketracker_0.4.6-4'

// a.ii:6:7: error: RHS of . or -> must be of the form "~ identifier" if the LHS is not a class; the LHS is `int const [2]' (inst from a.ii:19:13) (inst from a.ii:26:10)

// ERR-MATCH: RHS of . or -> must be of the form

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
  int items[] = { 0, 0 };
  SArray a(items);
}
