#include "shared_count.h"
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
