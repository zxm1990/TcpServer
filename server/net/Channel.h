#ifndef SERVER_NET_CHANNEL_H
#define SERVER_NET_CHANNEL_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <server/net/Socket.h>

namespace server
{

namespace net
{

class EventLoop;

class Channel : boost::noncopyable
{
public:
	typedef boost::function<void()> EventCallback;
	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;
	static const int kErrorEvent;

	Channel(EventLoop *loop, int fd);
	~Channel();

	void handle_event();
	void setReadCallback(const EventCallback &cb)
	{
		readCallback_ = cb;
	}

	void setWriteCallback(const EventCallback &cb)
	{
		writeCallback_ = cb;
	}

	void setErrorCallback(const EventCallback &cb)
	{
		errorCallback_ = cb;
	}

	int fd()
	{
		return fd_;
	}

	void set_events(int evt)
	{
		events_ = evt;
	}

	void set_revents(int revt)
	{
		revents_ = revt;
	}

	EventLoop* getLoop() {return loop_; }

private:
	EventLoop *loop_;
	int       fd_;
	int       events_;
	int       revents_;
	EventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback errorCallback_;
};
}
}


#endif //SERVER_NET_CHANNEL_H