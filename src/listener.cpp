#include <event2/event_struct.h>

#include "listener.h"
#include "commontypes.h"
#include "logger.h"

namespace slib{
  Listener::Listener(std::shared_ptr<event_base> base, unsigned flags, bool enableLocking) :listener(nullptr), ebase(base), refcnt(1){
    if (enableLocking){
      //TODO:create lock
      //EVTHREAD_ALLOC_LOCK(lev->base.lock, EVTHREAD_LOCKTYPE_RECURSIVE);
    }
  };
#ifdef OS_WIN
  static int setKeepAlive(evutil_socket_t fd){
    int on = 1;
    return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char*)&on, sizeof(on));
  }
#else
  static int setKeepAlive(evutil_socket_t fd){
    int on = 1;
    return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const void*)&on, sizeof(on));
  }
#endif

  //passed to libevent's callback
	class evconnlistener_event{
	public:
 		Listener* base;
		event listener;
	};	

  
  Listener::Listener():lock(NULL),flags(0),refcnt(0),enabled(false){
     
  }

  Listener::~Listener() throw() {

  }

  void Listener::on_new_conn_accepted(evutil_socket_t, struct sockaddr *, int socklen){
    MYDEBUG("get a new connection");
  }

  int Listener::open(const struct sockaddr *sa, int socklen, int backlog){
    int family = sa ? sa->sa_family : AF_UNSPEC;
    if (backlog == 0)
      return -1;
    evutil_socket_t fd = socket(family, SOCK_STREAM, 0);
    if (fd == -1)
      return -1;
    if (evutil_make_socket_nonblocking(fd) < 0) {
      evutil_closesocket(fd);
      return -1;
    }

    if (flags & LEV_OPT_CLOSE_ON_EXEC) {
      if (evutil_make_socket_closeonexec(fd) < 0) {
        evutil_closesocket(fd);
        return -1;
      }
    }

    if (setKeepAlive(fd)) {
      evutil_closesocket(fd);
      return -1;
    }
    if (flags & LEV_OPT_REUSEABLE) {
      if (evutil_make_listen_socket_reuseable(fd) < 0) {
        evutil_closesocket(fd);
        return -1;
      }
    }

    if (sa) {
      if (bind(fd, sa, socklen)<0) {
        evutil_closesocket(fd);
        return -1;
      }
    }

    int ret = this->open(fd, backlog);
    if (ret <0){
      evutil_closesocket(fd);
      return ret;
    }
    return 0;
  }
 
  void Listener::destory(){
    //TODO:LOCK(lev);
    this->decref_and_unlock();
  }

  int Listener::open(evutil_socket_t fd, int backlog){
    if (backlog != 0){
      if (backlog < 0)
        backlog = 128; //try to pick a good value for the backlog
      if (listen(fd, backlog) < 0)
        return -1;
    } //else  you have already called listen() on the socket you are providing it.
    event * ev = event_new(this->ebase.get(), fd, EV_READ | EV_PERSIST,
      &Listener::listener_read_cb, this);
    if (!ev){
      return -1;
    }
    this->listener=ev;
    this->enable();
    return 0;
  }


inline bool isSocketValid(evutil_socket_t fd){
#ifdef OS_WIN
  return fd != INVALID_SOCKET;
#else
  return fd >=0;
#endif
}

void Listener::on_delete(){
  event_del(listener);
  if (this->flags & LEV_OPT_CLOSE_ON_FREE)
    evutil_closesocket(event_get_fd(this->listener));
  event_free(this->listener);
}
bool Listener::decref_and_unlock(){
  int refcnt = --this->refcnt;
  if (refcnt == 0) {
    on_delete();
    //UNLOCK(listener);
    //EVTHREAD_FREE_LOCK(listener->lock, EVTHREAD_LOCKTYPE_RECURSIVE);
    delete this;
    return true;
  }
  else {
    //UNLOCK(listener);
    return false;
  }
}

#ifdef OS_WIN
#define MY_ERR_ACCEPT_RETRIABLE(e)			((e) == WSAEWOULDBLOCK ||	(e) == WSAEINTR)
#else
/* True iff e is an error that means a accept can be retried. */
#define MY_ERR_ACCEPT_RETRIABLE(e)			\
  ((e) == EINTR || (e) == EAGAIN || (e) == ECONNABORTED)
#endif

void Listener::on_fatal_error() {
  MYERROR("Error from accept() call");
};

void Listener::read_cb(evutil_socket_t fd, short what){
  
  //TODO:locker
  //LOCK(lev);
  while (true) {
    struct sockaddr_storage ss;
#ifdef WIN32
    int socklen = sizeof(ss);
#else
    socklen_t socklen = sizeof(ss);
#endif
    evutil_socket_t new_fd = accept(fd, (struct sockaddr*)&ss, &socklen);
    if (!isSocketValid(new_fd)) break;

    if (!(this->flags & LEV_OPT_LEAVE_SOCKETS_BLOCKING))
      evutil_make_socket_nonblocking(new_fd);


    ++this->refcnt;
    //TODO
    //UNLOCK(lev);
    this->on_new_conn_accepted(new_fd,(struct sockaddr*)&ss, (int)socklen);    
    //LOCK(lev);
    if (this->refcnt == 1) {
      MYASSERT(this->decref_and_unlock());      
      return;
    } else --this->refcnt;
  }
  int err = evutil_socket_geterror(fd);
  if (!MY_ERR_ACCEPT_RETRIABLE(err)) {
    ++this->refcnt;
    //UNLOCK(lev);
    this->on_fatal_error();
    //LOCK(lev);
    this->decref_and_unlock();
  } 
  //TODO
  //UNLOCK(lev);  
}

void Listener::listener_read_cb(evutil_socket_t fd, short what, void *p){
  Listener *lev = (Listener*)p;
  lev->read_cb(fd, what);
}
}
