#pragma once
#include <server/net/Buffer.h>
#include <google/protobuf/io/zero_copy_stream.h>

namespace server
{
namespace net
{

//直接操作数据，不需要内存拷贝
class BufferOutputSteam : public google::protobuf::io::ZeroCopyOutputStream
{
public:
	BufferOutputSteam(Buffer *buf)
		: buffer_(CHECK_NOTNULL(buf)),
		  originalSize_(buffer_->readableBytes())
	{

	}

	//data 获取buf的地址
	//size 获取buf的长度
	virtual bool Next(void **data, int *size)
	{
		buffer_->ensureWritableBytes(4096);
		*data = buffer_->beginWrite();
		*size = static_cast<int>(buffer_->writableBytes());
		buffer_->hasWritten(*size);
		return true;
	}

	//释放没有使用完的内存
	virtual int64_t BackUp(int count)
	{
		buffer_->unwrite(count);
	}

	//返回写入的字节数
	virtual int64_t ByteCount() const
	{
		return buffer_->readableBytes() - originalSize_;
	}

private:
	Buffer *buffer_;
	size_t originalSize_;
};


}
}