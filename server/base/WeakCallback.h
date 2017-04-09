#ifndef SERVER_BASE_WEAKCALLBACK_H
#define SERVER_BASE_WEAKCALLBACK_H

#include <functional>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace server
{

// A barely usable WeakCallback

//回调函数管理
//当回调函数参数对象还活着的时候，才能调用这个回调函数
template<typename CLASS>
class WeakCallback
{
public:

    WeakCallback(const boost::weak_ptr<CLASS>& object,
                 const boost::function<void (CLASS*)>& function)
        : object_(object), function_(function)
    {
    
    }

  // Default dtor, copy ctor and assignment are okay

    //智能指针作为回调函数的参数
    void operator()() const
    {
        boost::shared_ptr<CLASS> ptr(object_.lock());
        if (ptr)
        {
            function_(ptr.get());
        }
    }

private:

    //不增加引用计数，只能表明对象是否活着
    boost::weak_ptr<CLASS> object_;
    boost::function<void (CLASS*)> function_;
};

template<typename CLASS>
WeakCallback<CLASS> makeWeakCallback(const boost::shared_ptr<CLASS>& object,
                                     void (CLASS::*function)())
{
    return WeakCallback<CLASS>(object, function);
}

template<typename CLASS>
WeakCallback<CLASS> makeWeakCallback(const boost::shared_ptr<CLASS>& object,
                                     void (CLASS::*function)() const)
{
    return WeakCallback<CLASS>(object, function);
}

}

#endif //SERVER_BASE_WEAKCALLBACK_H
