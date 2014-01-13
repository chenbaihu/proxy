#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <assert.h>

struct evbuffer;
class MyData {
 private:
  uint8_t* data_;
  uint8_t* rdptr_;
  uint8_t* wptr_;
  size_t cap_;

 public:
  MyData() : data_(nullptr), cap_(0) {};

  /**alloc a buffer of size cap */
  MyData(size_t cap = 0);
  MyData(const MyData& obj);

  /** read a single byte from this buffer
   * \return true, ok; false,no more data to read
   */
  bool read(uint8_t& out);

  /**
   * WARN: do you really need this?
   * \return the beginning of the buffer.
   */
  uint8_t* data() const { return this->data_; }
  uint8_t* end() const { return this->data_ + this->cap_; }
  uint8_t* wptr() const { return this->wptr_; }
  size_t size() const { return this->cap_; }
  /**
   * how many bytes we can write
   */
  size_t remain() const {
    assert(wptr_ >= rdptr_);
    return this->cap_ - (this->wptr_ - this->rdptr_);
  }
  void ensure(size_t len);
  /**
   * rdptr == wptr ??
   */
  bool isEOF() const { return rdptr_ == wptr_; }

  ~MyData() throw() { delete[] data_; }

  void writeBuffer(evbuffer* input);
};
