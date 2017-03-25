#ifndef SERVER_BASE_PROCESSINFO_H
#define SERVER_BASE_PROCESSINFO_H

#include <server/base/StringPiece.h>
#include <server/base/Types.h>
#include <server/base/Timestamp.h>
#include <vector>

namespace server
{

//进程基本信息
namespace ProcessInfo
{
  //进程id
    pid_t pid();
  
    string pidString();
    
    uid_t uid();
    
    string username();
  
    uid_t euid();
    
    Timestamp startTime();
    
    int clockTicksPerSecond();
    
    int pageSize();
  
    bool isDebugBuild();  // constexpr

    string hostname();
  
    string procname();
  
    StringPiece procname(const string& stat);

    /// read /proc/self/status
    string procStatus();

    /// read /proc/self/stat
    string procStat();

    /// read /proc/self/task/tid/stat
    string threadStat();

    /// readlink /proc/self/exe
    string exePath();

    int openedFiles();
    
    int maxOpenFiles();

    struct CpuTime
    {
      double userSeconds;
      double systemSeconds;

      CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }
    };
  
    CpuTime cpuTime();

    int numThreads();
    
    std::vector<pid_t> threads();
}

}

#endif  // SERVER_BASE_PROCESSINFO_H
