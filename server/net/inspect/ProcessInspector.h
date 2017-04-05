#ifndef SERVER_NET_INSPECT_PROCESSINSPECTOR_H
#define SERVER_NET_INSPECT_PROCESSINSPECTOR_H

#include <server/net/inspect/Inspector.h>
#include <boost/noncopyable.hpp>

namespace server
{
namespace net
{

class ProcessInspector : boost::noncopyable
{
 public:
  void registerCommands(Inspector* ins);

  static string overview(HttpRequest::Method, const Inspector::ArgList&);
  static string pid(HttpRequest::Method, const Inspector::ArgList&);
  static string procStatus(HttpRequest::Method, const Inspector::ArgList&);
  static string openedFiles(HttpRequest::Method, const Inspector::ArgList&);
  static string threads(HttpRequest::Method, const Inspector::ArgList&);

  static string username_;
};

}
}

#endif  // SERVER_NET_INSPECT_PROCESSINSPECTOR_H
