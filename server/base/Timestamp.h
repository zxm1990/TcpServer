#ifndef SERVER_BASE_TIMESTAMP_H
#define SERVER_BASE_TIMESTAMP_H

#include <server/base/copyable.h>
#include <server/base/Types.h>

#include <boost/operators.hpp>


namespace server
{

//时刻
class Timestamp : public server::copyable,
                  public boost::less_than_comparable<Timestamp>
{
 public:

  //默认初始化一个不合法的时间
  Timestamp()
    : microSecondsSinceEpoch_(0)
  {
  }

  explicit Timestamp(int64_t microSecondsSinceEpoch);

  //时间交换
  void swap(Timestamp& that)
  {
      std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
  }

  string toString() const;
  string toFormattedString(bool showMicroseconds = true) const;

  //时间是否合法
  bool valid() const 
  { 
      return microSecondsSinceEpoch_ > 0; 
  }

  // 时间，微妙
  int64_t microSecondsSinceEpoch() const 
  { 
      return microSecondsSinceEpoch_; 
  }

  //时间，秒
  time_t secondsSinceEpoch() const
  { 
      return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond); 
  }

  static Timestamp now();

  static Timestamp invalid();

  static const int kMicroSecondsPerSecond = 1000 * 1000;

 private:
  int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

//时间间隔，秒
inline double timeDifference(Timestamp high, Timestamp low)
{
  int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
  return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

//时间向后延迟，秒
inline Timestamp addTime(Timestamp timestamp, double seconds)
{
  int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
  return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}

#endif  //SERVER_BASE_TIMESTAMP_H