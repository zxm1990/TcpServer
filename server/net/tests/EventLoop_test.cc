#include <server/net/EventLoop.h>
#include <cstdio>
using namespace server::net;

int main()
{
	EventLoop theLoop;
	theLoop.loop();
	
	printf("test success!\n");
}