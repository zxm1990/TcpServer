#ifndef SERVER_NET_EVENTLOOPTHREADPOOL_H
#define SERVER_NET_EVENTLOOPTHREADPOOL_H

#include <vector>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace server
{

namespace net
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : boost::noncopyable
{
 public:
  typedef boost::function<void(EventLoop*)> ThreadInitCallback;

  EventLoopThreadPool(EventLoop* baseLoop);
  ~EventLoopThreadPool();
  void setThreadNum(int numThreads) { numThreads_ = numThreads; }
  void start(const ThreadInitCallback& cb = ThreadInitCallback());

  // valid after calling start()
  /// round-robin
  EventLoop* getNextLoop();

  /// with the same hash code, it will always return the same EventLoop
  EventLoop* getLoopForHash(size_t hashCode);

  std::vector<EventLoop*> getAllLoops();

  bool started() const
  { return started_; }

 private:

  EventLoop* baseLoop_;
  bool started_;
  int numThreads_;
  int next_;
  boost::ptr_vector<EventLoopThread> threads_;
  std::vector<EventLoop*> loops_;
};

}
}

#endif  // SERVER_NET_EVENTLOOPTHREADPOOL_H
