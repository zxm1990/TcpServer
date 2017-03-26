#ifndef SERVER_BASE_THREAD_H
#define SERVER_BASE_THREAD_H

#include <pthread.h>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <server/base/Atomic.h>
#include <server/base/Types.h>

namespace server
{

class Thread : boost::noncopyable
{
public:
	typedef boost::function<void()> ThreadFunc;

	explicit Thread(const ThreadFunc&, const string &name = string());
	~Thread();
	
	void start();
	int join(); //return pthread_join()

	bool start() const 
	{
		return started_;
	}

	pid_t tid() const 
	{
		return *tid_;
	}

	const string& name()
	{
		return name_;
	}

	//记录创建线程个数
	static int numCreated()
	{
		return numCreated_.get();
	}

private:
	void setDefaultName();

	bool started_;
	bool joined_;
	pthread_t pthreadId_;
	boost::shared_ptr<pid_t> tid_;
	ThreadFunc func_;
	string name_;

	static AtomicInt32 numCreated_;

};


}



#endif //SERVER_BASE_THREAD_H