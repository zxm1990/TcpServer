add_executable(simple_echo echo/echo.cc echo/main.cc)
target_link_libraries(simple_echo server_net)

add_executable(simple_discard discard/discard.cc discard/main.cc)
target_link_libraries(simple_discard server_net)

add_executable(simple_daytime daytime/daytime.cc daytime/main.cc)
target_link_libraries(simple_daytime server_net)

add_executable(simple_time time/time.cc time/main.cc)
target_link_libraries(simple_time server_net)

add_executable(simple_timeclient timeclient/timeclient.cc)
target_link_libraries(simple_timeclient server_net)

add_executable(simple_chargen chargen/chargen.cc chargen/main.cc)
target_link_libraries(simple_chargen server_net)

add_executable(simple_allinone 
	chargen/chargen.cc 
	time/time.cc
	chargen/chargen.cc
	echo/echo.cc
	discard/discard.cc
	daytime/daytime.cc
	allinone/allinone.cc)
target_link_libraries(simple_allinone server_net)