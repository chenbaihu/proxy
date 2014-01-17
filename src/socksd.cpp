#include <iostream>
#include <memory>

#include <event2/event.h>
#include <event2/thread.h>
#include <string.h>  //memset

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

#include "MyThread.h"
#include "libevent_thread.h"

static void usage(){
  fprintf(stderr,"Usage: proxy -f config_file_path\n");
}

int my_main(int argc, char *argv[]) {
  std::string configFileName;
  int ch;
  while ((ch = slib::getopt(argc, argv, "dhf:")) != -1) {
    switch (ch) {
    case 'f':
      configFileName = slib::optarg;
      if (!CONFIG->parseFromFile(configFileName)){
        std::cout << "load config file fail\n";
        return -1;
      }
      break;
    case 'd':
      CONFIG->enableEventDebug = true;
      break;
    case 'h':
    case '?':
    default:
      usage();
      return -1;
    }
  }
  MYDEBUG("start");
  std::shared_ptr<IRunnable> r=std::make_shared<LibeventThread>();
  MyThread thr(r);
  thr.run();
  unsigned ret=thr.wait();
  return ret;
}
#if !defined(WIN32)
int main(int argc, char *argv[]) {
  int ret=my_main(argc,argv);
  return ret;
}
#endif
