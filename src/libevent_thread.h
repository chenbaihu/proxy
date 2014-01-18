#include "IRunnable.h"
#include <event2/event.h>
#include <event2/dns.h>
#include "EvdnsWrapper.h"

class LibeventThread:public IRunnable{
public:
  virtual unsigned svc(void) ;
private:
  std::shared_ptr<event_base> ebase;
  EvdnsWrapper dns;
};
