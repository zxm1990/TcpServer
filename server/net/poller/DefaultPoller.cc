#include <server/net/Poller.h>
#include <server/net/poller/PollPoller.h>
#include <server/net/poller/EPollPoller.h>

#include <stdlib.h>

using namespace server::net;

Poller* Poller::newDefaultPoller(EventLoop* loop)
{
#ifdef __MACH__
  return new PollPoller(loop);
#else
  if (::getenv("SERVER_USE_POLL"))
  {
    return new PollPoller(loop);
  }
  else
  {
    return new EPollPoller(loop);
  }
#endif
}
