#ifndef SERVER_EXAMPLES_ASIO_CHAT_CODEC_H
#define SERVER_EXAMPLES_ASIO_CHAT_CODEC_H

#include <server/base/Logging.h>
#include <server/net/Buffer.h>
#include <server/net/Endian.h>
#include <server/net/TcpConnection.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

class LengthHeaderCodec : boost::noncopyable
{
public:
	typedef boost::function<void(const server::net::TcpConnectionPtr&,
								 const server::string &message,
								 server::Timestamp)> StringMessageCallback;
	explicit LengthHeaderCodec(const StringMessageCallback &cb)
		: messageCallback_(cb)
	{

	}

	void onMessage(const server::net::TcpConnectionPtr &conn,
				   server::net::Buffer *buf,
				   server::Timestamp receiveTime)
	{
		//反复的读取数据
		while (buf->readableBytes() >= kHeaderLen)
		{
			//定位到只读区域的头部
			const void *data = buf->peek();
			//消息格式的前面4个字节代表消息的长度
			int32_t be32 = *static_cast<int32_t*>(data);
			const int32_t len = server::net::sockets::networkToHost32(be32);
			if (len > 65536 || len < 0)
			{
				LOG_ERROR << " Invalid length " << len;
				conn->shutdown();
				break;
			}
			else if (buf->readableBytes() >= len + kHeaderLen)
			{
				//将只读区域缩小，指针向后移动,跳过代表长度的字节
				buf->retrieve(kHeaderLen);
				server::string message(buf->peek(), len);
				messageCallback_(conn, message, receiveTime);
				//跳过读取的数据
				buf->retrieve(len);
			}
			else //读取的数据，不够一条完整的消息
			{
				break;
			}
		}
	}

	void send(server::net::TcpConnectionPtr *conn,
			 const server::StringPiece &message)
	{
		server::net::Buffer buf;
		//加入到只读区域
		buf.append(message.data(), message.size());
		int32_t len = static_cast<int32_t>(message.size());
		int32_t be32 = server::net::sockets::hostToNetwork32(len);
		//加入到只读区域的头部，只读区域向前扩大
		buf.prepend(&be32, sizeof(be32));
		conn->send(&buf);
	}

private:
	StringMessageCallback messageCallback_;
	const static size_t kHeaderLen = sizeof(int32_t);
};


#endif //SERVER_EXAMPLES_ASIO_CHAT_CODEC_H