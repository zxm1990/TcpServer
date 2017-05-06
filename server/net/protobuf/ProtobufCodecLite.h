#ifndef SERVER_NET_PROTOBUF_CODEC_H
#define SERVER_NET_PROTOBUF_CODEC_H

#include <server/base/StringPiece.h>
#include <server/base/Timestamp.h>
#include <server/net/Callbacks.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/bind.hpp>

#ifndef NDEBUG
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>
#endif

namespace google
{
namespace protobuf
{

class Message;
}
}

namespace server
{

namespace net
{

class Buffer;
class TcpConnection;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef boost::shared_ptr<google::protobuf::Message> MessagePtr;

// wire format
//
// Field     Length  Content
//
// size      4-byte  M+N+4
// tag       M-byte  could be "RPC0", etc.
// payload   N-byte
// checksum  4-byte  adler32 of tag+payload
//

class ProtobufCodecLite : boost::noncopyable
{
public:
	const static int kHeaderLen = sizeof(int32_t);
	const static int kChecksumLen = sizeof(int32_t);
	const static int kMaxMessageLen = 64*1024*1024;

	enum ErrorCode
	{
		kNoError = 0,
		kInvalidLength,
		kCheckSumError,
		kInvalidNameLen,
		kUnknownMessageType,
		kParseError,
	};

	typedef boost::function<bool (const TcpConnectionPtr&,
								  StringPiece,
								  Timestamp)> RawMessageCallback;

	typedef boost::function<bool (const TcpConnectionPtr&,
								  const MessagePtr,
								  Timestamp)> ProtobufMessageCallback;

	typedef boost::function<bool (const TcpConnectionPtr&,
								  Buffer*,
								  Timestamp,
								  ErrorCode)> ErrorCallback;

	ProtobufCodecLite(const ::google::protobuf::Message *prototype,
					  StringPiece tagArg,
					  const ProtobufMessageCallback &messageCb,
					  const RawMessageCallback &rawCb = RawMessageCallback(),
					  const ErrorCallback &errorCb = defaultErrorCallback)
		: prototype_(prototype),
		  tag_(tagArg.as_string()),
		  messageCallback_(messageCb),
		  rawCb_(rawCb),
		  errorCallback_(errorCb),
		  kMinMessageLen(tagArg.size() + kChecksumLen)
	{

	}

	virtual ~ProtobufCodecLite()
	{

	}

	const string& tag() const
	{
		return tag_;
	}

	void send(const TcpConnectionPtr &conn,
			  const ::google::protobuf::Message &message);

	void onMessage(const TcpConnectionPtr &conn,
				   Buffer *buf,
				   Timestamp receiveTime);

	virtual bool parseFromBuffer(StringPiece buf, google::protobuf::Message* message);

	virtual int serializeToBuffer(const google::protobuf::Message &message, Buffer *buff);

	static const string& errorCodeToString(ErrorCode errorCode);

	ErrorCode parse(const char *buf, int len, ::google::protobuf::Message *message);

	void fillEmptyBuffer(server::net::Buffer *buf, const google::protobuf::Message &message);

	static int32_t checksum(const void *buf, int len);
	static bool validateChecksum(const char *buf, int len);
	static int32_t asInt32(const char *buf);
	static void defaultErrorCallback(const TcpConnectionPtr&,
									 Buffer*,
									 Timestamp,
									 ErrorCode);
private:
	const ::google::protobuf::Message *prototype_;
	const string tag_;
	ProtobufMessageCallback messageCallback_;
	RawMessageCallback rawCb_;
	ErrorCallback errorCallback_;
	const int kMinMessageLen;

};

template<typename MSG, const char* TAG, typename CODEC=ProtobufCodecLite>  
class ProtobufCodecLiteT
{
#ifndef NDEBUG
  	BOOST_STATIC_ASSERT((boost::is_base_of<ProtobufCodecLite, CODEC>::value));
#endif
 public:
  	typedef boost::shared_ptr<MSG> ConcreteMessagePtr;
  	typedef boost::function<void (const TcpConnectionPtr&,
    	                          const ConcreteMessagePtr&,
                                  Timestamp)> ProtobufMessageCallback;
  	typedef ProtobufCodecLite::RawMessageCallback RawMessageCallback;
  	typedef ProtobufCodecLite::ErrorCallback ErrorCallback;

  	explicit ProtobufCodecLiteT(const ProtobufMessageCallback& messageCb,
    	                        const RawMessageCallback& rawCb = RawMessageCallback(),
                                const ErrorCallback& errorCb = ProtobufCodecLite::defaultErrorCallback)
    	: messageCallback_(messageCb),
      	  codec_(&MSG::default_instance(),
             	 TAG,
             	 boost::bind(&ProtobufCodecLiteT::onRpcMessage, this, _1, _2, _3),
             	 rawCb,
             	 errorCb)
  	{

  	}

	const string& tag() const 
	{ 
  		return codec_.tag(); 
	}

  	void send(const TcpConnectionPtr& conn, const MSG& message)
  	{
    	codec_.send(conn, message);
  	}

  	void onMessage(const TcpConnectionPtr& conn,
                   Buffer* buf,
                   Timestamp receiveTime)
  	{
     	codec_.onMessage(conn, buf, receiveTime);
  	}

  
  	void onRpcMessage(const TcpConnectionPtr& conn,
                      const MessagePtr& message,
                      Timestamp receiveTime)
  	{
    	messageCallback_(conn, ::server::down_pointer_cast<MSG>(message), receiveTime);
  	}

  	void fillEmptyBuffer(server::net::Buffer* buf, const MSG& message)
  	{
    	codec_.fillEmptyBuffer(buf, message);
  	}

private:
	ProtobufMessageCallback messageCallback_;
  	CODEC codec_;
};

} //net

} //server

#endif //SERVER_NET_PROTOBUF_CODEC_H