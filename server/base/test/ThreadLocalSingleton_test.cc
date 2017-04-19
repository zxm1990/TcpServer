#include <server/base/ThreadLocalSingleton.h>
#include <server/base/CurrentThread.h>
#include <server/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <stdio.h>

class Test : boost::noncopyable
{
public:
	Test()
	{
		printf("tid = %d, construction %p\n", server::CurrentThread::tid(), this);
	}

	~Test()
	{
		printf("tid = %d, destruction %p, name = %s\n", server::CurrentThread::tid(), this, name_.c_str());
	}

	const server::string& name() const 
	{
		return name_;
	}

	void setName(const server::string& n)
	{
		name_ = n;
	}

private:
	server::string name_;
};

void threadFunc(const char *changeTo)
{
	printf("tid = %d, %p name = %s\n", 
			server::CurrentThread::tid(),
			&server::ThreadLocalSingleton<Test>::instance(),
			server::ThreadLocalSingleton<Test>::instance().name().c_str());

	server::ThreadLocalSingleton<Test>::instance().setName(changeTo);

	printf("tid = %d, %p name = %s\n", 
			server::CurrentThread::tid(),
			&server::ThreadLocalSingleton<Test>::instance(),
			server::ThreadLocalSingleton<Test>::instance().name().c_str());

}

int main()
{
	server::ThreadLocalSingleton<Test>::instance().setName("main one");
	server::Thread t1(boost::bind(threadFunc, "thread1"));
	server::Thread t2(boost::bind(threadFunc, "thread2"));
	t1.start();
	t2.start();
	t1.join();
	printf("tid = %d, %p name = %s\n", 
			server::CurrentThread::tid(),
			&server::ThreadLocalSingleton<Test>::instance(),
			server::ThreadLocalSingleton<Test>::instance().name().c_str());

	t2.join();

	pthread_exit(0);
}