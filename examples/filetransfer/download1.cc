#include <server/base/Logging.h>
#include <server/net/EventLoop.h>
#include <server/net/TcpServer.h>

#include <stdio.h>

using namespace server;
using namespace server::net;

const char *g_file = NULL;

string readFile(const char *filename)
{
	string content;
	FILE *fp = ::fopen(filename, "rb");
	if (fp)
	{
		const int kBufSize = 1024*1024;
		char iobuf[kBufSize];
		//设置缓冲区
		::setbuffer(fp, iobuf, sizeof iobuf);

		char buf[kBufSize];
		size_t nread = 0;
		//一次读取所有的数据
		while ((nread = ::fread(buf, 1, sizeof buf, fp)) > 0)
		{
			content.append(buf, nread);
		}
		::fclose(fp);
	}

	return content;
}

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
        conn->setHighWaterMarkCallback(onHighWaterMark, 64*1024);
        string fileContent = readFile(g_file);
        conn->send(fileContent);
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
		tcpServer.start();
		loop.loop();
	}
	else
	{
		fprintf(stderr, "Usage: %s file_for_downloading\n", argv[0]);
	}
}