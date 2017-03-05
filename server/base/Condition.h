#ifndef SERVER_BASE_CONDITION_H
#define SERVER_BASE_CONDITION_H

#include <server/base/Mutex.h>
#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace server
{

class Condition : boost::noncopyable
{
public:
	explicit Condition(MutexLock &mutex)
		: mutex_(mutex)
	{
		pthread_cond_init(&pcond_, NULL);
	}

	~Condition()
	{
		pthread_cond_detroy(&pcond_);
	}

	void wait()
	{
		MutexLock::unassignGuard ug(mutex_);
		pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
	}

	bool waitForSeconds(int seconds);

	void notify()
	{
		pthread_cond_signal(&pcond_);
	}

	void notifyAll()
	{
		pthread_cond_broadcase(&pcond_);
	}

private:
	MutexLock &mutex_;
	pthread_cond_t pcond_;
	
};
}

#endif //SERVER_BASE_CONDITION_H