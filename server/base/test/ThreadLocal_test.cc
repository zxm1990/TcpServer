#include <server/base/ThreadLocal.h>
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

//全局变量
//证明全局变量在各个线程中具备不同的数据
server::ThreadLocal<Test> testObj1;
server::ThreadLocal<Test> testObj2;

void print()
{
  printf("-----print start-----\n");
  printf("tid=%d, obj1 %p name=%s\n",
         server::CurrentThread::tid(),
         &testObj1.value(),
         testObj1.value().name().c_str());
  printf("tid=%d, obj2 %p name=%s\n",
         server::CurrentThread::tid(),
         &testObj2.value(),
         testObj2.value().name().c_str());
  
  printf("-----print end-----\n");
}

void threadFunc()
{
  print();
  testObj1.value().setName("changed 1");
  testObj2.value().setName("changed 42");
  print();
}

int main()
{
  testObj1.value().setName("main one");
  print();
  server::Thread t1(threadFunc);
  t1.start();
  t1.join();
  testObj2.value().setName("main two");
  print();

  pthread_exit(0);
}
