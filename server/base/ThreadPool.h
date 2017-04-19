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

//线程池
/*
* 线程池有任务队列
* 队列有大小，就必须有条件变量，来通知线程执行
*具体每个线程执行什么任务，需要将函数注册到线程对象中
*/
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
		threadInitCallback_ = cb;
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
	Task threadInitCallback_;
	boost::ptr_vector<server::Thread> threads_;
	std::deque<Task> queue_;
	size_t maxQueueSize_;
	bool running_;
};
}

#endif //SERVER_BASE_THREADPOOL_H