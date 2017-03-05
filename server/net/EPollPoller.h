#ifndef SERVER_NET_EPOLLPOLLER_H
#define SERVER_NET_EPOLLPOLLER_H

#include <server/net/Poller.h>

#include <map>
#include <vector>

namespace server
{

namespace net
{

//IO Multiplexing with epoll

class EPollPoller : public Poller
{
public:

	virtual ~EPollPoller();
	
	virtual void poll(int timeoutMs, ChannelList* activeChannels);

private:
	std::map<int, Channel*> channels_;
};
}
}


#endif //SERVER_NET_EPOLLPOLLER_H