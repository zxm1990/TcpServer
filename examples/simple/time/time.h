#ifndef SERVER_EXAMPLES_SIMPLE_TIME_TIME_H
#define SERVER_EXAMPLES_SIMPLE_TIME_TIME_H

#include <server/net/TcpServer.h>

//以二进制形式发送当前时间，并主动断开连接
class TimeServer
{
public:
	TimeServer(server::net::EventLoop *loop,
			  const server::net::InetAddress &listenAddr);

	void start();

private:
	void onConnection(const server::net::TcpConnectionPtr &conn);

	void onMessage(const server::net::TcpConnectionPtr &conn,
				   server::net::Buffer *buf,
				   server::Timestamp time);

	server::net::TcpServer server_;
};

#endif //SERVER_EXAMPLES_SIMPLE_TIME_TIME_H