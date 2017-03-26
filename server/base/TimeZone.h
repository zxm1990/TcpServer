#ifndef SERVER_BASE_TIMEZONE_H
#define SERVER_BASE_TIMEZONE_H

#include <time.h>
#include <boost/shared_ptr.hpp>
#include <server/base/copyable.h>

namespace server
{

//TimerZone for 1970~2030
//时区转换
class TimeZone : public server::copyable
{
public:
	explicit TimeZone(const char *zonefile);
	TimeZone(int eastOfUtc, const char *tzname);
	//an invalid timezone
	TimeZone() 
	{

	}
	
	bool valid() const
	{
		return static_cast<bool>(data_);
	}

	struct tm toLocalTime(time_t secondsSinceEpoch) const;
	time_t fromLocalTime(const struct tm&) const;

	static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);
	static time_t fromUtcTime(const struct tm&);
	static time_t fromUtcTime(int year, int month, int day,
							  int hour, int minute, int seconds);
	struct Data;

private:
	boost::shared_ptr<Data> data_;
};

}


#endif //SERVER_BASE_TIMEZONE_H