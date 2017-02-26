#ifndef SERVER_NET_CHANNEL_H
#define SERVER_NET_CHANNEL_H

#include "Socket.h"

namespace server
{

namespace net
{

class EventLoop;

class Channel
{
public:
	Channel(EventLoop *loop, Socket sock);
	~Channel();

	void handle(int revents);

	EventLoop* getLoop() {return loop_; }

private:
	EventLoop *loop_;
	Socket    sock_;
	int       events_;
	
};
}
}


#endif //SERVER_NET_CHANNEL_H