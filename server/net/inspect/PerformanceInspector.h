#ifndef SERVER_NET_INSPECT_PERFORMANCEINSPECTOR_H
#define SERVER_NET_INSPECT_PERFORMANCEINSPECTOR_H

#include <server/net/inspect/Inspector.h>
#include <boost/noncopyable.hpp>

namespace server
{
namespace net
{

class PerformanceInspector : boost::noncopyable
{
 public:
  void registerCommands(Inspector* ins);

  static string heap(HttpRequest::Method, const Inspector::ArgList&);
  static string growth(HttpRequest::Method, const Inspector::ArgList&);
  static string profile(HttpRequest::Method, const Inspector::ArgList&);
  static string cmdline(HttpRequest::Method, const Inspector::ArgList&);
  static string memstats(HttpRequest::Method, const Inspector::ArgList&);
  static string memhistogram(HttpRequest::Method, const Inspector::ArgList&);

  static string symbol(HttpRequest::Method, const Inspector::ArgList&);
};

}
}

#endif  // SERVER_NET_INSPECT_PERFORMANCEINSPECTOR_H
