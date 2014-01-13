#pragma once

#include <event2/dns.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include "commontypes.h"

class Acceptor;


class ClientHandler{
private:
  Acceptor* lis_;
  ConnectionIDType id_;
  bufferevent* bev_;
  bufferevent* remote_bev_;
  evbuffer* readbuf;
  uint8_t version;
  uint8_t nmethods;
  enum Socks5ServerState{
    VERSION,
    S5_NMETHODS,
    S5_METHODS,
    S5_REQ,
    CONNECTING,
    RESOLVING,
    PROXY,
  } state;
  ClientHandler(const ClientHandler&){};
  ClientHandler& operator=(const ClientHandler&){return *this;}
public:
  static void readcb(struct bufferevent *bev, void *ptr){
    ((ClientHandler*)ptr)->on_read(bev);
  }
  static void writecb(struct bufferevent *bev, void *ptr){
    ((ClientHandler*)ptr)->on_write(bev);
  }
  static void eventcb(bufferevent *bev, short events, void *ptr){
    ((ClientHandler*)ptr)->on_event(bev,events);
  }
  static void remote_eventcb(bufferevent *bev, short events, void *ptr){
    ((ClientHandler*)ptr)->on_remote_event(bev,events);
  }

  static void remote_readcb(struct bufferevent *bev, void *ptr){
    LOG(INFO)<<"read";
    ((ClientHandler*)ptr)->on_remote_read(bev);
  }


  void close();
  void on_read(bufferevent *bev);
  void on_remote_read(bufferevent *bev);
  void on_write(bufferevent *bev);
  void on_event(bufferevent *bev,short events);
  void on_remote_event(bufferevent *bev,short events);
  void on_remote_servername_resolved(int result, struct evutil_addrinfo *res);
  ClientHandler(Acceptor* lis,ConnectionIDType id,bufferevent* bev):lis_(lis),id_(id),bev_(bev),remote_bev_(NULL),readbuf(evbuffer_new()),state(VERSION){

  }
  ~ClientHandler() throw();
};

