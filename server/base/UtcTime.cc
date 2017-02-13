#include "UtcTime.h"

#include <sys/time.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

using namespace server;

UtcTime::UtcTime()
	: microSecondsSinceEpoch_(0)
{

}

UtcTime::UtcTime(long long microseconds)
	:microSecondsSinceEpoch_(microseconds)
{

}
std::string UtcTime::toString() const
{
	char buf[32] = {0};
	int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
	int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
	snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
	return buf;
}

UtcTime UtcTime::now()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	long long seconds = tv.tv_sec;
	return UtcTime(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

UtcTime UtcTime::invalid()
{
	return UtcTime();
}
