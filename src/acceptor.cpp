
#ifdef OS_WIN
#include <Ws2tcpip.h>
#endif

#include "acceptor.h"
#include "ClientHandler.h"
#include "logger.h"

static ConnectionIDType getNewConnId() {
  static ConnectionIDType id(0);
  return ++id;
}


void Acceptor::on_new_conn_accepted(evutil_socket_t sock, struct sockaddr *addr, int addrlen) {
  char xhost[1024];
  char xport[64];
  if (getnameinfo(addr, addrlen, xhost, sizeof(xhost), xport, sizeof(xport),
	               NI_NUMERICHOST|NI_NUMERICSERV) == 0) {
    MYDEBUG("got a new connection from %s:%s", xhost, xport);
  } else
    MYDEBUG("got a new connection");
  event_base *base = this->get_base();
  ConnectionIDType cid = getNewConnId();
  // TODO:add BEV_OPT_THREADSAFE??
  bufferevent *bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
  ClientHandler *h = new ClientHandler(this, cid, bev);
  bufferevent_setcb(bev, &ClientHandler::readcb, &ClientHandler::writecb,
                    &ClientHandler::eventcb, h);
  bufferevent_enable(bev, EV_READ); /* Start reading. */
  handlers[cid] = h;
}

void Acceptor::remove(ConnectionIDType id) {
  HandlerMap::iterator iter = this->handlers.find(id);
  if (iter != handlers.end()) {
    delete iter->second;
    this->handlers.erase(iter);
  }
}

void Acceptor::on_delete(){
  for (std::pair<ConnectionIDType, ClientHandler *> h : handlers) {
    delete h.second;
  }
  super::on_delete();
}

