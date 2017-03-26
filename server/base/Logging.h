#ifndef SERVER_BASE_LOGGING_H
#define SERVER_BASE_LOGGING_H

#include <server/base/LogStream.h>
#include <server/base/Timestamp.h>

namespace server
{

class TimeZone;

//日志类
class Logger
{
 public:
  enum LogLevel
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

  // compile time calculation of basename of source file
  class SourceFile
  {
   public:
    template<int N>
    inline SourceFile(const char (&arr)[N])
      : data_(arr),
        size_(N-1)
    {
      //查找/在字符串最后一次出现的位置
      const char* slash = strrchr(data_, '/'); // builtin function
      if (slash)
      {
        data_ = slash + 1;
        size_ -= static_cast<int>(data_ - arr);
      }
    }

    explicit SourceFile(const char* filename)
      : data_(filename)
    {
      const char* slash = strrchr(filename, '/');
      if (slash)
      {
        data_ = slash + 1;
      }
      size_ = static_cast<int>(strlen(data_));
    }

    //data指向最后一个'/'之后的名字
    const char* data_;
    int size_;
  };

  Logger(SourceFile file, int line);
  Logger(SourceFile file, int line, LogLevel level);
  Logger(SourceFile file, int line, LogLevel level, const char* func);
  Logger(SourceFile file, int line, bool toAbort);
  ~Logger();

  LogStream& stream() { return impl_.stream_; }

  static LogLevel logLevel();
  static void setLogLevel(LogLevel level);

  typedef void (*OutputFunc)(const char* msg, int len);
  typedef void (*FlushFunc)();
  static void setOutput(OutputFunc);
  static void setFlush(FlushFunc);
  static void setTimeZone(const TimeZone& tz);

 private:

class Impl
{
 public:
  typedef Logger::LogLevel LogLevel;
  Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
  void formatTime();
  void finish();

  Timestamp time_;
  LogStream stream_;
  LogLevel level_;
  int line_;
  SourceFile basename_;
};

  Impl impl_;

};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
  return g_logLevel;
}

//将数据记录到buffer中
#define LOG_TRACE if (server::Logger::logLevel() <= server::Logger::TRACE) \
  server::Logger(__FILE__, __LINE__, server::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (server::Logger::logLevel() <= server::Logger::DEBUG) \
  server::Logger(__FILE__, __LINE__, server::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (server::Logger::logLevel() <= server::Logger::INFO) \
  server::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN server::Logger(__FILE__, __LINE__, server::Logger::WARN).stream()
#define LOG_ERROR server::Logger(__FILE__, __LINE__, server::Logger::ERROR).stream()
#define LOG_FATAL server::Logger(__FILE__, __LINE__, server::Logger::FATAL).stream()
#define LOG_SYSERR server::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL server::Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);


#define CHECK_NOTNULL(val) \
  ::server::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))


template <typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr)
{
  if (ptr == NULL)
  {
   Logger(file, line, Logger::FATAL).stream() << names;
  }
  return ptr;
}

}

#endif  // SERVER_BASE_LOGGING_H
