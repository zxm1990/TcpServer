#add_executable(eventLoop_test EventLoop_test.cc)

#target_link_libraries(eventLoop_test server_net server_base)

add_executable(TimerQueue_test TimerQueue_test.cc)

target_link_libraries(TimerQueue_test server_net)