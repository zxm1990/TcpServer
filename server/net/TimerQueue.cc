#include <server/net/TimerQueue.h>
#include <server/net/Timer.h>
#include <server/net/TimerId.h>
#include <server/net/EventLoop.h>

#include <boost/bind.hpp>

#include <stdio.h>
#include <sys/timerfd.h>

using namespace server;
using namespace server::net;

namespace
{

int creatTimerfd()
{
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if (timerfd < 0)
	{
		perror("Failed in timerfd_create!\n");
		abort();
	}

	return timerfd;
}

}


TimerQueue::TimerQueue(EventLoop *loop)
	: loop_(loop),
	  timerfd_(creatTimerfd()),
	  timerfdChannel_(loop, timerfd_),
	  timers_()
{
	timerfdChannel_.setReadCallback(boost::bind(&TimerQueue::timerout, this));
}

TimerQueue::~TimerQueue()
{
	::close(timerfd_);
	//do not remove channel, since in EventLoop::dtor()
}

void TimerQueue::timeout()
{
	timerfdChannel_.set_events(0);
	loop_->updateChannel(&timerfdChannel_);
}

TimerId TimerQueue::schedule(const TimerCallback &cb, UtcTime at, double interval)
{
	Timer *timer = new Timer(cb, at, interval);
	timers_.push_front(timer);
	timerfdChannel_.set_events(Channel::kReadEvent);
	loop_->updateChannel(&timerfdChannel_);
	return TimerId(timer);
}