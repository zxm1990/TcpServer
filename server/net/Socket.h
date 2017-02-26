#ifndef SERVER_NET_SOCKET_H
#define SERVER_NET_SOCKET_H

namespace server
{
// TCP networking

namespace net
{

//Wrapper of socket file descriptor

class Socket
{
public:
	explicit Socket(int sockfd)
		: sockfd_(sockfd)
	{

	}

	int fd() { return sockfd_; }

	//Enable/disable TCP_NODELAY (disable / enable Nagle's algorithm)
	void setTcpNoDelay(bool on);

private:
	int sockfd_;
};

}
}

#endif //SERVER_NET_SOCKET_H