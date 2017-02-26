#include "TimerQueue.h"
#include "Timer.h"
#include "TimerId.h"

using namespace server;
using namespace server::net;

TimerQueue::TimerQueue()
{

}

TimerQueue::~TimerQueue()
{
	
}

void TimerQueue::tick(UtcTime now)
{

}

TimerId TimerQueue::schedule(const TimerCallback &cb, UtcTime at, double interval)
{
	Timer *timer = new Timer(cb, at, interval);
	timers_.push_front(timer);
	return TimerId(timer);
}