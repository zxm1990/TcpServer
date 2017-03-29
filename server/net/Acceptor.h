#ifndef SERVER_NET_ACCEPTOR_H
#define SERVER_NET_ACCEPTOR_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <server/net/Channel.h>
#include <server/net/Socket.h>

namespace server
{
namespace net
{

class EventLoop;
class InetAddress;

///
/// Acceptor of incoming TCP connections.
/// 给tcpServer类使用
class Acceptor : boost::noncopyable
{
 public:
  typedef boost::function<void (int sockfd,
                                const InetAddress&)> NewConnectionCallback;

  Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
  ~Acceptor();

  void setNewConnectionCallback(const NewConnectionCallback& cb)
  { newConnectionCallback_ = cb; }

  bool listenning() const { return listenning_; }
  void listen();

 private:
  void handleRead();

  EventLoop* loop_;
  Socket acceptSocket_;
  Channel acceptChannel_;
  NewConnectionCallback newConnectionCallback_;
  bool listenning_;
  int idleFd_;
};

}
}

#endif  // SERVER_NET_ACCEPTOR_H
