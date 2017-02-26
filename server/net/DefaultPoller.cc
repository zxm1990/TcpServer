#include "Poller.cc"
#include "PollPoller.cc"

Poller* Poller::newDefaultPoller()
{
	return new PollPoller;
}