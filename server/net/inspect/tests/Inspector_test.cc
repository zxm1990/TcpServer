#include <server/net/inspect/Inspector.h>
#include <server/net/EventLoop.h>
#include <server/net/EventLoopThread.h>

using namespace server;
using namespace server::net;

int main()
{
  EventLoop loop;
  EventLoopThread t;
  Inspector ins(t.startLoop(), InetAddress(12345), "test");
  loop.loop();
}

