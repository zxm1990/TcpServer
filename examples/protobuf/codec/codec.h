#ifndef SERVER_EXAMPLES_PROTOBUF_CODEC_CODEC_H
#define SERVER_EXAMPLES_PROTOBUF_CODEC_CODEC_H

#include <server/net/Buffer.h>
#include <server/net/TcpConnection.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <google/protobuf/message.h>

typedef boost::shared_ptr<google::protobuf::Message> MessagePtr;

// wire format
//
// Field          Length           Content
//
// len            4-byte           M+N+4
// nameLen        4-byte          
// typename       nameLen-byte
// protobufData   len-nameLen-8
// checksum       4-byte           adler32 of tag+payload
//


class ProtobufCodec : boost::noncopyable
{

public:

	enum ErrorCode
	{
		kNoError = 0;
		kInvalidLength,
		kCheckSumError,
		kInvalidNameLen,
		kUnknownMessageType,
		kParseError,
	};

	typedef boost::function<void (const server::net::TcpConnectionPtr&,
								  const MessagePtr&,
								  server::Timestamp)> ProtobufMessageCallback;

	typedef boost::function<void (const server::net::TcpConnectionPtr&,
								  server::net::Buffer*,
								  server::Timestamp,
								  ErrorCode)> ErrorCallback;

	explicit ProtobufCodec(const ProtobufMessageCallback& messageCb, const ErrorCallback &errorCb)
				: messageCallback_(messageCb),
				  errorCallback_(errorCb)
	{

	}

	void onMessage(const server::net::TcpConnectionPtr& conn,
				   server::net::Buffer *buf,
				   server::Timestamp receiveTime);

	//将message转换为buf，在发送
	void send(const server::net::TcpConnectionPtr& conn,
			  const google::protobuf::Message& message)
	{
		server::net::Buffer buf;
		fillEmptyBuffer(&buf, message);
		conn->send(&buf);
	}

	static const server::string& errorCodeToString(ErrorCode errorCode);
	static void fillEmptyBuffer(server::net::Buffer *buf, const google::protobuf::Message& message);
	static google::protobuf::Message* createMessage(const std::string& type_name);
	static MessagePtr parse(const char* buf, int len, ErrorCode *errorCode);

private:
	static void defaultErrorCallback(const server::net::TcpConnectionPtr&,
									 server::net::Buffer*,
									 server::Timestamp,
									 ErrorCode);

	ProtobufMessageCallback messageCallback_;
	ErrorCallback errorCallback_;

	const static int kHeaderLen = sizeof(int32_t);
	const static int kMinMessageLen = 2*kHeaderLen + 2; // nameLen + typeName + checkSum
	const static int kMaxMessageLen = 64*1024*1024; // same as codec_stream.h kDefaultTotalBytesLimit
};


#endif // SERVER_EXAMPLES_PROTOBUF_CODEC_CODEC_H