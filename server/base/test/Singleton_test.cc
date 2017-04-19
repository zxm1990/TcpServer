#include <server/base/Singleton.h>
#include <server/base/CurrentThread.h>
#include <server/base/Thread.h>

#include <boost/noncopyable.hpp>
#include <stdio.h>

class Test : boost::noncopyable
{
 public:
  Test()
  {
    printf("tid=%d, constructing %p\n", server::CurrentThread::tid(), this);
  }

  ~Test()
  {
    printf("tid=%d, destructing %p %s\n", server::CurrentThread::tid(), this, name_.c_str());
  }

  const server::string& name() const { return name_; }
  void setName(const server::string& n) { name_ = n; }

 private:
  server::string name_;
};

class TestNoDestroy : boost::noncopyable
{
 public:
  // Tag member for Singleton<T>
  void no_destroy();

  TestNoDestroy()
  {
    printf("tid=%d, constructing TestNoDestroy %p\n", server::CurrentThread::tid(), this);
  }

  ~TestNoDestroy()
  {
    printf("tid=%d, destructing TestNoDestroy %p\n", server::CurrentThread::tid(), this);
  }
};

void threadFunc()
{
  //子线程，共享一个实例
  printf("tid=%d, %p name=%s\n",
         server::CurrentThread::tid(),
         &server::Singleton<Test>::instance(),
         server::Singleton<Test>::instance().name().c_str());
  server::Singleton<Test>::instance().setName("only one, changed");
}

//多个线程依然只有一个实例
int main()
{
  server::Singleton<Test>::instance().setName("only one");
  server::Thread t1(threadFunc);
  t1.start();
  t1.join();
  printf("tid=%d, %p name=%s\n",
         server::CurrentThread::tid(),
         &server::Singleton<Test>::instance(),
         server::Singleton<Test>::instance().name().c_str());

  //TestNoDestroy 没有成员，不会释放资源，资源泄露1个字节
  server::Singleton<TestNoDestroy>::instance();
  printf("with valgrind, you should see %zd-byte memory leak.\n", sizeof(TestNoDestroy));
}
