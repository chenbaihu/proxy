#pragma once

#include <event2/util.h>
#include <event2/event.h>
#include <memory>

struct event_iocp_port;
namespace slib {

  class Listener;

  //TODO: 把这两个callback变成Listener的虚函数？

  /**
  A callback that we invoke when a listener has a new connection.

  @param listener The Listener
  @param fd The new file descriptor
  @param addr The source address of the connection
  @param socklen The length of addr
  @param user_arg the pointer passed to Listener_new()
  */
  typedef void(*Listener_cb)(Listener *, evutil_socket_t,
  struct sockaddr *, int socklen, void *);

  /**
  A callback that we invoke when a listener encounters a non-retriable error.

  @param listener The Listener
  @param user_arg the pointer passed to Listener_new()
  */
  typedef void(*Listener_errorcb)(Listener *, void *);

  /** Flag: Indicates that we should not make incoming sockets nonblocking
  * before passing them to the callback. */
#define LEV_OPT_LEAVE_SOCKETS_BLOCKING (1u << 0)
  /** Flag: Indicates that freeing the listener should close the underlying
  * socket. */
#define LEV_OPT_CLOSE_ON_FREE (1u << 1)
  /** Flag: Indicates that we should set the close-on-exec flag, if possible */
#define LEV_OPT_CLOSE_ON_EXEC (1u << 2)
  /** Flag: Indicates that we should disable the timeout (if any) between when
  * this socket is closed and when we can listen again on the same port. */
#define LEV_OPT_REUSEABLE (1u << 3)
  

  class Listener {
  private:
    event *listener;
  protected:
    std::shared_ptr<event_base> ebase;
  public:
    Listener(std::shared_ptr<event_base> base, unsigned flags, bool enableLocking);

    /**
     *\param flags Any number of LEV_OPT_* flags
     *\param backlog Passed to the listen() call to determine the length of the
     *\acceptable connection backlog.  Set to -1 for a reasonable default.
     *\Set to 0 if the socket is already listening.
     *\param fd The file descriptor to listen on.  It must be a nonblocking
     *\file descriptor, and it should already be bound to an appropriate
     *\port and address.
     *
     */
    int open(evutil_socket_t fd,  int backlog) ;
    int open(const struct sockaddr *sa, int socklen, int backlog=128);
   
    virtual ~Listener() throw();

    /**
    Re-enable an Listener that has been disabled.
    */
    int enable(){
      int ret = event_add(listener, NULL);;
        this->enabled = ret == 0;
        return ret;
    }
    /**
    Stop listening for connections on an Listener.
    */
    int disable(){ this->enabled = false; return event_del(listener); }

    /** Return an Listener's associated event_base. */
    event_base *get_base(){ return event_get_base(listener); }

    /** Return the socket that an evconnlistner is listening on. */
    evutil_socket_t get_fd(){ return event_get_fd(listener); }

    
    virtual void on_delete();

    void destory();
  protected:
    void *lock;
    unsigned flags;
    short refcnt;
    bool enabled;
   
    virtual void shutdown_impl() {/*do nothing */
    }

    void on_new_conn_accepted(evutil_socket_t, struct sockaddr *, int socklen);
    void on_fatal_error();
    bool decref_and_unlock();
    void read_cb(evutil_socket_t fd, short what);
  private:
    static void listener_read_cb(evutil_socket_t fd, short what, void *p);
  };


}
