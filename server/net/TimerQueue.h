#ifndef SERVER_NET_TIMERQUEUE_H
#define SERVER_NET_TIMERQUEUE_H

#include <list>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <server/base/Mutex.h>
#include <server/base/UtcTime.h>
#include <server/net/Channel.h>
namespace server
{

namespace net
{

class EventLoop;
class Timer;
class TimerId;

class TimerQueue
{
public:
	typedef boost::function<void()> TimerCallback;

	TimerQueue(EventLoop *loop);
	~TimerQueue();

	//schedules the callback to be run at given time
	//repeats if interval > 0.0
	// Must be thread safe, Usually be called from other threads
	TimerId schedule(const TimerCallback &cb, UtcTime when, double interval);

	void cancel(TimerId TimerId);

private:
	void timerout();//called when timerfds arms
	bool insertWithLockHold(Timer *timer);//insert timer in sorted list
	typedef std::list<Timer*> TimerList;

	EventLoop *loop_;
	// const int timerfd_;
	// Channel timerfdChannel_;
	MutexLock mutex_;
	TimerList timers_;//Timer list sorted by expiration
};

}
}

#endif //SERVER_NET_TIMERQUEUE_H