#include "chargen.h"

#include <server/base/Logging.h>
#include <server/net/EventLoop.h>

#include <boost/bind.hpp>
#include <stdio.h>

using namespace server;
using namespace server::net;

ChargenServer::ChargenServer(EventLoop *loop,
							 const InetAddress &listenAddr,
							 bool print)
	: server_(loop, listenAddr, "ChargenServer"),
	  transferred_(0),
	  startTime_(Timestamp::now())
{
	server_.setConnectionCallback(boost::bind(&ChargenServer::onConnection, this, _1));
	server_.setMessageCallback(boost::bind(&ChargenServer::onMessage, this, _1, _2, _3));
	server_.setWriteCompleteCallback(boost::bind(&ChargenServer::onWriteComplete, this, _1));

	if (print)
	{
		loop->runEvery(3.0, boost::bind(&ChargenServer::printThroughput, this));
	}

	string line;
	for (int i = 33; i < 127; ++i)
	{
		line.push_back(char(i));
	}
	line += line;
	for (size_t i = 0; i < 127-33; ++i)
  	{
    	message_ += line.substr(i, 72) + '\n';
  	}
}

void ChargenServer::start()
{
	server_.start();
}

void ChargenServer::onConnection(const TcpConnectionPtr &conn)
{
	LOG_INFO << "ChargenServer - " << conn->peerAddress().toIpPort() << " -> "
           	 << conn->localAddress().toIpPort() << " is "
           	 << (conn->connected() ? "UP" : "DOWN");

    //数据不要存储，有数据就发送
    //第一次连接建立就发送数据
    if (conn->connected())
    {
    	conn->setTcpNoDelay(true);
    	conn->send(message_);
    }
}

void ChargenServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time)
{
	//取出只读数据, 直接丢弃
	string msg(buf->retrieveAllAsString());
	LOG_INFO << conn->name() << " discards " << msg.size()
             << " bytes received at " << time.toString();
}

//避免客户端接收不及时，需要writeComplete函数
void ChargenServer::onWriteComplete(const TcpConnectionPtr &conn)
{
	transferred_ += message_.size();
	conn->send(message_);
}

void ChargenServer::printThroughput()
{
	Timestamp endTime = Timestamp::now();
	double time = timeDifference(endTime, startTime_);
	printf("%4.3f MiB/s\n", static_cast<double>(transferred_)/time/1024/1024);
	transferred_ = 0;
	startTime_ = endTime;
}

