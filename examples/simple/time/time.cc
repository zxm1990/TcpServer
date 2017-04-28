#include "time.h"

#include <server/base/Logging.h>
#include <server/net/Endian.h>

#include <boost/bind.hpp>

using namespace server;
using namespace server::net;

TimeServer::TimeServer(EventLoop *loop,
						const InetAddress &listenAddr)
	: server_(loop, listenAddr, "TimeServer")
{
	server_.setConnectionCallback(boost::bind(&TimeServer::onConnection, this, _1));
	server_.setMessageCallback(boost::bind(&TimeServer::onMessage, this, _1, _2, _3));
}

void TimeServer::start()
{
	server_.start();
}

void TimeServer::onConnection(const TcpConnectionPtr &conn)
{
	LOG_INFO << "TimeServer - " << conn->peerAddress().toIpPort() << " -> "
    		 << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
    if (conn->connected())
    {
    	time_t now = ::time(NULL);
    	int32_t be32 = sockets::hostToNetwork32(static_cast<int32_t>(now));
    	//将当前时间发送
    	conn->send(&be32, sizeof(int32_t));
    	conn->shutdown();
    }
}

void TimeServer::onMessage(const TcpConnectionPtr &conn, Buffer* buf, Timestamp time)
{
	//丢弃客户端发过来的数据
	string msg(buf->retrieveAllAsString());
	LOG_INFO << conn->name() << " discards " << msg.size()
             << " bytes received at " << time.toString();
}
