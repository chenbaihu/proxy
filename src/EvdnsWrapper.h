#pragma once
#include <event2/dns.h>
#include <string>
#include <vector>
#include "shared_count.h"

#define Q__DISABLE_COPY(Class) \
  Class(const Class &); \
  Class &operator=(const Class &);

class MyAddrInfo {

public:
  int              ai_flags;
  int              ai_family;
  int              ai_socktype;
  int              ai_protocol;
#ifdef OS_WIN
  size_t           ai_addrlen;
#else
  socklen_t        ai_addrlen;
#endif
  struct sockaddr *ai_addr;
  std::string      ai_canonname;
  MyAddrInfo(const evutil_addrinfo *res);
  MyAddrInfo(const MyAddrInfo &);
  MyAddrInfo &operator=(const MyAddrInfo &);
  std::string toString() const;
  ~MyAddrInfo() throw();
};

/**
 * new完之后如果你想用，请立即把引用计数加1
 */
class GetAddrCallBack :public slib::shared_count{
public:
  GetAddrCallBack() :slib::shared_count(0){}
  virtual void on(int result, const std::vector<MyAddrInfo>& res) = 0;
  virtual ~GetAddrCallBack() throw(){}
  virtual void on_zero_shared() throw() { delete this; }
};

class EvdnsWrapper
{
private:
  Q__DISABLE_COPY(EvdnsWrapper)
public:
  EvdnsWrapper(){}
  void init(event_base* ebase);
  EvdnsWrapper(event_base* ebase);
  void async_getaddrinfo(const char* nodename, const char* servname, evutil_addrinfo* hints, GetAddrCallBack* c);
  ~EvdnsWrapper();
private:
  evdns_base* evbase;
  static void on_remote_servername_resolved(int result, struct evutil_addrinfo *res,
    void *arg);
};

