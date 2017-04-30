#include <server/base/Logging.h>
#include <server/net/EventLoop.h>
#include <server/net/TcpServer.h>

#include <stdio.h>

using namespace server;
using namespace server::net;

/*
 * 先发送64KB数据，等数据发送完毕之后，再发送64KB数据
*/


const int kBufSize = 64*1024;
const char *g_file = NULL;

void onHighWaterMark(const TcpConnectionPtr &conn, size_t len)
{
	LOG_INFO << "HighWaterMark " << len;
}

void onConnection(const TcpConnectionPtr &conn)
{
	LOG_INFO << "FileServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
    if (conn->connected())
    {
    	LOG_INFO << "FileServer - Sending file " << g_file
                 << " to " << conn->peerAddress().toIpPort();
        //
        conn->setHighWaterMarkCallback(onHighWaterMark, kBufSize + 1);
        
        FILE *fp = ::fopen(g_file, "rb");
        if (fp)
        {
        	//存储上下文
        	conn->setContext(fp);
        	char buf[kBufSize];
        	size_t nread = ::fread(buf, 1, sizeof buf, fp);
        	conn->send(buf, static_cast<int>(nread));
        }
        else
        {
        	conn->shutdown();
        	LOG_INFO << "FileServer - no such file";
        }
    }
    else
    {
    	if (!conn->getContext().empty())
    	{
    		FILE* fp = boost::any_cast<FILE*>(conn->getContext());
      		if (fp)
      		{
        		::fclose(fp);
      		}
    	}
    }
}

void onWriteComplete(const TcpConnectionPtr &conn)
{
	FILE *fp = boost::any_cast<FILE*>(conn->getContext());
	char buf[kBufSize];
	size_t nread = ::fread(buf, 1, sizeof buf, fp);
	if (nread > 0)
	{
		conn->send(buf, static_cast<int>(nread));
	}
	else
	{
		::fclose(fp);
		fp = NULL;
		conn->setContext(fp);
		conn->shutdown();
		LOG_INFO << "FileServer - done";
	}
}

int main(int argc, char *argv[])
{
	LOG_INFO << "pid = " << getpid();
	if (argc > 1)
	{
		g_file = argv[1];

		EventLoop loop;
		InetAddress listenAddr(2021);
		TcpServer tcpServer(&loop, listenAddr, "FileServer");
		tcpServer.setConnectionCallback(onConnection);
		tcpServer.setWriteCompleteCallback(onWriteComplete);
		tcpServer.start();
		loop.loop();
	}
	else
	{
		fprintf(stderr, "Usage: %s file_for_downloading\n", argv[0]);
	}
}