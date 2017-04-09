#include "echo.h"

#include <server/base/Logging.h>
#include <server/net/EventLoop.h>


int main()
{
  LOG_INFO << "pid = " << getpid();
  server::net::EventLoop loop;
  server::net::InetAddress listenAddr(2007);
  EchoServer echoserver(&loop, listenAddr);
  echoserver.start();
  loop.loop();
}

