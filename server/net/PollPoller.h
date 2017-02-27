#ifndef SERVER_NET_POLLPOLLER_H
#define SERVER_NET_POLLPOLLER_H

#include <server/net/Poller.h>

#include <map>
#include <vector>

#include <poll.h>

namespace server
{

namespace net
{

// IO Multiplexing with poll

class PollPoller : public Poller
{
public:

	virtual ~PollPoller();

	virtual void poll(int timeoutMs, ChannelList *activeChannels);

private:
	typedef std::vector<struct pollfd> PollFdList;
	PollFdList pollfds_;
	std::map<int, Channel*> channels_;
	
};
}
}

#endif //SERVER_NET_POLLPOLLER_H