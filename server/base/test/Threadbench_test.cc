#include <server/base/CurrentThread.h>
#include <server/base/Mutex.h>
#include <server/base/Thread.h>
#include <server/base/Timestamp.h>

#include <map>
#include <string>
#include <boost/bind.hpp>
#include <stdio.h>
#include <sys/wait.h>

server::MutexLock g_mutex;

//按照线程创建的延长时间进行排序
std::map<int, int> g_delays;

void threadFunc()
{
  //printf("tid=%d\n", server::CurrentThread::tid());
}

void threadFunc2(server::Timestamp start)
{
  server::Timestamp now(server::Timestamp::now());
  int delay = static_cast<int>(timeDifference(now, start) * 1000000);
  server::MutexLockGuard lock(g_mutex);
  ++g_delays[delay];
}

void forkBench()
{
  sleep(10);
  server::Timestamp start(server::Timestamp::now());
  int kProcesses = 10*1000;

  //创建10万个进程
  for (int i = 0; i < kProcesses; ++i)
  {
    pid_t child = fork();
    if (child == 0)
    {
      exit(0);
    }
    else
    {
      waitpid(child, NULL, 0);
    }
  }

  double timeUsed = timeDifference(server::Timestamp::now(), start);
  printf("process creation time used %f us\n", timeUsed*1000000/kProcesses);
  printf("number of created processes %d\n", kProcesses);
}

int main(int argc, char* argv[])
{
  printf("pid=%d, tid=%d\n", ::getpid(), server::CurrentThread::tid());
  server::Timestamp start(server::Timestamp::now());

  //创建10万个线程
  int kThreads = 100*1000;
  for (int i = 0; i < kThreads; ++i)
  {
    server::Thread t1(threadFunc);
    t1.start();
    t1.join();
  }

  //创建10个线程所用时间
  double timeUsed = timeDifference(server::Timestamp::now(), start);
  printf("thread creation time %f us\n", timeUsed*1000000/kThreads);
  printf("number of created threads %d\n", server::Thread::numCreated());

  for (int i = 0; i < kThreads; ++i)
  {
    server::Timestamp now(server::Timestamp::now());
    server::Thread t2(boost::bind(threadFunc2, now));
    t2.start();
    t2.join();
  }
  {
    server::MutexLockGuard lock(g_mutex);
    for (std::map<int, int>::iterator it = g_delays.begin();
        it != g_delays.end(); ++it)
    {
      printf("delay = %d, count = %d\n",
             it->first, it->second);
    }
  }

  forkBench();
}
