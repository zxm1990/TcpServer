#ifndef SERVER_NET_EVENTLOOP_H
#define SERVER_NET_EVENTLOOP_H 

#include <vector>

#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <server/base/UtcTime.h>
#include <server/net/TimerId.h>

namespace server
{

namespace net
{
class Channel;
class Poller;
class TimerQueue;

// Reactor, at most one per thread;

class EventLoop : boost::noncopyable
{
public:
	typedef boost::function<void()> TimerCallback;

	EventLoop();
	~EventLoop();
	/*
		Loops forever
		Must be called in the same thread as creation of
		the object
	*/
	void loop();
	void quit();
	void wakeup();

	TimerId runAt(const UtcTime &time, const TimerCallback &cb);
	TimerId runAfter(double delay, const TimerCallback &cb);
	TimerId runEvery(double interval, const TimerCallback &cb);

	void cancel(TimerId timerId);

	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);

private:
	void init();
	typedef std::vector<Channel*> ChannelList;

	boost::scoped_ptr<Poller> poller_;
	boost::scoped_ptr<TimerQueue> timerQueue_;
	bool looping_;//atomic
	bool quit_;
	pid_t thread_;
	ChannelList activeChannels_;
	
};
}
}

#endif  //SERVER_NET_EVENTLOOP_H