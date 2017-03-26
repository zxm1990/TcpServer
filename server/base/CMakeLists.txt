set(base_SRCS
  AsyncLogging.cc
  Condition.cc
  CountDownLatch.cc
  Date.cc
  Exception.cc
  FileUtil.cc
  LogFile.cc
  Logging.cc
  LogStream.cc
  ProcessInfo.cc 
  Timestamp.cc
  TimeZone.cc
  Thread.cc
  ThreadPool.cc)

add_library(server_base ${base_SRCS})

add_subdirectory(test)