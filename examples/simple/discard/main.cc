#include "discard.h"

#include <server/base/Logging.h>
#include <server/net/EventLoop.h>

using namespace server;
using namespace server::net;

int main()
{
	LOG_INFO << "pid = " << getpid();
	EventLoop loop;
	InetAddress listenAddr(2000);
	DiscardServer server(&loop, listenAddr);
	loop.loop();
}