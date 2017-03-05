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

	virtual void updateChannel(Channel* channel);

private:
	void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

	typedef std::vector<struct pollfd> PollFdList;
	typedef std::map<int, Channel*> ChannelMap;
	PollFdList pollfds_;
	ChannelMap channels_;
	
};
}
}

#endif //SERVER_NET_POLLPOLLER_H