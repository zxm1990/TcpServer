#ifndef SERVER_NET_POLLER_H
#define SERVER_NET_POLLER_H

#include <vector>

namespace server
{

namespace net
{

class Channel;
/*
	Base class for IO Multiplexing
*/
class  Poller
{
public:
	typedef std::vector<Channel*> ChannelList;

	virtual ~Poller();

	virtual void poll(int timeroutMs, ChannelList *activeChannels) = 0;

	static Poller* newDefaultPoller();
	
};

}
}
#endif
