#ifndef SERVER_BASE_THREADPOOL_H
#define SERVER_BASE_THREADPOOL_H

#include <server/base/Condition.h>
#include <server/base/Mutex.h>
#include <server/base/Thread.h>
#include <server/base/Types.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <deque>

namespace server
{

class ThreadPool : boost::noncopyable
{
public:
	typedef boost::function<void()> Task;

	explicit ThreadPool(const string& name = string("ThreadPool"));
	~ThreadPool();

	//Must be called before start
	void setMaxQueueSize(int maxSize)
	{
		maxQueueSize_ = maxSize;
	}

	void setThreadInitCallback(const Task &cb)
	{
		threadInitCallbakc_ = cb;
	}

	void start(int numThreads);
	void stop();

	//Could block if maxQueueSize > 0
	void run(const Task& f);

private:
	bool isFull() const;
	void runInThread();
	Task take();
	MutexLock mutex_;
	Condition notEmpty_;
	Condition notFull_;
	string name_;
	Task threadInitCallbakc_;
	boost::ptr_vector<server::Thread> threads_;
	std::deque<Task> queue_;
	size_t maxQueueSize_;
	bool running_;
};
}

#endif //SERVER_BASE_THREADPOOL_H