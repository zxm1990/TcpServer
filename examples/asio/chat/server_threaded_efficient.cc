#include "codec.h"

#include <server/base/Logging.h>
#include <server/base/Mutex.h>
#include <server/net/EventLoop.h>
#include <server/net/TcpServer.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <set>
#include <stdio.h>

using namespace server;
using namespace server::net;

class ChatServer : boost::noncopyable
{
public:
	ChatServer(EventLoop *loop,
			   const InetAddress &listenAddr)
		: server_(loop, listenAddr, "ChatServer"),
		  codec_(boost::bind(&ChatServer::onStringMessage, this, _1, _2, _3)),
		  connections_(new ConnectionList)
	{
		server_.setConnectionCallback(boost::bind(&ChatServer::onConnection, this, _1));
		server_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
	}

	void setThreadNum(int numThreads)
	{
		server_.setThreadNum(numThreads);
	}

	void start()
	{
		server_.start();
	}

private:
	//使用copy-on-write技术实现读写锁功能
	void onConnection(const TcpConnectionPtr &conn)
	{
		LOG_INFO << conn->localAddress().toIpPort() << " -> "
        		 << conn->peerAddress().toIpPort() << " is "
        		 << (conn->connected() ? "UP" : "DOWN");

       	//注意锁保护的临界区
      	MutexLockGuard lock(mutex_);
      	//如果有读者，先把数据拷贝出来
      	if (!connections_.unique())
      	{
      		connections_.reset(new ConnectionList(*connections_));
      	}
      	assert(connections_.unique());

      	if (conn->connected())
      	{
      		connections_->insert(conn);
      	}
      	else
      	{
      		connections_->erase(conn);
      	}
	}

	//下面的定义一个变量要使用
	typedef std::set<TcpConnectionPtr> ConnectionList;
	typedef boost::shared_ptr<ConnectionList> ConnectionListPtr;

	//临界区只在获取数据的那一段
	void onStringMessage(const TcpConnectionPtr &,
						 const string &message,
						 Timestamp)
	{
		ConnectionListPtr connections = getConnectionList();
		for (ConnectionList::iterator it = connections_->begin();
			 it != connections_->end();
			 ++it)
		{
			codec_.send(get_pointer(*it), message);
		}
	}

	//相对于上一版本临界区变小了
	ConnectionListPtr getConnectionList()
  	{
    	MutexLockGuard lock(mutex_);
    	return connections_;
  	}


  	TcpServer server_;
  	LengthHeaderCodec codec_;
  	MutexLock mutex_;
  	ConnectionListPtr connections_;
};

int main(int argc, char *argv[])
{
	LOG_INFO << "pid = " << getpid();
	if (argc > 1)
	{
		EventLoop loop;
    	uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
    	InetAddress serverAddr(port);
    	ChatServer server(&loop, serverAddr);
    	if (argc > 2)
    	{
      		server.setThreadNum(atoi(argv[2]));
    	}
    	server.start();
    	loop.loop();
	}
	else
	{
		printf("Usage: %s port [thread_num]\n", argv[0]);
	}
}