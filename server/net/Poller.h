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

	//Polls the I/O events
	//must be called in the loop thread
	virtual void poll(int timeroutMs, ChannelList *activeChannels) = 0;

	//changes the interested I/O events
	//Must be called in the loop thread
	virtual void updateChannel(Channel* channel) = 0;

	static Poller* newDefaultPoller();
	
};

}
}
#endif
