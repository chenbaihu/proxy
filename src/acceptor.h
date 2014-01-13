#pragma once
#include <stdint.h>
#include <memory>  //shared_ptr
#include <unordered_map>
#include <event2/dns.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "commontypes.h"

class ClientHandler;

class Acceptor {
 private:
  typedef std::unordered_map<ConnectionIDType, ClientHandler*> HandlerMap;
  HandlerMap handlers;
  Acceptor(const Acceptor&) {};
  Acceptor& operator=(const Acceptor&) { return *this; }

 public:
  std::shared_ptr<evdns_base> dns_;
  std::shared_ptr<evconnlistener> evlistener;
  event_base* get_event_base();
  Acceptor(std::shared_ptr<evdns_base> dns) : dns_(dns) {};
  void remove(ConnectionIDType id);
  ~Acceptor() throw();
  void on_new(struct evconnlistener* listener, evutil_socket_t sock,
              struct sockaddr* addr, int addrlen);
};
