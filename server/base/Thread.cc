#include <server/base/Thread.h>
#include <server/base/CurrentThread.h>
#include <server/base/Exception.h>
#include <server/base/Logging.h>

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/weak_ptr.hpp>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#ifndef __MACH__
#include <sys/prctl.h>
#include <linux/unistd.h>
#endif

namespace server
{
namespace CurrentThread
{
  __thread int t_cachedTid = 0;
  __thread char t_tidString[32];
  __thread int t_tidStringLength = 6;
  __thread const char* t_threadName = "unknown";
  const bool sameType = boost::is_same<int, pid_t>::value;
  BOOST_STATIC_ASSERT(sameType);
}

namespace detail
{

#ifdef __MACH__
pid_t gettid()
{
  	return pthread_mach_thread_np(pthread_self());
}
#else
pid_t gettid()
{
  	return static_cast<pid_t>(::syscall(SYS_gettid));
}
#endif

void afterFork()
{
	  server::CurrentThread::t_cachedTid = 0;
	  server::CurrentThread::t_threadName = "main";
	  CurrentThread::tid();
}

class ThreadNameInitializer
{
 public:
	  ThreadNameInitializer()
	  {
		    server::CurrentThread::t_threadName = "main";
		    CurrentThread::tid();
		    pthread_atfork(NULL, NULL, &afterFork);
	  }
};

ThreadNameInitializer init;

struct ThreadData
{
	  typedef server::Thread::ThreadFunc ThreadFunc;
	  ThreadFunc func_;
	  string name_;
	  boost::weak_ptr<pid_t> wkTid_;

	  ThreadData(const ThreadFunc& func,
	             const string& name,
	             const boost::shared_ptr<pid_t>& tid)
		    : func_(func),
		      name_(name),
		      wkTid_(tid)
	  { 

	  }

  void runInThread()
  {
	    pid_t tid = server::CurrentThread::tid();

	    boost::shared_ptr<pid_t> ptid = wkTid_.lock();
	    if (ptid)
	    {
		      *ptid = tid;
		      ptid.reset();
	    }

    server::CurrentThread::t_threadName = name_.empty() ? "serverThread" : name_.c_str();
#ifndef __MACH__
    ::prctl(PR_SET_NAME, server::CurrentThread::t_threadName);
#endif
    try
    {
      func_();
      server::CurrentThread::t_threadName = "finished";
    }
    catch (const Exception& ex)
    {
      server::CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
      fprintf(stderr, "reason: %s\n", ex.what());
      fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
      abort();
    }
    catch (const std::exception& ex)
    {
      server::CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
      fprintf(stderr, "reason: %s\n", ex.what());
      abort();
    }
    catch (...)
    {
      server::CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
      throw; // rethrow
    }
  }
};

void* startThread(void* obj)
{
  ThreadData* data = static_cast<ThreadData*>(obj);
  data->runInThread();
  delete data;
  return NULL;
}

}
}

using namespace server;

void CurrentThread::cacheTid()
{
  if (t_cachedTid == 0)
  {
    t_cachedTid = detail::gettid();
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

bool CurrentThread::isMainThread()
{
  return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
  struct timespec ts = { 0, 0 };
  ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
  ::nanosleep(&ts, NULL);
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const string& n)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(new pid_t(0)),
    func_(func),
    name_(n)
{
  setDefaultName();
}

Thread::~Thread()
{
  if (started_ && !joined_)
  {
  	//设置为unjoinable状态，系统自动回收资源
    pthread_detach(pthreadId_);
  }
}

void Thread::setDefaultName()
{
  int num = numCreated_.incrementAndGet();
  if (name_.empty())
  {
    char buf[32];
    snprintf(buf, sizeof buf, "Thread%d", num);
    name_ = buf;
  }
}

void Thread::start()
{
  assert(!started_);
  started_ = true;
  // FIXME: move(func_)
  detail::ThreadData* data = new detail::ThreadData(func_, name_, tid_);
  if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
  {
    started_ = false;
    delete data; // or no delete?
    LOG_SYSFATAL << "Failed in pthread_create";
  }
}

int Thread::join()
{
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}

