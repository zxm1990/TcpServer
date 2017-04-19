#include <server/base/AsyncLogging.h>
#include <server/base/Logging.h>
#include <server/base/Timestamp.h>

#include <stdio.h>
#include <sys/resource.h>
#ifdef __MACH__
#include <libgen.h>  // basename()
#endif

int kRollSize = 500*1000*1000;

server::AsyncLogging* g_asyncLog = NULL;

void asyncOutput(const char* msg, int len)
{
  g_asyncLog->append(msg, len);
}

void bench(bool longLog)
{
  server::Logger::setOutput(asyncOutput);

  int cnt = 0;
  const int kBatch = 1000;
  server::string empty = " ";
  server::string longStr(3000, 'X');
  longStr += " ";

  for (int t = 0; t < 30; ++t)
  {
    server::Timestamp start = server::Timestamp::now();
    for (int i = 0; i < kBatch; ++i)
    {
      LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
               << (longLog ? longStr : empty)
               << cnt;
      ++cnt;
    }
    server::Timestamp end = server::Timestamp::now();
    printf("%f\n", timeDifference(end, start)*1000000/kBatch);
    struct timespec ts = { 0, 500*1000*1000 };
    nanosleep(&ts, NULL);
  }
}

int main(int argc, char* argv[])
{
  {
    size_t kOneGB = 1000*1024*1024;
    rlimit rl = { 2*kOneGB, 2*kOneGB };
    setrlimit(RLIMIT_AS, &rl);
  }

  printf("pid = %d\n", getpid());

  char name[256];
  strncpy(name, argv[0], 256);
  server::AsyncLogging log(::basename(name), kRollSize);
  log.start();
  g_asyncLog = &log;

  bool longLog = argc > 1;
  bench(longLog);
}
