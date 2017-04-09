add_executable(simple_echo echo/echo.cc echo/main.cc)
target_link_libraries(simple_echo server_net)