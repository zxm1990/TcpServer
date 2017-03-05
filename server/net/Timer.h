#ifndef SERVER_NET_TIMER_H
#define SERVER_NET_TIMER_H

#include <map>

#include <boost/function.hpp>

#include <server/base/UtcTime.h>

namespace server
{

namespace net
{

	//Internal class for timer event
class Timer
{
public:
	typedef boost::function<void()> TimerCallback;
	Timer(const TimerCallback &cb, UtcTime when, double interval)
		: cb_(cb),
		  expiration_(when),
		  interval_(interval),
		  repeat_(interval > 0.0)
	{

	}

	void run() const
	{
		cb_();
	}

	UtcTime expiration() const { return expiration_; }
	bool repeat() const { return repeat_; }

	void restart(UtcTime now);

private:
	const TimerCallback cb_;
	UtcTime expiration_;
	const double interval_;
	const bool repeat_;
};
}
}

#endif // SERVER_NET_TIMER_H