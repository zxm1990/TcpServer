#include "codec.h"

#include <server/base/Logging.h>
#include <server/base/Mutex.h>
#include <server/net/EventLoopThread.h>
#include <server/net/TcpClient.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include <iostream>
#include <stdio.h>

using namespace server;
using namespace server::net;

class ChatClient : boost::noncopyable
{
public:
	ChatClient(EventLoop *loop,
			   const InetAddress &serverAddr)
		: client_(loop, serverAddr, "ChatClient"),
		  codec_(boost::bind(&ChatClient::onStringMessage, this, _1, _2, _3))
	{
		client_.setConnectionCallback(boost::bind(&ChatClient::onConnection, this, _1));
		client_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
		//重试
		client_.enableRetry();
	}

	void connect()
	{
		client_.connect();
	}

	void disconnect()
	{
		client_.disconnect();
	}

	void write(const StringPiece &message)
	{
		//多线程竞争
		MutexLockGuard lock(mutex_);
		if (connection_)
		{
			codec_.send(get_pointer(connection_), message);
		}
	}

private:
	void onConnection(const TcpConnectionPtr& conn)
	{
		LOG_INFO << conn->localAddress().toIpPort() << " -> "
             	 << conn->peerAddress().toIpPort() << " is "
             	 << (conn->connected() ? "UP" : "DOWN");
        MutexLockGuard lock(mutex_);
        if (conn->connected())
        {
        	//记录每个连接对象
        	connection_ = conn;
        }
        else
        {
        	connection_.reset();
        }
	}

	//客户端收到消息，直接显示，这里不能用iostream,因为不是线程安全
	void onStringMessage(const TcpConnectionPtr&,
                         const string& message,
                         Timestamp)
  	{
    	printf("<<< %s\n", message.c_str());
  	}

  	TcpClient client_;
  	LengthHeaderCodec codec_;
  	MutexLock mutex_;
  	TcpConnectionPtr connection_;
};

int main(int argc, char* argv[])
{
	LOG_INFO << " pid = " << getpid();
	if (argc > 2)
	{
		//一个线程用于接收网络库消息
		EventLoopThread loogThread;
		uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    	InetAddress serverAddr(argv[1], port);

    	ChatClient client(loogThread.startLoop(), serverAddr);
    	client.connect();

    	//主线程接收键盘消息
    	std::string line;
    	while (std::getline(std::cin, line))
    	{
    		client.write(line);
    	}
    	client.disconnect();
    	CurrentThread::sleepUsec(1000*1000);
	}
	else
	{
		printf("Usage: %s host_ip port\n", argv[0]);
	}
}