#include <server/net/EventLoop.h>
#include <server/base/Thread.h>

#include <boost/bind.hpp>

#include <cstdio>
#include <unistd.h>

using namespace server;
using namespace server::net;

int cnt = 0;
EventLoop *g_loop;

void print(const char *msg)
{
	printf("msg %s %s\n", UtcTime::now().toString().c_str(), msg);
  	if (++cnt == 20)
  	{
    	g_loop->quit();
  	}
}

int main()
{
	printf("pid = %d\n", getpid());
	EventLoop theLoop;

	print();

	theLoop.runAfter(1.0, print);
	theLoop.loop();

	printf("test success!\n");
}