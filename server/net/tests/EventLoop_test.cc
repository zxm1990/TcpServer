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
	printf("pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
	
	sleep(10);
	{
		EventLoop loop;
		g_loop = &loop;
		print("main");
    	loop.runAfter(1, boost::bind(print, "once1"));
    	loop.runAfter(1.5, boost::bind(print, "once1.5"));
    	loop.runAfter(2.5, boost::bind(print, "once2.5"));
    	loop.runAfter(3.5, boost::bind(print, "once3.5"));
    	loop.runEvery(2, boost::bind(print, "every2"));
    	loop.runEvery(3, boost::bind(print, "every3"));

  		loop.loop();
    	print("exit");
	}

	sleep(30);
	
}