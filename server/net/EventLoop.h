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
	typedef boost::function<void()> Functor;
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

	//Runs callback immediately in the loop thread
	// It wakes up the loop and run the cb,
	//Safe to call from other threads
	void runInLoop(const Functor &cb);

	TimerId runAt(const UtcTime &time, const TimerCallback &cb);
	TimerId runAfter(double delay, const TimerCallback &cb);
	TimerId runEvery(double interval, const TimerCallback &cb);

	void cancel(TimerId timerId);

	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);

	pid_t threadId() 
	{
		return threadId_;
	}
	void assertInLoopThread();

private:
	void wakedup();

	typedef std::vector<Channel*> ChannelList;

	bool looping_;//atomic
	bool quit_;
	const pid_t threadId_;
	boost::scoped_ptr<Poller> poller_;
	boost::scoped_ptr<TimerQueue> timerQueue_;
	int wakeupFd_;
	//unlink in TimerQueue which is an internal class
	//we don't expose Channel to client
	boost::scoped_ptr<Channel> wakeupChannel_;
	ChannelList activeChannels_;
	
};
}
}

#endif  //SERVER_NET_EVENTLOOP_H