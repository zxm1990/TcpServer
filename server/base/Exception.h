#ifndef SERVER_BASE_EXCEPTION_H
#define SERVER_BASE_EXCEPTION_H

#include <server/base/Type.h>
#include <exception>

namespace muduo
{

class Exception : public std::exception
{
public:
	explicit Exception(const char *what);
	explicit Exception(const string &what);
	virtual ~Exception() throw();
	virtual const char* what() const throw();
	const char* stackTrace() const throw();

private:
	void fillStackTrace();
	string message_;
	string stack_;
	
};
}


#endif //SERVER_BASE_EXCEPTION_H