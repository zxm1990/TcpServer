#include <server/base/ProcessInfo.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

int main()
{
  printf("pid = %d\n", server::ProcessInfo::pid());
  printf("uid = %d\n", server::ProcessInfo::uid());
  printf("euid = %d\n", server::ProcessInfo::euid());
  printf("start time = %s\n", server::ProcessInfo::startTime().toFormattedString().c_str());
  printf("hostname = %s\n", server::ProcessInfo::hostname().c_str());
  printf("opened files = %d\n", server::ProcessInfo::openedFiles());
  printf("threads = %zd\n", server::ProcessInfo::threads().size());
  printf("num threads = %d\n", server::ProcessInfo::numThreads());
  printf("status = %s\n", server::ProcessInfo::procStatus().c_str());
}
