#ifndef SERVER_NET_POLLER_H
#define SERVER_NET_POLLER_H

namespace server
{

namespace net
{
/*
	Base class for IO Multiplexing
*/
class  Poller
{
public:

	virtual ~Poller();

	virtual void poll(int timeroutMs) = 0;

	static Poller* newDefaultPoller();
	
};

}
}
#endif
