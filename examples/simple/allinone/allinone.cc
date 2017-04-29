#include "../chargen/chargen.h"
#include "../daytime/daytime.h"
#include "../discard/discard.h"
#include "../echo/echo.h"
#include "../time/time.h"

#include <server/base/Logging.h>
#include <server/net/EventLoop.h>

using namespace server;
using namespace server::net;

int main()
{
  LOG_INFO << "pid = " << getpid();
  EventLoop loop;  // one loop shared by multiple servers

  ChargenServer chargenServer(&loop, InetAddress(2019));
  chargenServer.start();

  DaytimeServer daytimeServer(&loop, InetAddress(2013));
  daytimeServer.start();

  DiscardServer discardServer(&loop, InetAddress(2009));
  discardServer.start();

  EchoServer echoServer(&loop, InetAddress(2007));
  echoServer.start();

  TimeServer timeServer(&loop, InetAddress(2037));
  timeServer.start();

  loop.loop();
}

