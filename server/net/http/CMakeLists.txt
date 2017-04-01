set(http_SRCS
  HttpServer.cc
  HttpResponse.cc
  )

add_library(server_http ${http_SRCS})
target_link_libraries(server_http server_net)



add_executable(httpserver_test tests/HttpServer_test.cc)
target_link_libraries(httpserver_test server_http)