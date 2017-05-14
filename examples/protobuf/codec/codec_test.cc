#include "codec.h"

#include <server/net/Endian.h>
#include <examples/protobuf/codec/query.pb.h>

#include <stdio.h>
#include <zlib.h>

using namespace server;
using namespace server::net;

void print(const Buffer &buf)
{
	printf("encode to %zd bytes\n", buf.readableBytes());
	for (size_t i = 0; i < buf.readableBytes(); ++i)
	{
		unsigned char ch = static_cast<unsigned char>(buf.peek()[i]);
		printf("%2zd: 0x%02x %c\n", i, ch, isgraph(ch) ? ch : ' ');
	}
}

void testQuery()
{
	server::Query query;
  	query.set_id(1);
  	query.set_questioner("Tcp Server Protobuf");
  	query.add_question("Running?");

  	Buffer buf;
  	ProtobufCodec::fillEmptyBuffer(&buf, query);
  	print(buf);

  	const int32_t len = buf.readInt32();
  	assert(len == static_cast<int32_t>(buf.readableBytes()));

  	ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
  	MessagePtr message = ProtobufCodec::parse(buf.peek(), len, &errorCode);
  	assert(errorCode == ProtobufCodec::kNoError);
  	assert(message != NULL);
  	message->PrintDebugString();
  	assert(message->DebugString() == query.DebugString());

  	boost::shared_ptr<server::Query> newQuery = down_pointer_cast<server::Query>(message);
  	assert(newQuery != NULL);
}


int main()
{
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  testQuery();
  puts("");

  google::protobuf::ShutdownProtobufLibrary();
}