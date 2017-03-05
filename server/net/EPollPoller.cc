#include <server/net/EPollPoller.h>
#include <server/net/Channel.h>

#include <assert.h>
//#include <sys/epoll.h>

using namespace server;
using namespace net;

EPollPoller::~EPollPoller()
{

}

void EPollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{

}