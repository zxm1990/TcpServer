#include "Channel.h"

using namespace server;
using namespace server::net;

Channel::Channel(EventLoop *loop, Socket sock)
	: loop_(loop),
	  sock_(sock),
	  events_(0)
{

}

Channel::~Channel()
{
	
}