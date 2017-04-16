#ifndef SERVER_EXAMPLES_SIMPLE_DISCARD_DISCARD_H
#define SERVER_EXAMPLES_SIMPLE_DISCARD_DISCARD_H

#include <server/net/TcpServer.h>

class DiscardServer
{
public:
	DiscardServer(server::net::EventLoop *loop,
				  const server::net::InetAddress & listenAddr);
	void start();

private:
	void onConnection(const server::net::TcpConnectionPtr &conn);

	void onMessage(const server::net::TcpConnectionPtr &conn,
				   server::net::Buffer *buf,
				   server::Timestamp time);

	server::net::TcpServer server_;
};



#endif //SERVER_EXAMPLES_SIMPLE_DISCARD_DISCARD_H