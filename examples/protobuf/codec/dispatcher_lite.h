#ifndef SERVER_EXAMPLES_PROTOBUF_CODEC_DISPATCHER_LITE_H
#define SERVER_EXAMPLES_PROTOBUF_CODEC_DISPATCHER_LITE_H

#include <server/net/Callbacks.h>

#include <google/protobuf/message.h>

#include <map>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<google::protobuf::Message> MessagePtr;

class ProtobufDispatcherLite : boost::noncopyable
{
public:
    typedef boost::function<void (const server::net::TcpConnectionPtr&,
                                  const MessagePtr&,
                                  server::Timestamp)> ProtobufMessageCallback;

    explicit ProtobufDispatcherLite(const ProtobufMessageCallback& defaultCb)
                : defaultCallback_(defaultCb)
    {

    }

    void onProtobufMessage(const server::net::TcpConnectionPtr& conn,
                           const MessagePtr& message,
                           server::Timestamp receiveTime) const
    {
        CallbackMap::const_iterator it = callbacks_.find(message->GetDescriptor());
        if (it != callbacks_.end())
        {
            it->second(conn, message, receiveTime);
        }
        else
        {
            defaultCallback_(conn, message, receiveTime);
        }
    }

    //将descriptor和messagecall关联起来
    void registerMessageCallback(const google::protobuf::Descriptor* desc,
                                 const ProtobufMessageCallback& callback)
    {
        callbacks_[desc] = callback;
    }

private:

    typedef std::map<const google::protobuf::Descriptor*, ProtobufMessageCallback> CallbackMap;
    CallbackMap callbacks_;
    ProtobufMessageCallback defaultCallback_;
};

#endif  // SERVER_EXAMPLES_PROTOBUF_CODEC_DISPATCHER_LITE_H

