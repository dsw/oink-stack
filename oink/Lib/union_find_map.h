// see License.txt for copyright and terms of use

#ifndef UNION_FIND_MAP_H
#define UNION_FIND_MAP_H

// A map _f_ from keys to values where the keyspace can be modded out
// by an equivalence relation _R_ implemented as a union-find.  The
// invariant is that R is a strict refinement of the pullback
// equivalence relation induced by f, f^{-1}; That is, the two actions
// of 1) modding out by R and 2) going through f are guaranteed to
// make a commutative triangle.  In other words f/R is a well-defined
// function.

// The methods are: "lookup": find key eq rep and then map it; and
// "union": merge two keys and their values.  When union occurs, there
// are three situations: 0) nether have a value, 1) one has a value,
// 2) both have a value.  In case 0), we need a callback to make a new
// value; in case 1), the value of one becomes the value of both; in
// case 2) we need to merge the two values, the client code will have
// to watch for this case and do that.

#include "ptrmap.h"
#include "Lib/union_find.h"
#include "xassert.h"

template<class K, class V>
class UnionFindMap {
  public:
  // a factory function for making more values
  typedef V *valueFactory_t();
  // a function for choosing between two values; return true for the
  // first, false for the second
  typedef bool chooseValue_t(V *a, V *b);

  private:
  UnionFind<K> *keysUnionFind;
  PtrMap<K, V> *keysToValues; // owned: made by us in the ctor, and unmade in the dtor
  valueFactory_t *valueFactory;
  chooseValue_t *chooseValue;
  friend class XmlValueWriter;
  friend class XmlValueReader;

  public:
  explicit UnionFindMap(UnionFind<K> *keysUnionFind0,
                        valueFactory_t *valueFactory0, chooseValue_t *chooseValue0)
    : keysUnionFind(keysUnionFind0)
    , keysToValues(new PtrMap<K, V>)
    , valueFactory(valueFactory0)
    , chooseValue(chooseValue0)
  {}

  // this one is for de-serializiation
  private:
  explicit UnionFindMap()
    : keysUnionFind(NULL)
    , keysToValues(NULL)
    , valueFactory(NULL)
    , chooseValue(NULL)
  {}
  public:
  static UnionFindMap *makeUnionFindMap_uninitialized() {return new UnionFindMap();}

  public:
  ~UnionFindMap() {
    xassert(keysToValues);
    delete keysToValues;
  }
  private:
  UnionFindMap(UnionFindMap &x);      // prohibit

  public:
  // Returns the discarded value if two values were merged, NULL
  // otherwise; if there is further merging to be done of two values,
  // the client must watch for a non-NULL return and do that.  This
  // way of accomplishing that due to a suggestion by Rob Johnson.
  V *uni(K *src, K *tgt);       // union two domain elements
  V *get(K *key);               // get value for key; NULL if unmapped
  V *getAndMake(K *key);        // get; make a value if unmapped
  void checkOrAdd(K *key, V *value0); // make key map to value or check that it already does
  void import(UnionFindMap &other); // import another UnionFind into this one
};

template<class K, class V>
V *UnionFindMap<K, V>::uni(K *src, K *tgt) {
//    cout << "UnionFind<K>::uni" << endl;
  xassert(src);
  xassert(tgt);
  // get the two eq-class reps before the union
  K *srcRep = keysUnionFind->find(src);
  K *tgtRep = keysUnionFind->find(tgt);
  // if they are the same, we are done
  if (srcRep == tgtRep) {
    return NULL;
  }
  // do the union
  keysUnionFind->uni(srcRep, tgtRep);
  // get the new rep for both
  K *mergedRep = keysUnionFind->find(srcRep);

  // get their old values
  V *srcValue = keysToValues->get(srcRep);
  V *tgtValue = keysToValues->get(tgtRep);
  // if a value is discarded, sort it here so we can return it to the
  // user in case there is more merging for the client to do
  V *discardedValue = NULL;

  // merge their values
  if (tgtValue) {
    if (srcValue) {
      if (tgtValue == srcValue) {
        // there is nothing to do
//          cout << "\tcase 1: nothing to do" << endl;
      } else {
//          cout << "\tcase 5: merge the two values, ";
        if (chooseValue && chooseValue(tgtValue, srcValue)) {
          // tgtValue is chosen
//            cout << "tgtValue is chosen" << endl;
          keysToValues->add(mergedRep, tgtValue);
          discardedValue = srcValue;
        } else {
          // srcValue is chosen
//            cout << "srcValue is chosen" << endl;
          keysToValues->add(mergedRep, srcValue);
          discardedValue = tgtValue;
        }
      }
    } else {
      // use tgtValue for both
//        cout << "\tcase 2: use tgtValue for both" << endl;
      keysToValues->add(mergedRep, tgtValue);
    }
  } else {
    if (srcValue) {
      // use srcValue for both
//        cout << "\tcase 3: use srcValue for both" << endl;
      keysToValues->add(mergedRep, srcValue);
    } else {
      // make a new one and share it for both
//        cout << "\tcase 4: make a new value and share it for both" << endl;
      V *tmpValue = valueFactory();
      keysToValues->add(mergedRep, tmpValue);
    }
  }
  {
    V *getSrc = get(src);
    xassert(getSrc);
    xassert(getSrc == get(tgt));
  }
  return discardedValue;
}

template<class K, class V>
V *UnionFindMap<K, V>::get(K *key) {
  xassert(key);
  // find the representative of the key
  key = keysUnionFind->find(key);
  // lookup its value
  return keysToValues->get(key);
}

template<class K, class V>
V *UnionFindMap<K, V>::getAndMake(K *key) {
  V *value = get(key);
  if (!value) {
    value = valueFactory();
    keysToValues->add(key, value);
  }
  return value;
}

template<class K, class V>
void UnionFindMap<K, V>::checkOrAdd(K *key, V *value0) {
  V *value = get(key);
  if (value) xassert(value == value0);
  else keysToValues->add(key, value0);
}

template<class K, class V>
void UnionFindMap<K, V>::import(UnionFindMap &other) {
  xassert(valueFactory == other.valueFactory);
  xassert(chooseValue == other.chooseValue);
  // it is important to first import the UnionFind on the domain so
  // that it is operating as we insert the edges of the other's map
  keysUnionFind->import(*other.keysUnionFind);
  // now import the map edges
  for(typename PtrMap<K, V>::Iter iter(*other.keysToValues); !iter.isDone(); iter.adv()) {
    checkOrAdd(iter.key(), iter.value());
  }
}

#endif // UNION_FIND_MAP_H
