#ifndef SERVER_BASE_ATOMIC_H
#define SERVER_BASE_ATOMIC_H

#include <boost/noncopyable.hpp>
#include <stdint.h>

namespace server
{

namespace detail
{

template <typename T> class AtomicIntegerT : boost::noncopyable
{
public:
	AtomicIntegerT()
		: value_(0)
	{

	}

	T get()
	{
		//value, oldvalue, newvalue
		return __sync_val_compare_and_swap(&value_, 0, 0);
	}

	T getAndAdd(T x)
	{
		return __sync_fetch_and_add(&value_, x);
	}

	T addAndGet(T x)
	{
		return getAndAdd(x) + x;
	}

	T incrementAndGet()
	{
		return addAndGet(1);
	}

	T decrementAndGet()
	{
		return addAndGet(-1);
	}

	void add(T x)
	{
		getAndAdd(x);
	}

	void increment()
	{
		incrementAndGet();
	}

	void decrement()
	{
		decrementAndGet();
	}

	T getAndSet(T newValue)
	{
		return __sync_lock_test_and_set(&value_, newValue);
	}
	~AtomicIntegerT();
	
private:
	volatile T value_;
};
}

typedef detail::AtomicIntegerT<int32_t> AtomicInte32;
typedef detail::AtomicIntegerT<int64_t> AtomicInte64;
}


#endif //SERVER_BASE_ATOMIC_H