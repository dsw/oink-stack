// check that assert_type and change_type at least parse; these are
// cqual additions to the language somewhat resembling sizeof or
// typeof in that one of their arguments is a type

extern inline void spin_lock(spinlock_t *lock)
{
  __qual_assert_type(3, int);
  __qual_change_type(4, double);

  __qual_assert_type(*lock, spinlock_t $unlocked);
  __qual_change_type(*lock, spinlock_t $locked);

  // FIX: I don't know why this parses since the qualifier comes
  // first.  Seems that it shouldn't.
  __qual_assert_type(*lock, $unlocked spinlock_t);
  __qual_change_type(*lock, $locked spinlock_t);
}
