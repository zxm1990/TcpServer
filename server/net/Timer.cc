#include "Timer.h"

using namespace server;
using namespace server::net;

void Timer::restart(UtcTime now)
{
	if (repeat_)
	{
		expiration_ = addTime(now, interval_);
	}
	else
	{
		expiration_ = UtcTime::invalid();
	}
}