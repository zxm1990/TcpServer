#include <server/base/Logging.h>
#include <server/net/EventLoop.h>
#include <server/net/TcpServer.h>

#include <boost/shared_ptr.hpp>
#include <stdio.h>

using namespace server;
using namespace server::net;

/*
 * 先发送64KB数据，等数据发送完毕之后，再发送64KB数据
*/
/*
 * 使用智能指针管理文件资源
*/

const int kBufSize = 64*1024;
const char *g_file = NULL;
typedef boost::shared_ptr<FILE> FilePtr;

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
            FilePtr ctx(fp, ::fclose);
        	//存储上下文
        	conn->setContext(ctx);
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
}

void onWriteComplete(const TcpConnectionPtr &conn)
{
	const FilePtr &fp = boost::any_cast<const FilePtr&>(conn->getContext());
	char buf[kBufSize];
	size_t nread = ::fread(buf, 1, sizeof buf, get_pointer(fp));
	if (nread > 0)
	{
		conn->send(buf, static_cast<int>(nread));
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