#ifndef SERVER_BASE_COUNTDOWNLATCH_H
#define SERVER_BASE_COUNTDOWNLATCH_H

#include <server/base/Condition.h>
#include <server/base/Mutex.h>

#include <boost/noncopyable.hpp>

namespace server
{

//倒计时
//主要用于一个主线程等待多个子线程
class CountDownLatch : boost::noncopyable
{
 public:

  explicit CountDownLatch(int count);

  void wait();

  void countDown();

  int getCount() const;

 private:
  mutable MutexLock mutex_;
  Condition condition_;
  int count_;
};

}
#endif  // SERVER_BASE_COUNTDOWNLATCH_H
