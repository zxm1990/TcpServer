#include <server/base/Thread.h>
#include <server/base/CurrentThread.h>

#include <string>
#include <boost/bind.hpp>
#include <stdio.h>
#include <unistd.h>

//睡眠几秒
void mysleep(int seconds)
{
	timespec t = {seconds, 0};
	nanosleep(&t, NULL);
}

void threadFunc()
{
	printf("tid = %d\n", server::CurrentThread::tid());
}

void threadFunc2(int x)
{
	printf("tid = %d, x = %d\n", server::CurrentThread::tid(), x);
}

void threadFunc3()
{
	printf("tid = %d\n", server::CurrentThread::tid());
	mysleep(1);
}

class Foo
{
public:
	explicit Foo(double x)
		: x_(x)
	{

	}

	void memberFunc()
	{
		printf("tid = %d, Foo::x = %f\n", server::CurrentThread::tid(), x_);
	}

	void memberFunc2(const std::string &text)
	{
		printf("tid = %d, Foo::x = %f, text = %s\n", server::CurrentThread::tid(), x_, text.c_str());
	}

private:
	double x_;
};

int main()
{
	//主线程
	printf("pid = %d, tid = %d\n", ::getpid(), server::CurrentThread::tid());

	server::Thread t1(threadFunc);
	t1.start();
	t1.join();

	server::Thread t2(boost::bind(threadFunc2, 32), "thread for free function with argument");
	t2.start();
	t2.join();

	Foo foo(43.43);
	server::Thread t3(boost::bind(&Foo::memberFunc, &foo), "thread for member function without argument");
	t3.start();
	t3.join();

	server::Thread t4(boost::bind(&Foo::memberFunc2, boost::ref(foo), std::string("zxm1990")));
	t4.start();
	t4.join();

	{
		server::Thread t5(threadFunc3);
		t5.start();
	}

	mysleep(2);
	{
		server::Thread t6(threadFunc3);
		t6.start();
		mysleep(2);
	}

	sleep(2);
	printf("number of created threads %d\n", server::Thread::numCreated());
}

