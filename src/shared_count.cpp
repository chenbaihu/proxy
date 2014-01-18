#include "shared_count.h"
#ifdef OS_WIN
#include <Windows.h>

long increment(long v) throw() {
  return InterlockedIncrement(&v);
}

long decrement(long v) throw() {
  return InterlockedDecrement(&v);
}
#endif

namespace slib {

shared_count::~shared_count() {}

void shared_count::add_shared() throw() { increment(shared_owners_); }

bool shared_count::release_shared() throw() {
  if (decrement(shared_owners_) == -1) {
    on_zero_shared();
    return true;
  }
  return false;
}
}
