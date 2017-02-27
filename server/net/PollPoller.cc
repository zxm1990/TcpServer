#include <server/net/PollPoller.h>

#include <server/net/Channel.h>

#include <assert.h>

using namespace server;
using namespace server::net;

PollPoller::~PollPoller()
{

}

void PollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
	int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);

	for (PollFdList::iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && numEvents > 0; ++pfd)
	{
		if (pfd->revents > 0)
		{
			--numEvents;
			Channel *channel = channels_[pfd->fd];
			assert(channel->fd() == pfd->fd);
			channel->set_revents(pfd->revents);
			activeChannels->push_back(channel);
		}
	}
}