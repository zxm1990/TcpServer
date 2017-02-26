#ifndef SERVER_NET_TIMERQUEUE_H
#define SERVER_NET_TIMERQUEUE_H

#include <list>

#include <boost/function.hpp>

#include <server/base/UtcTime.h>

namespace server
{

namespace net
{

class Timer;
class TimerId;

class TimerQueue
{
public:
	typedef boost::function<void()> TimerCallback;

	TimerQueue();
	~TimerQueue();

	void tick(UtcTime now);

	//schedules the callback to be run at given time
	//repeats if interval > 0.0
	TimerId schedule(const TimerCallback &cb, UtcTime at, double interval);

	void cancel(TimerId TimerId);

private:
	typedef std::list<Timer*> Timers;
	Timers timers_;
};

}
}

#endif //SERVER_NET_TIMERQUEUE_H