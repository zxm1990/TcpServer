#ifndef SERVER_EXAMPLES_SIMPLE_CHARGEN_CHARGEN_H
#define SERVER_EXAMPLES_SIMPLE_CHARGEN_CHARGEN_H

#include <server/net/TcpServer.h>

//只发送数据，不接受数据
//发送数据的速度不能快过客户端接收的速度
class ChargenServer
{
public:
	ChargenServer(server::net::EventLoop *loop,
				  const server::net::InetAddress &listenAddr,
				  bool print = false);

	void start();

private:
	void onConnection(const server::net::TcpConnectionPtr &conn);

	void onMessage(const server::net::TcpConnectionPtr &conn,
				   server::net::Buffer *buf,
				   server::Timestamp time);

	void onWriteComplete(const server::net::TcpConnectionPtr &conn);

	void printThroughput();

	server::net::TcpServer server_;
	server::string message_;
	int64_t transferred_;
	server::Timestamp startTime_; 
};

#endif // SERVER_EXAMPLES_SIMPLE_CHARGEN_CHARGEN_H