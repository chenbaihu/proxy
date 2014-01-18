#include <event2/event.h>
#include <gtest/gtest.h>
#include <iostream>
#include <curl/curl.h>
#include <memory>

#include "EvdnsWrapper.h"
#include "MyThread.h"
#include "IRunnable.h"
#ifdef OS_WIN
#include "WsaModule.h"
#endif

class EvdnsWrapperTest : public ::testing::Test {
  virtual void SetUp() {}
  virtual void TearDown() {}
};

class ResolveCallback : public GetAddrCallBack{
private:
  event_base* base;
public:
  ResolveCallback(event_base* b) :base(b){}
  void on(int result, const std::vector<MyAddrInfo>& res){
    int index = 1;
    for (std::vector<MyAddrInfo>::const_iterator iter = res.begin(); iter != res.end(); ++iter){
      std::cout << index++<<" "<<iter->toString() << std::endl;
    }
    event_base_loopexit(base, NULL);
  }
};
class TestThread :public IRunnable{
  virtual unsigned svc(void) {
    event_base* base=event_base_new();
    EvdnsWrapper w(base);
    evutil_addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; 
    hints.ai_flags = EVUTIL_AI_NUMERICSERV;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    w.async_getaddrinfo("www.google.com", "80", &hints, new ResolveCallback(base));
    event_base_loop(base, 0);
    event_base_free(base);
    return 0;
  }
};

class TestThread2 :public IRunnable{
  virtual unsigned svc(void) {
    event_base* base = event_base_new();
    EvdnsWrapper w(base);
    evutil_addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_flags = EVUTIL_AI_NUMERICSERV;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    w.async_getaddrinfo("www.google.com", "80", &hints, new ResolveCallback(base));
    event_base_loop(base, 0);
    event_base_free(base);
    return 0;
  }
};

TEST_F(EvdnsWrapperTest, test1) {
  std::shared_ptr<TestThread> r(new TestThread());
  MyThread thr(r);  
  thr.run();
  thr.wait();
}

TEST_F(EvdnsWrapperTest, test2) {
  std::shared_ptr<TestThread2> r(new TestThread2());
  MyThread thr(r);
  thr.run();
  thr.wait();
}

int main(int argc, char* argv[]) {
#ifdef OS_WIN
  WSA_MODULE::instance();
#endif
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
