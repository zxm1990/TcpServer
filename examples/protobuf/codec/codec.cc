#include "codec.h"

#include <server/base/Logging.h>
#include <server/net/Endian.h>
#include <server/net/protobuf/google-inl.h>

#include <google/protobuf/descriptor.h>

#include <zlib.h> 

using namespace server;
using namespace server::net;

void ProtobufCodec::fillEmptyBuffer(Buffer *buf, const google::protobuf::Message& message)
{
	//buf应该是空的
	assert(buf->readableBytes() == 0);

	//先加入name
	const std::string& typeName = message.GetTypeName();
	int32_t nameLen = static_cast<int32_t>(typeName.size() + 1);
	buf->appendInt32(nameLen);
	buf->append(typeName.c_str(), nameLen);

	//先保证buf有足够的空间
	int byte_size = message.ByteSize();
	buf->ensureWritableBytes(byte_size);

	uint8_t* start = reinterpret_cast<uint8_t*>(buf->beginWrite());
	//序列化
	uint8_t* end = message.SerializeWithCachedSizesToArray(start);
	if (end - start != byte_size)
	{
		ByteSizeConsistencyError(byte_size, message.ByteSize(), static_cast<int>(end - start));
	}
	//移动write Index
	buf->hasWritten(byte_size);

	int32_t checkSum = static_cast<int32_t>(::adler32(1,
													  reinterpret_cast<const Bytef*>(buf->peek()),
													  static_cast<int>(buf->readableBytes())));

	//添加校验码
	buf->appendInt32(checkSum);
	assert(buf->readableBytes() == sizeof nameLen + nameLen + byte_size + sizeof checkSum);
	//在前面加入总长度
	int32_t len = sockets::hostToNetwork32(static_cast<int32_t>(buf->readableBytes()));
	buf->prepend(&len, sizeof len);

}

namespace
{
	const string kNoErrorStr = "NoError";
  	const string kInvalidLengthStr = "InvalidLength";
  	const string kCheckSumErrorStr = "CheckSumError";
  	const string kInvalidNameLenStr = "InvalidNameLen";
  	const string kUnknownMessageTypeStr = "UnknownMessageType";
  	const string kParseErrorStr = "ParseError";
  	const string kUnknownErrorStr = "UnknownError";
}

const string& ProtobufCodec::errorCodeToString(ErrorCode errorCode)
{
	switch (errorCode)
  	{
   		case kNoError:
     		return kNoErrorStr;

   		case kInvalidLength:
     		return kInvalidLengthStr;

   		case kCheckSumError:
     		return kCheckSumErrorStr;

   		case kInvalidNameLen:
     		return kInvalidNameLenStr;

   		case kUnknownMessageType:
     		return kUnknownMessageTypeStr;

   		case kParseError:
     		return kParseErrorStr;

   		default:
     		return kUnknownErrorStr;
  	}
}

void ProtobufCodec::defaultErrorCallback(const TcpConnectionPtr& conn, Buffer *buf, Timestamp, ErrorCode errorCode)
{
	LOG_ERROR << "ProtobufCodec::defaultErrorCallback - " << errorCodeToString(errorCode);
  	if (conn && conn->connected())
  	{
    	conn->shutdown();
  	}
}

int32_t asInt32(const char* buf)
{
	int32_t be32 = 0;
  	::memcpy(&be32, buf, sizeof(be32));
  	return sockets::networkToHost32(be32);
}

void ProtobufCodec::onMessage(const TcpConnectionPtr& conn, Buffer *buf, Timestamp receiveTime)
{
	//接收一个完整的数据才解析
	while (buf->readableBytes() >= kMinMessageLen + kHeaderLen)
	{
		//解析总数据长度
		//发送时总长度存在pre区域
		//接收时总长度存在read区域
		const int32_t len = buf->peekInt32();
		if (len > kMaxMessageLen || len < kMinMessageLen)
		{
			errorCallback_(conn, buf, receiveTime, kInvalidLength);
			break;
		}
		else if (buf->readableBytes() >= implicit_cast<size_t>(len + kHeaderLen))
		{
			//有完整的数据

			ErrorCode errorCode = kNoError;

			//抽象的message对象, 在buf中解析出
			MessagePtr message = parse(buf->peek() + kHeaderLen, len, errorCode);
			if (errorCode == kNoError && message)
			{
				messageCallback_(conn, message, receiveTime);
				buf->retrieve(kHeaderLen + len);
			}
			else
			{
				errorCallback_(conn, buf, receiveTime, errorCode);
        		break;
			}
		}
		else
		{
			break;
		}
	}
}

//抽象Message
MessagePtr ProtobufCodec::parse(const char *buf, int len, ErrorCode *error)
{
	MessagePtr message;

	//先检查校验和
	int32_t expectedCheckSum = asInt32(buf + len - kHeaderLen);
	int32_t checkSum = static_cast<int32_t>(::adler32(1,
                									  reinterpret_cast<const Bytef*>(buf),
                									  static_cast<int>(len - kHeaderLen)));
	if (checkSum == expectedCheckSum)
	{
		//获取message的name
		int32_t nameLen = asInt32(buf);
		if (nameLen >= 2 && nameLen <= len - 2*kHeaderLen)
		{
			std::string typeName(buf + kHeaderLen, buf + kHeaderLen + nameLen - 1);
			//创建message
			message.reset(createMessage(typeName));
			if (message)
			{
				//解析内容
				const char *data = buf + kHeaderLen + nameLen;
				int32_t dataLen = len - nameLen - 2*kHeaderLen;
				if (message->parseFromArray(data, dataLen))
				{
					*error = kNoError;
				}
				else
				{
					*error = kParseError;
				}
			}
			else //创建message 失败
			{
				*error = kUnknownMessageType;
			}

		}
		else //nameLen 失败
		{
			*error = kInvalidNameLen;
		}
	}
	else //校验和失败
	{
		*error = kCheckSumError;
	}

	return message;
}

//根据typeName创建具体的Message
google::protobuf::Message* ProtobufCodec::createMessage(const std::string& typeName)
{
	google::protobuf::Message *message = NULL;

	//先得到descriptor
	const google::protobuf::Descriptor *decriptor = 
		google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
	if (descriptor)
	{
		//得到Message
		const google::protobuf::Message *prototype = 
			google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype)
		{
			message = prototype->New();
		}
	}

	return message;
}












