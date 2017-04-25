#include "daytime.h"

#include <server/base/Logging.h>
#include <server/net/EventLoop.h>

using namespace server;
using namespace server::net;

int main()
{
  LOG_INFO << "pid = " << getpid();
  EventLoop loop;
  InetAddress listenAddr(2013);
  DaytimeServer Dayserver(&loop, listenAddr);
  Dayserver.start();
  loop.loop();
}

