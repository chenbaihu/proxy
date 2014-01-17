
#include <gtest/gtest.h>
#include <memory>
#include "MyThread.h"
#include "IRunnable.h"

class ThreadTest : public ::testing::Test {
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
std::shared_ptr<IRunnable> r=std::make_shared<R>(i);\
MyThread thr(r); \
thr.run(); \
auto ret = thr.wait(); \
ASSERT_EQ(i, ret); }

TEST_F(ThreadTest, test1) {
  TEST_RETVALUE(0);
  TEST_RETVALUE(1);
  TEST_RETVALUE(2);
  TEST_RETVALUE(-1);
}

class R2:public IRunnable{
  virtual unsigned svc(void) {
    throw std::runtime_error("test exception");
  }
};
TEST_F(ThreadTest, test2) {
  std::shared_ptr<IRunnable> r=std::make_shared<R2>();	
  MyThread thr(r);					
  thr.run();						
  thr.wait(); 
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
