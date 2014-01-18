#include "EvdnsWrapper.h"

#include <sstream>
#ifdef OS_WIN
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#endif

MyAddrInfo::MyAddrInfo(const struct addrinfo *res) :ai_flags(res->ai_flags), ai_family(res->ai_family), ai_socktype(res->ai_socktype), ai_protocol(res->ai_protocol), ai_addrlen(res->ai_addrlen){
  if (res->ai_canonname)
    ai_canonname = res->ai_canonname;
  ai_addr = (sockaddr *)new char[res->ai_addrlen];
  memcpy(this->ai_addr, res->ai_addr, res->ai_addrlen);
}

MyAddrInfo::MyAddrInfo(const MyAddrInfo &res) : ai_flags(res.ai_flags), ai_family(res.ai_family), ai_socktype(res.ai_socktype), ai_protocol(res.ai_protocol), ai_addrlen(res.ai_addrlen), ai_canonname(res.ai_canonname){
  ai_addr = (sockaddr *)new char[res.ai_addrlen];
  memcpy(this->ai_addr, res.ai_addr, res.ai_addrlen);
}

MyAddrInfo& MyAddrInfo::operator = (const MyAddrInfo &res){
  if (ai_addr) delete ai_addr;
  ai_flags = res.ai_flags;
  ai_family = res.ai_family;
  ai_socktype = res.ai_socktype;
  ai_protocol = res.ai_protocol;
  ai_addrlen = res.ai_addrlen;
  ai_canonname = res.ai_canonname;
  ai_addr = (sockaddr *)new char[res.ai_addrlen];
  memcpy(this->ai_addr, res.ai_addr, res.ai_addrlen);
  return *this;
}

std::string MyAddrInfo::toString() const{
  char hostbuf[NI_MAXHOST];
  char portbuf[NI_MAXSERV];
  getnameinfo(this->ai_addr, this->ai_addrlen, hostbuf, sizeof(hostbuf), portbuf, sizeof(portbuf), NI_NUMERICHOST | NI_NUMERICSERV);
  std::ostringstream oss;
  oss << "addr=" << hostbuf << ",port=" << portbuf << ",ai_flags=" << ai_flags << ",ai_family=" << ai_family << ",ai_socktype=" << ai_socktype << ",ai_protocol=" << ai_protocol << ",ai_canonname=" << ai_canonname;
  return oss.str();
}

MyAddrInfo::~MyAddrInfo() throw(){
  if (ai_addr) delete ai_addr;
}

EvdnsWrapper::EvdnsWrapper(event_base* ebase)
{
  init(ebase);
}

void EvdnsWrapper::init(event_base* ebase){
  evbase = evdns_base_new(ebase, 1);
  if (!evbase)
    throw std::runtime_error("init EvdnsWrapper fail");
}

EvdnsWrapper::~EvdnsWrapper() throw(){
  if (evbase) evdns_base_free(evbase, 0);
}

std::vector<MyAddrInfo> createMyAddrInfo(struct evutil_addrinfo *res){
  std::vector<MyAddrInfo> ret;
  for (evutil_addrinfo *rp = res; rp != NULL; rp = rp->ai_next) {
    ret.push_back(MyAddrInfo(rp));
  }
  return ret;
}

void EvdnsWrapper::on_remote_servername_resolved(int result, struct evutil_addrinfo *res,
  void *arg) {
  GetAddrCallBack *c = (GetAddrCallBack *)arg;
  std::vector<MyAddrInfo> ret;
  if (result == 0){
    for (evutil_addrinfo *rp = res; rp != NULL; rp = rp->ai_next) {
      ret.push_back(MyAddrInfo(rp));
    }
    evutil_freeaddrinfo(res);
  }
  c->on(result, ret);
  c->release_shared();
}
void EvdnsWrapper::async_getaddrinfo(const char* nodename, const char* servname, evutil_addrinfo* hints, GetAddrCallBack* c){
  c->add_shared();
  evdns_getaddrinfo(this->evbase, nodename, servname, hints, on_remote_servername_resolved, c);
}