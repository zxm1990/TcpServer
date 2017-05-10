#ifndef SERVER_NET_PROTORPC_PROCODEC_H
#define SERVER_NET_PROTORPC_PROCODEC_H

#include <server/base/Timestamp.h>
#include <server/net/protobuf/ProtobufCodecLite.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace server
{

namespace net
{

class Buffer;
class TcpConnection;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

class RpcMessage;
typedef boost::shared_ptr<RpcMessage> RpcMessagePtr;

extern const char rpctag[];

// rpc的具体格式
//
// Field     Length  Content
//
// size      4-byte  N+8
// "RPC0"    4-byte
// payload   N-byte
// checksum  4-byte  adler32 of "RPC0"+payload

typedef ProtobufCodecLiteT<RpcMessage, rpctag> RpcCodec;

}
}


#endif // SERVER_NET_PROTORPC_PROCODEC_H