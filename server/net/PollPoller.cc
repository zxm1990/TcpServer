#include <server/net/PollPoller.h>

#include <server/net/Channel.h>

#include <assert.h>
#include <stdio.h>


using namespace server;
using namespace server::net;

PollPoller::~PollPoller()
{

}

void PollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
	int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
	if (numEvents > 0)
	{
		fillActiveChannels(numEvents, activeChannels);
	}
	else if (numEvents == 0)
	{
		printf("nothing\n");
	}
	else
	{
		perror("PollPoller::poll");
	}

}

void PollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
	for (PollFdList::iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && numEvents > 0; ++pfd)
	{
		if (pfd->revents > 0)
		{
			--numEvents;
			ChannelMap::const_iterator ch = channels_.find(pfd->fd);
			assert(ch != channels_.end());
			Channel *channle = ch->second;
			assert(channel->fd() == pfd->fd);
			channel->set_revents(pfd->revents);
			activeChannels->push_back(channel);
		}
	}
}

void PollPoller::updateChannel(Channel* channel)
{
	if (channel->index() < 0)
	{
		// a new one, add to pollfd_
		assert(channels_.find(channel->fd()) == channels_.end());
		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds_.push_back(pfd);
		channel->set_index(static_cast<int>(pollfds_.size())-1);
		channels_[pfd.fd] = channel;
	}
	else
	{
		//update existing one
		assert(channels_.find(channel->fd()) != channels_.end());
		assert(channels_[channel->fd)] == channle);
		int idx = channel->index();
		assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
		struct pollfd &pfd = pollfds_[idx];
		assert(pfd.fd == channel->fd() || pfd.fd == -1);
		pfd.events = static_cast<short>(channel->events());
		if (pfd.events == 0)
		{
			//ignore this pollfd
			pfd.fd = -1;
			printf("set pfd.fd = -1 for fd = %d\n", channel->fd());
		}
	}
}




