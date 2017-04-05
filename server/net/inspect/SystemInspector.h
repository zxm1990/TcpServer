#ifndef SERVER_NET_INSPECT_SYSTEMINSPECTOR_H
#define SERVER_NET_INSPECT_SYSTEMINSPECTOR_H

#include <server/net/inspect/Inspector.h>

namespace server
{
namespace net
{

class SystemInspector : boost::noncopyable
{
 public:
  void registerCommands(Inspector* ins);

  static string overview(HttpRequest::Method, const Inspector::ArgList&);
  static string loadavg(HttpRequest::Method, const Inspector::ArgList&);
  static string version(HttpRequest::Method, const Inspector::ArgList&);
  static string cpuinfo(HttpRequest::Method, const Inspector::ArgList&);
  static string meminfo(HttpRequest::Method, const Inspector::ArgList&);
  static string stat(HttpRequest::Method, const Inspector::ArgList&);
};

}
}

#endif  // SERVER_NET_INSPECT_SYSTEMINSPECTOR_H
