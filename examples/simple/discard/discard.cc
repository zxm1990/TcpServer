#include "discard.h"

#include <server/base/Logging.h>

#include <boost/bind.hpp>

using namespace server;

using namespace server::net;

DiscardServer::DiscardServer(EventLoop *loop,
							const InetAddress &listenAddr)
	: server_(loop, listenAddr, "DiscardServer")
{
	server_.setConnectionCallback(boost::bind(&DiscardServer::onConnection, this, _1));
	server_.setMessageCallback(boost::bind(&DiscardServer::onMessage, this, _1, _2, _3));
}

void DiscardServer::start()
{
	server_.start();
}

void DiscardServer::onConnection(const TcpConnectionPtr &conn)
{
	LOG_INFO << "DiscardServer - " << conn->peerAddress().toIpPort() << " -> "
			 << conn->localAddress().toIpPort() << " is "
			 << (conn->connected() ? "UP" : "DOWN");
}

void DiscardServer::onMessage(const TcpConnectionPtr &conn,
							  Buffer *buf,
							  Timestamp time)
{
	//服务器收到的数据全部丢弃
	string msg(buf->retrieveAllAsString());
	LOG_INFO << conn->name() << " discards " << msg.size()
			 << "bytes received at " << time.toString();
}