#添加非标准库的搜索路径 对应protobuf
link_directories(/usr/local/lib)

add_library(protobuf_codec codec.cc)
target_link_libraries(protobuf_codec protobuf server_net z)

add_custom_command(OUTPUT query.pb.cc query.pb.h
	COMMAND protoc
 	ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/query.proto -I${CMAKE_CURRENT_SOURCE_DIR}
  	DEPENDS query.proto
  	VERBATIM )

 set_source_files_properties(query.pb.cc PROPERTIES COMPILE_FLAGS "-Wno-conversion -Wno-shadow")
include_directories(${PROJECT_BINARY_DIR})

add_library(query_proto query.pb.cc)
target_link_libraries(query_proto protobuf pthread)

add_executable(protobuf_codec_test codec_test.cc codec.cc query.pb.cc)
set_target_properties(protobuf_codec_test PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(protobuf_codec_test protobuf_codec query_proto)

