#ifndef SERVER_BASE_UTCTIME_H__
#define SERVER_BASE_UTCTIME_H__

#include "Types.h"

namespace server
{

class UtcTime
{
public:

	UtcTime();

	explicit UtcTime(long long microSecondsSinceEpoch);

	std::string toString() const;

	bool valid() const { return microSecondsSinceEpoch_ > 0; }

	bool before(UtcTime rhs) const
	{
		return microSecondsSinceEpoch_ < rhs.microSecondsSinceEpoch_;
	}

	bool after(UtcTime rhs) const
	{
		return microSecondsSinceEpoch_ > rhs.microSecondsSinceEpoch_;
	}

	bool equals(UtcTime rhs) const
	{
		return microSecondsSinceEpoch_ == rhs.microSecondsSinceEpoch_;
	}

	int microSecondsSinceEpoch() const 
	{
		return microSecondsSinceEpoch_;
	}

	static UtcTime now();
	static UtcTime invalid();

	static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
	long long microSecondsSinceEpoch_;
};

inline bool operator<(UtcTime lhs, UtcTime rhs)
{
	return lhs.before(rhs);
}

inline bool operator==(UtcTime lhs, UtcTime rhs)
{
	return lhs.equals(rhs);
}

inline double timeDifference(UtcTime high, UtcTime low)
{
	long long diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
	return static_cast<double>(diff) / UtcTime::kMicroSecondsPerSecond;
}

inline UtcTime addTime(UtcTime timestamp, double seconds)
{
	long long delta = static_cast<long long> (seconds * UtcTime::kMicroSecondsPerSecond);
	return UtcTime(timestamp.microSecondsSinceEpoch() + delta);
}

}

#endif