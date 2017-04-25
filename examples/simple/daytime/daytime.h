#ifndef SERVER_EXAMPLES_SIMPLE_DAYTIME_DAYTIME_H
#define SERVER_EXAMPLES_SIMPLE_DAYTIME_DAYTIME_H

#include <server/net/TcpServer.h>

class DaytimeServer
{
public:
	DaytimeServer(server::net::EventLoop *loop,
				const server::net::InetAddress &listenAddr);
	void start();

private:
	void onConnection(const server::net::TcpConnectionPtr &conn);

	void onMessage(const server::net::TcpConnectionPtr &conn,
					server::net::Buffer *buf,
					server::Timestamp time);

	server::net::TcpServer server_;
};

#endif //SERVER_EXAMPLES_SIMPLE_DAYTIME_DAYTIME_H