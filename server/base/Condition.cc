#include <server/base/Condition.h>

#include <errno.h>
#include <sys/time.h>

using namespace server;

//returns true if time out false otherwise
bool Condition::waitForSeconds(int seconds)
{
	struct timespec abstime;
#ifdef CLOCK_REALTIME
	clock_gettime(CLOCK_REALTIME, &abstime);
#else  //Mac OS X
	struct timeval tv;
	gettimeofday(&tv, NULL);
	abstime.tv_sec = tv.tv_sec;
	abstime.tv_nsec = tv.tv_usec * 1000;
#endif
	abstime.tv_sec += seconds;
	MutexLock::UnassignGuard ug(mutex_);
	return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
}