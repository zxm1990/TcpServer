#ifndef SERVER_EXAMPLES_SIMPLE_ECHO_ECHO_H
#define SERVER_EXAMPLES_SIMPLE_ECHO_ECHO_H

#include <server/net/TcpServer.h>

// 将收到的数据发给客户端
class EchoServer
{
 public:
  EchoServer(server::net::EventLoop* loop,
             const server::net::InetAddress& listenAddr);

  void start(); 

 private:
  void onConnection(const server::net::TcpConnectionPtr& conn);

  void onMessage(const server::net::TcpConnectionPtr& conn,
                 server::net::Buffer* buf,
                 server::Timestamp time);

  server::net::TcpServer server_;
};

#endif  // SERVER_EXAMPLES_SIMPLE_ECHO_ECHO_H
