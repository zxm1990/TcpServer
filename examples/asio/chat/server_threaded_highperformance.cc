#include "codec.h"

#include <server/base/Logging.h>
#include <server/base/Mutex.h>
#include <server/base/ThreadLocalSingleton.h>
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
		  codec_(boost::bind(&ChatServer::onStringMessage, this, _1, _2, _3))
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
 		//创建线程时，调用threadInit. 将loop插入set集合
 		server_.setThreadInitCallback(boost::bind(&ChatServer::threadInit, this, _1));
 		server_.start();
 	}

 private:
 	void onConnection(const TcpConnectionPtr& conn)
 	{
 		LOG_INFO << conn->localAddress().toIpPort() << " -> "
                 << conn->peerAddress().toIpPort() << " is "
             	 << (conn->connected() ? "UP" : "DOWN");

        //每个线程都有一个conn的set集合，所以这里不需要使用mutex
        //避免锁争用，所以提升效率
        if (conn->connected())
        {
        	LocalConnections::instance().insert(conn);
        }
        else
        {
        	LocalConnections::instance().erase(conn);
        }
 	}

 	void onStringMessage(const TcpConnectionPtr&,
 						 const string &message,
 						 Timestamp)
 	{
 		EventLoop::Functor f = boost::bind(&ChatServer::distributeMessage, this, message);
 		LOG_DEBUG << "onStringMessage begin";

 		//每个线程一个EventLoop，这个集合每个线程都会争用
 		MutexLockGuard lock(mutex_);
 		for (std::set<EventLoop*>::iterator it = loops_.begin();
 			 it != loops_.end(); ++it)
 		{
 			//非线程的任务放入队列中
 			(*it)->queueInLoop(f);
 		}

 		LOG_DEBUG << "onStringMessage end";
 	}

 	typedef std::set<TcpConnectionPtr> ConnectionList;

 	void distributeMessage(const string &message)
 	{
 		//肯定在自己线程内执行，不需要加锁
 		LOG_DEBUG << "begin";

 		for (ConnectionList::iterator it = LocalConnections::instance().begin();
 			 it != LocalConnections::instance().end();
 			 ++it)
 		{
 			codec_.send(get_pointer(*it), message);
 		}

 		LOG_DEBUG << "end";
 	}

 	void threadInit(EventLoop *loop)
 	{
 		assert(LocalConnections::pointer() == NULL);
 		LocalConnections::instance();
 		assert(LocalConnections::pointer() != NULL);

 		MutexLockGuard lock(mutex_);
 		loops_.insert(loop);
 	}

 	TcpServer server_;
 	LengthHeaderCodec codec_;
 	typedef ThreadLocalSingleton<ConnectionList> LocalConnections;

 	MutexLock mutex_;
 	std::set<EventLoop*> loops_;
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