#ifndef SERVER_NET_EVENTLOOPTHREAD_H
#define SERVER_NET_EVENTLOOPTHREAD_H

#include <server/base/Condition.h>
#include <server/base/Mutex.h>
#include <server/base/Thread.h>

#include <boost/noncopyable.hpp>

namespace server
{
namespace net
{

class EventLoop;

class EventLoopThread : boost::noncopyable
{
 public:
  typedef boost::function<void(EventLoop*)> ThreadInitCallback;

  EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
  ~EventLoopThread();
  EventLoop* startLoop();

 private:
  void threadFunc();

  EventLoop* loop_;
  bool exiting_;
  Thread thread_;
  MutexLock mutex_;
  Condition cond_;
  ThreadInitCallback callback_;
};

}
}

#endif  // SERVER_NET_EVENTLOOPTHREAD_H

