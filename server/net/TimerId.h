#ifndef SERVER_NET_TIMERID_H
#define SERVER_NET_TIMERID_H

namespace server
{

namespace net
{

class Timer;

class TimerId
{

public:
	explicit TimerId(Timer *timer)
		: value_(timer)
	{

	}

private:
	Timer *value_;

};
}
}

#endif // SERVER_NET_TIMERID_H