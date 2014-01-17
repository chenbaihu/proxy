
#include <gtest/gtest.h>
#include <memory>
#include "Win32Thread.h"

class Win32ThreadTest : public ::testing::Test {
  virtual void SetUp() {}
  virtual void TearDown() {}
};

class  R:public IRunnable{
private:
  int value;
public:
  R(int i) :value(i){}
  virtual unsigned svc(void) {
    return value;
  }
};

#define TEST_RETVALUE(i) { \
std::auto_ptr<MyThread> thr(MyThread::create(new R(i))); \
thr->run(); \
auto ret = thr->wait(); \
ASSERT_EQ(i, ret); }

TEST_F(Win32ThreadTest, test1) {
  TEST_RETVALUE(0);
  TEST_RETVALUE(1);
  TEST_RETVALUE(2);
  TEST_RETVALUE(-1);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
