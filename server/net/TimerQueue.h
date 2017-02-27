#ifndef SERVER_NET_TIMERQUEUE_H
#define SERVER_NET_TIMERQUEUE_H

#include <list>

#include <boost/function.hpp>

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

//	void tick(UtcTime now);

	//schedules the callback to be run at given time
	//repeats if interval > 0.0
	TimerId schedule(const TimerCallback &cb, UtcTime at, double interval);

	void cancel(TimerId TimerId);

private:
	void timerout();

	typedef std::list<Timer*> Timers;

	EventLoop *loop_;
	int timerfd_;
	Channel timerfdChannel_;
	Timers timers_;
};

}
}

#endif //SERVER_NET_TIMERQUEUE_H