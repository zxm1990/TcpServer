#ifndef SERVER_NET_TIMER_H
#define SERVER_NET_TIMER_H

#include <boost/noncopyable.hpp>

#include <server/base/Atomic.h>
#include <server/base/Timestamp.h>
#include <server/net/Callbacks.h>

namespace server
{
namespace net
{
///
/// Internal class for timer event.
///
class Timer : boost::noncopyable
{
 public:
  Timer(const TimerCallback& cb, Timestamp when, double interval)
    : callback_(cb),
      expiration_(when),
      interval_(interval),
      repeat_(interval > 0.0),
      sequence_(s_numCreated_.incrementAndGet())
  { }

  void run() const
  {
    callback_();
  }

  Timestamp expiration() const  { return expiration_; }
  bool repeat() const { return repeat_; }
  int64_t sequence() const { return sequence_; }

  void restart(Timestamp now);

  static int64_t numCreated() { return s_numCreated_.get(); }

 private:
  const TimerCallback callback_;
  Timestamp expiration_;
  const double interval_;
  const bool repeat_;
  const int64_t sequence_;

  static AtomicInt64 s_numCreated_;//单纯的序列号，表示创建了多个个定时器
};
}
}
#endif  // SERVER_NET_TIMER_H
