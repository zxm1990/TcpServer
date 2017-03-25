typedef boost::ptr_vector<LargeBuffer> BufferVector;
typedef BufferVector::auto_type        BufferPtr;
MutexLock      mutex_;
Condition      cond_;
BufferPtr      currentBuffer_;  //当前缓冲区
BufferPtr      nextBuffer_;     //预备缓冲区
BufferVector   buffers_;        //待写入文件已填满的缓冲

/*
typedef BufferVector::auto_type 类似C++11的std::unique_ptr 具备移动语义，
而且能自动管理生命周期
*/

//前端写入日志
void AsyncLogging::append(const char* logline, int len)
{
	MutexLockGuard lock(mutex_);

	//如果当前剩余空间足够大
	if (currentBuffer_->avail() > len)
	{
		currentBuffer_.append(logline, len);
	}
	else 
	{
		//buffer 已经满了，添加到vector中
		buffers_.push_back(currentBuffer_.release());

		if (nextBuffer_)
		{
			//预备缓冲区可用
			//移动而不是复制
			currentBuffer_ = boost::ptr_container::move(nextBuffer_);
		}
		else
		{
			//预备缓冲区也满了
			currentBuffer_.reset(new LargeBuffer);
		}

		currentBuffer_.append(logline, len);

		//当前缓冲区满，通知后端
		cond_.notify();
	}
}

//后端写入文件
void AsyncLogging::threadFunc()
{
	BufferPtr newBuffer1(new LargeBuffer);
	BufferPtr newBuffer2(new LargeBuffer);
	BufferVector bufferToWrite;

	while (running_)
	{
		//交换要写入的缓冲区，保持临界区最短
		{
			MutexLockGuard lock(mutex_);
			if (buffers_.empty())
			{
				cond_.waitForSeconds(flushInterval_);
			}
			//将当前的缓冲区压入
			buffers_.push_back(currentBuffer_.release());
			//当前缓冲区指向空闲
			currentBuffer_ = boost::ptr_container::move(newBuffer1);
			//交换
			bufferToWrite.swap(buffers_);

			if (!nextBuffer_)
			{
				nextBuffer_ = boost::ptr_container::move(newBuffer2);
			}
		}

		//将 bufferToWrite写入文件
		//重新填充newBuffer1 and newBuffer2
	}

	//刷新flush
}