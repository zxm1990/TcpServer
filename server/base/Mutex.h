#ifndef SERVER_BASE_MUTEX_H
#define SERVER_BASE_MUTEX_H

#include <pthread.h>

#include <boost/noncopyable.hpp>
#include <assert.h>
#include <server/base/CurrentThread.h>

namespace server
{

//RAII手法封装mutex资源
class MutexLock : boost::noncopyable
{
public:
	MutexLock()
		: holder_(0)
	{
		pthread_mutex_init(&mutex_, NULL);
	}
	~MutexLock()
	{
		assert(holder_ == 0);
		pthread_mutex_destroy(&mutex_);
	}

	//must be called when locked
	//判断是否在同一线程中已经加锁➕
	//避免死锁
	bool isLockedByThisThread() const
	{
		return holder_ == CurrentThread::tid();
	}

	void assertLocked() const
	{
		assert(isLockedByThisThread());
	}

	//加锁的同时，记录线程id
	void lock()
	{
		pthread_mutex_lock(&mutex_);
		assignHolder();
	}

	//释放锁的同时，将holder恢复
	//这里的tid最小值为1.
	void unlock()
	{
		unassignHolder();
		pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t* getPthreadMutex()
	{
		return &mutex_;
	}

private:
	friend class Condition;

	class UnassignGuard : boost::noncopyable
	{
	public:
		UnassignGuard(MutexLock &owner)
			: owner_(owner)
		{
			owner_.unassignHolder();
		}

		~UnassignGuard()
		{
			owner_.assignHolder();
		}
	private:
		MutexLock &owner_;
		
	};

	//tid最小值是1. 故可以为0表示空线程
	void unassignHolder()
	{
		holder_ = 0;
	}

	void assignHolder()
	{
		holder_ = CurrentThread::tid();
	}
	pthread_mutex_t mutex_;
	pid_t holder_;
	
};

class MutexLockGuard : boost::noncopyable
{
public:
	explicit MutexLockGuard(MutexLock &mutex)
					: mutex_(mutex)
	{
		mutex_.lock();
	}
	~MutexLockGuard()
	{
		mutex_.unlock();
	}
	
private:
	MutexLock &mutex_;
};

}

//避免错用锁，应该使用对象加锁。
#define MutexLockGuard(x)  error "Missing guard object name"

#endif //SERVER_BASE_MUTEX_H