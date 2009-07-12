// check that assert_type() and change_type() pretty print correctly
// check if assembly is attaching qualifiers to variables
typedef struct {
	volatile unsigned int lock_gronk;
} spinlock_t;
extern inline void spin_unlock(spinlock_t *lock)
{
  __qual_assert_type(*lock, $locked spinlock_t);
//  	__asm__ __volatile__(
//  		"movb $1,%0"
//  		:"=m" ((*(__dummy_lock_t *)( lock )) ));
  __qual_change_type(*lock, spinlock_t);
}
