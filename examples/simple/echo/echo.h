#ifndef SERVER_EXAMPLES_SIMPLE_ECHO_ECHO_H
#define SERVER_EXAMPLES_SIMPLE_ECHO_ECHO_H

#include <server/net/TcpServer.h>

// RFC 862
class EchoServer
{
 public:
  EchoServer(server::net::EventLoop* loop,
             const server::net::InetAddress& listenAddr);

  void start();  // calls server_.start();

 private:
  void onConnection(const server::net::TcpConnectionPtr& conn);

  void onMessage(const server::net::TcpConnectionPtr& conn,
                 server::net::Buffer* buf,
                 server::Timestamp time);

  server::net::TcpServer server_;
};

#endif  // SERVER_EXAMPLES_SIMPLE_ECHO_ECHO_H
