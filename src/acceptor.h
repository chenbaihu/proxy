#pragma once
#include <stdint.h>
#include <memory>  //shared_ptr
#include <unordered_map>
#include <event2/dns.h>

#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "listener.h"
#include "commontypes.h"

class ClientHandler;
class EvdnsWrapper;
class Acceptor :public slib::Listener{
 private:
  typedef slib::Listener super;
  typedef std::unordered_map<ConnectionIDType, ClientHandler*> HandlerMap;
  HandlerMap handlers;
  Acceptor(const Acceptor&) ;
  Acceptor& operator=(const Acceptor&) ;

 public:
  EvdnsWrapper* dns_;
  Acceptor(std::shared_ptr<event_base> base, unsigned flags, bool enableLocking, EvdnsWrapper* dns) :slib::Listener(base, flags, enableLocking), dns_(dns){}
  void remove(ConnectionIDType id);
  virtual void on_delete();
  virtual void on_new_conn_accepted(evutil_socket_t sock, struct sockaddr* addr, int addrlen);
};
