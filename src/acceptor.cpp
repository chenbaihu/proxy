#include <glog/logging.h>

#include "acceptor.h"
#include "ClientHandler.h"

static ConnectionIDType getNewConnId(){
  static ConnectionIDType id(0);
  return ++id;
}


void Acceptor::on_new(struct evconnlistener *listener,
              evutil_socket_t sock, struct sockaddr *addr, int addrlen){
    char xhost[1024];
    char xport[64];
    if (getnameinfo(addr, addrlen, xhost, sizeof(xhost), xport,
                    sizeof(xport), NI_NUMERICSERV) == 0){
      LOG(INFO)<<"got a new connection from "<<xhost<<":"<<xport<<".";
    } else LOG(INFO)<<"got a new connection";
    event_base* base=evconnlistener_get_base(listener);
    ConnectionIDType cid=getNewConnId();
    //TODO:add BEV_OPT_THREADSAFE??
    bufferevent* bev=bufferevent_socket_new(base,sock,BEV_OPT_CLOSE_ON_FREE);
    ClientHandler* h=new ClientHandler(this,cid,bev);
    bufferevent_setcb(bev,&ClientHandler::readcb,&ClientHandler::writecb,&ClientHandler::eventcb,h);
    bufferevent_enable(bev, EV_READ); /* Start reading. */
    handlers[cid]=h;
  }

void Acceptor::remove(ConnectionIDType id){
  HandlerMap::iterator iter=this->handlers.find(id);
  if(iter!=handlers.end()){
    delete iter->second;
    this->handlers.erase(iter);
  }

}

Acceptor::~Acceptor() throw(){
  for(std::pair<ConnectionIDType, ClientHandler*> h:handlers) {
    delete h.second;
  }
}
