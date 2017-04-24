#include "daytime.h"

#include <server/base/Logging.h>
#include <server/base/EventLoop.h>

#include <boost/bind.hpp>

using namespace server;
using namespace server::net;

DaytimeServer::DaytimeServer(EventLoop *loop,
							const InetAddress &listenAddr)
	: server_(loop, listenAddr, "DaytimeServer")
{
	server_.setConnectionCallback(boost::bind(&DaytimeServer::onConnection, this, _1));
	server_.setMessageCallback(boost::bind(&DaytimeServer::onMessage, this, _1, _2, _3));
}

void DaytimeServer::start()
{
	server_.start();
}

void DaytimeServer::onConnection()