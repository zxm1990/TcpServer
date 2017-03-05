set(net_SRCS
	Channel.cc
	DefaultPoller.cc
	EPollPoller.cc
	EventLoop.cc
	Poller.cc
	PollPoller.cc
	Socket.cc
	Timer.cc
	TimerQueue.cc)

add_library(server_net ${net_SRCS})

add_subdirectory(tests)