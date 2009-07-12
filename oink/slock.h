// see License.txt for copyright and terms of use

// quarl 2006-06-08
//    single-threaded lock, for debugging

#ifndef SLOCK_H
#define SLOCK_H

#include "exc.h"

class SLock {
  bool locked;

public:
  SLock() : locked(false) {}
  ~SLock() { xassert(!locked); }

  void acquire() { xassert(!locked); locked = true; }
  void release() { xassert(locked); locked = false; }
};

class SLockAcq {
  SLock &lock;
public:
  SLockAcq(SLock &lock0) : lock(lock0) { lock.acquire(); }
  ~SLockAcq() { lock.release(); }
};

#endif
