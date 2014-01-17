#include <event2/event.h>
#include <event2/thread.h>
#include "acceptor.h"
#include "ClientHandler.h"
#include "logger.h"
#include "my_getopt.h"
#include "commontypes.h"
#include "googleinit.h"
#include "ObjectManager.h"
#include "singleton.h"
#include "port.h"
#include "Config.h"

#ifdef OS_WIN
#include <Ws2tcpip.h>
#include "WsaModule.h"
#endif
#include "LibEventInit.h"

#include "libevent_thread.h"

static void my_free_event_base(event_base *ebase) {
  MYDEBUG("free event_base");
  if (ebase) event_base_free(ebase);
  return;
}

static void my_free_event_dns_base_imm(evdns_base *base) {
  evdns_base_free(base, 0);
}


unsigned LibeventThread::svc(void){
#ifdef OS_WIN
  WSA_MODULE::instance();
#endif
  LIBEVENTINIT::instance();

    std::shared_ptr<event_base> ebase(event_base_new(), my_free_event_base);
  if (!ebase) {
    MYDEBUG("create event base fail");
    return -1;
  }

  std::shared_ptr<evdns_base> dns_base(evdns_base_new(ebase.get(), 1),
    my_free_event_dns_base_imm);
  if (!dns_base) {
    MYDEBUG("create event dns base fail");
    return -1;
  }
  const char *port = "1080";
  evutil_addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;  // AF_UNSPEC; /* v4 or v6 is fine. */
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;     /* We want a TCP socket */
  hints.ai_flags = EVUTIL_AI_PASSIVE;  // or EVUTIL_AI_ADDRCONFIG ?
  struct addrinfo *res;
  int error;
  if ((error = evutil_getaddrinfo(NULL, port, &hints, &res)) != 0) {
    MYDEBUG("Unable to resolve port %s:%s", port, evutil_gai_strerror(error));
    return -1;
  }
  std::shared_ptr<addrinfo> tmp_(res,evutil_freeaddrinfo);
  Acceptor* lis=new Acceptor(ebase, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, false, dns_base);
  for (evutil_addrinfo *ai = res; ai != NULL; ai = ai->ai_next) {
    char xhost[1024];
    char xport[64];
    if (getnameinfo(ai->ai_addr, ai->ai_addrlen, xhost, sizeof(xhost), xport,
      sizeof(xport), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
      MYDEBUG("Listening on %s:%s", xhost, xport);
    }

    if (lis->open(ai->ai_addr, ai->ai_addrlen) == 0) {
      MYDEBUG("create listener successfully");
      break;
    }
  }

  MYDEBUG("enter event loop");
  event_base_loop(ebase.get(), 0);

  // It was introduced in Libevent 2.1.1-alpha.
  // libevent_global_shutdown();
  lis->destory();

  return 0;
}
