#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <server/net/TimerQueue.h>

#include <server/base/Logging.h>
#include <server/net/EventLoop.h>
#include <server/net/Timer.h>
#include <server/net/TimerId.h>

#include <boost/bind.hpp>

namespace server
{
namespace net
{
namespace detail
{

//与当前时间的时间间隔，秒
int howMuchTimeFromNow(Timestamp when)
{
  int64_t microseconds = when.microSecondsSinceEpoch()
                         - Timestamp::now().microSecondsSinceEpoch();
  if (microseconds < 1000)
  {
    microseconds = 1000;
  }
  return static_cast<int>(microseconds / 1000);
}

}
}
}

using namespace server;
using namespace server::net;
using namespace server::net::detail;

TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timers_(),
    callingExpiredTimers_(false)
{
}

//释放timer的内存
TimerQueue::~TimerQueue()
{
  // do not remove channel, since we're in EventLoop::dtor();
  for (TimerList::iterator it = timers_.begin();
      it != timers_.end(); ++it)
  {
    delete it->second;
  }
}

//回调函数注册到timer中，时间到时会去调用
TimerId TimerQueue::addTimer(const TimerCallback& cb,
                             Timestamp when,
                             double interval)
{
	//申请内存空间，TimerQueue对象析构时释放。
  Timer* timer = new Timer(cb, when, interval);
  //在同一线程中插入定时
  loop_->runInLoop(
      boost::bind(&TimerQueue::addTimerInLoop, this, timer));
  return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId)
{
  loop_->runInLoop(
      boost::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
  loop_->assertInLoopThread();
  insert(timer);
}

int TimerQueue::getTimeout() const
{
  loop_->assertInLoopThread();
  if (timers_.empty())
  {
    return 10000;
  }
  else
  {
    return howMuchTimeFromNow(timers_.begin()->second->expiration());
  }
}

//取消定时任务，注意activeTimers和timers的一致性
void TimerQueue::cancelInLoop(TimerId timerId)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  ActiveTimer timer(timerId.timer_, timerId.sequence_);
  ActiveTimerSet::iterator it = activeTimers_.find(timer);
  if (it != activeTimers_.end())
  {
    size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
    assert(n == 1); (void)n;
    delete it->first; // FIXME: no delete please
    activeTimers_.erase(it);
  }
  else if (callingExpiredTimers_)
  {
  	//正在执行一个被取消的任务，先记录，下次执行时再取消
    cancelingTimers_.insert(timer);
  }
  assert(timers_.size() == activeTimers_.size());
}

void TimerQueue::processTimers()
{
  loop_->assertInLoopThread();
  Timestamp now(Timestamp::now());

  std::vector<Entry> expired = getExpired(now);

  callingExpiredTimers_ = true;
  cancelingTimers_.clear();
  // safe to callback outside critical section
  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
  	//调用构造timerQueue对象时注册的回调函数
    it->second->run();
  }
  callingExpiredTimers_ = false;

  reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
  assert(timers_.size() == activeTimers_.size());
  std::vector<Entry> expired;
  Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));

  //返回第一个未到期的Timer
  TimerList::iterator end = timers_.lower_bound(sentry);
  assert(end == timers_.end() || now < end->first);

  //将到期的Timer拷贝
  std::copy(timers_.begin(), end, back_inserter(expired));

  //删除timers_ 和 acitveTimers_中到期的Timer
  //注意这个时候并没有delete掉资源，在reset中delete
  timers_.erase(timers_.begin(), end);
  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());
    size_t n = activeTimers_.erase(timer);
    assert(n == 1); (void)n;
  }

  assert(timers_.size() == activeTimers_.size());
  return expired;
}

//如果是重复的定时任务，需要重新insert到容器中
void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
  Timestamp nextExpire;

  for (std::vector<Entry>::const_iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());

    //是循环定时任务 ，并且没有被取消掉
    if (it->second->repeat()
        && cancelingTimers_.find(timer) == cancelingTimers_.end())
    {
      it->second->restart(now);
      insert(it->second);
    }
    else
    {
      // FIXME move to a free list
      delete it->second; // FIXME: no delete please
    }
  }

  if (!timers_.empty())
  {
    nextExpire = timers_.begin()->second->expiration();
  }
}

bool TimerQueue::insert(Timer* timer)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  bool earliestChanged = false;
  Timestamp when = timer->expiration();
  TimerList::iterator it = timers_.begin();
  if (it == timers_.end() || when < it->first)
  {
    earliestChanged = true;
  }
  {
    std::pair<TimerList::iterator, bool> result
      = timers_.insert(Entry(when, timer));
    assert(result.second); (void)result;
  }
  {
    std::pair<ActiveTimerSet::iterator, bool> result
      = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    assert(result.second); (void)result;
  }

  assert(timers_.size() == activeTimers_.size());
  return earliestChanged;
}

