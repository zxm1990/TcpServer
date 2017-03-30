#ifndef SERVER_NET_POLLER_POLLPOLLER_H
#define SERVER_NET_POLLER_POLLPOLLER_H

#include <server/net/Poller.h>

#include <vector>

struct pollfd;

namespace server
{
namespace net
{

///
/// IO Multiplexing with poll(2).
///
class PollPoller : public Poller
{
 public:

  PollPoller(EventLoop* loop);
  virtual ~PollPoller();

  virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
  virtual void updateChannel(Channel* channel);
  virtual void removeChannel(Channel* channel);

 private:
  void fillActiveChannels(int numEvents,
                          ChannelList* activeChannels) const;

  typedef std::vector<struct pollfd> PollFdList;
  PollFdList pollfds_;
};

}
}
#endif  // SERVER_NET_POLLER_POLLPOLLER_H
