#include <server/net/PollPoller.h>

#include <poll.h>

using namespace server;
using namespace server::net;

PollPoller::~PollPoller()
{

}

void PollPoller::poll(int timeoutMs)
{
	PollFdList pollfds(pollfds_);
	int numEvents = ::poll(&*pollfds.begin(), pollfds.size(), timeoutMs);

	for (PollFdList::iterator pfd = pollfds.begin(); pfd != pollfds.end() && numEvents > 0; ++pfd)
	{
		if (pfd->revents > 0)
		{
			--numEvents;
		}
	}
}