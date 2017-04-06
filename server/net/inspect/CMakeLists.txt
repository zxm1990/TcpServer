set(inspect_SRCS
  Inspector.cc
  PerformanceInspector.cc
  ProcessInspector.cc
  SystemInspector.cc
  )

add_library(server_inspect ${inspect_SRCS})
target_link_libraries(server_inspect server_http)


if(NOT CMAKE_BUILD_NO_EXAMPLES)
add_executable(inspector_test tests/Inspector_test.cc)
target_link_libraries(inspector_test server_inspect)
endif()

