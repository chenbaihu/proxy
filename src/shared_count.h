#pragma once

namespace slib {
template <class T>
inline T increment(T& t) throw() {
  return __sync_add_and_fetch(&t, 1);
}

template <class T>
inline T decrement(T& t) throw() {
  return __sync_add_and_fetch(&t, -1);
}

class shared_count {
  shared_count(const shared_count&);
  shared_count& operator=(const shared_count&);

 protected:
  long shared_owners_;
  virtual ~shared_count();

 private:
  virtual void on_zero_shared() throw() = 0;

 public:
  explicit shared_count(long refs = 0) throw() : shared_owners_(refs) {}

  void add_shared() throw();
  bool release_shared() throw();
  long use_count() const throw() { return shared_owners_ + 1; }
};
}
