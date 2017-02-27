set(base_SRCS 
  UtcTime.cc
  Thread.cc)

add_library(server_base ${base_SRCS})

add_subdirectory(test)