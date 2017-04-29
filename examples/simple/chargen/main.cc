#include "chargen.h"

#include <server/base/Logging.h>
#include <server/net/EventLoop.h>

using namespace server;
using namespace server::net;

int main()
{
	LOG_INFO << "pid = " << getpid();
  	EventLoop loop;
  	InetAddress listenAddr(2019);
  	ChargenServer chargenServer(&loop, listenAddr, true);
  	chargenServer.start();
  	loop.loop();
}