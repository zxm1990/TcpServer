#include <server/net/InetAddress.h>

#include <server/base/Logging.h>
#include <server/net/Endian.h>
#include <server/net/SocketsOps.h>

#include <netdb.h>
#include <strings.h>  // bzero
#include <netinet/in.h>

#include <boost/static_assert.hpp>

// INADDR_ANY use (type)value casting.
// #pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;//0.0.0.0
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;//127.0.0.1
// #pragma GCC diagnostic error "-Wold-style-cast"

//     /* Structure describing an Internet socket address.  */
//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

//     /* Internet address. */
//     typedef uint32_t in_addr_t;
//     struct in_addr {
//         in_addr_t       s_addr;     /* address in network byte order */
//     };

using namespace server;
using namespace server::net;

BOOST_STATIC_ASSERT(sizeof(InetAddress) == sizeof(struct sockaddr_in));

InetAddress::InetAddress(uint16_t port, bool loopbackOnly)
{
  bzero(&addr_, sizeof addr_);
  addr_.sin_family = AF_INET;
  in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
  addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
  addr_.sin_port = sockets::hostToNetwork16(port);
}

InetAddress::InetAddress(StringArg ip, uint16_t port)
{
  bzero(&addr_, sizeof addr_);
  sockets::fromIpPort(ip.c_str(), port, &addr_);
}

string InetAddress::toIpPort() const
{
  char buf[32];
  sockets::toIpPort(buf, sizeof buf, addr_);
  return buf;
}

string InetAddress::toIp() const
{
  char buf[32];
  sockets::toIp(buf, sizeof buf, addr_);
  return buf;
}

uint16_t InetAddress::toPort() const
{
  return sockets::networkToHost16(addr_.sin_port);
}

static __thread char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(StringArg hostname, InetAddress* out)
{
  assert(out != NULL);
  struct hostent hent;
  struct hostent* he = NULL;
  bzero(&hent, sizeof(hent));

#ifndef __MACH__
  int herrno = 0;
  int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);
#else
  he = gethostbyname(hostname.c_str());
  int ret = 0;
#endif
  if (ret == 0 && he != NULL)
  {
    assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
    out->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
    return true;
  }
  else
  {
    if (ret)
    {
      LOG_SYSERR << "InetAddress::resolve";
    }
    return false;
  }
}
