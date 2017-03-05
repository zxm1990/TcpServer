#include <server/net/Poller.h>
#include <server/net/PollPoller.h>
#include <server/net/EPollPoller.h>

using namespace server;
using namespace server::net;

Poller* Poller::newDefaultPoller()
{
	return new PollPoller;
}