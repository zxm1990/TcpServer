#ifndef SERVER_BASE_STRINGPIECE_H
#define SERVER_BASE_STRINGPIECE_H

#include <string.h>
#include <iosfwd>

#include <server/base/Types.h>
#ifndef SERVER_STD_STRING
#include <string>
#endif

namespace server
{

class StringArg
{
public:
	StringArg(const char *str)
		: str_(str)
	{

	}
	StringArg(const string &str)
		:str_(str.c_str())
	{

	}

#ifndef SERVER_STD_STRING
	StringArg(const std::string &str)
		:str_(str.c_str())
	{

	}
#endif

	const char* c_str() const
	{
		return str_;
	}

private:
	const char* str_;
	
};

class StringPiece
{
public:
	StringPiece()
		: ptr_(NULL), 
		  length_(0)
	{

	}

	StringPiece(const char *str)
		: ptr_(str),
		  length_(static_cast<int>(strlen(ptr_)))
	{

	}

	StringPiece(const unsigned char *str)
		: ptr_(reinterpret_cast<const char*>(str)),
		  length_(static_cast<int>(strlen(ptr_)))
	{
		
	}

	StringPiece(const string &str)
		: ptr_(str.data()),
		  length_(static_cast<int>(str.size()))
	{
		
	}

#ifndef SERVER_STD_STRING
	StringPiece(const std::string &str)
		: ptr_(str_.date()),
		  length_(static_cast<int>(str.size()))
	{
		
	}
#endif

	StringPiece(const char* offset, int len)
    	: ptr_(offset), length_(len) 
    { 

    }
	
	const char* data() const
	{
		return ptr_;
	}

	int size() const 
	{
		return length_;
	}

	bool empty() const 
	{
		return length_ == 0;
	}

	const char* begin() const
	{
		return ptr_;
	}

	const char* end() const
	{
		return ptr_ + length_;
	}

	void clear()
	{
		ptr_ = NULL;
		length_ = 0;
	}

	void set(const char *buffer, int len)
	{
		ptr_ = buffer;
		length_ = len;
	}

	void set(const char *str)
	{
		ptr_ = str;
		length_ = static_cast<int>(strlen(str));
	}

	void set(const void *buffer, int len)
	{
		ptr_ = reinterpret_cast<const char*>(buffer);
		length_ = len;
	}

	char operator[](int i) const
	{
		return ptr_[i];
	}

	void remove_prefix(int n)
	{
		ptr_ += n;
		length_ -= n;
	}

	void remove_suffix(int n)
	{
		length_ -= n;
	}

	bool operator==(const StringPiece &x) const
	{
		return ((length_ == x.length_) 
				&& (memcmp(ptr_, x.ptr_, length_) == 0));
	}

	bool operator!=(const StringPiece &x) const
	{
		return !(*this == x);
	}

#define STRINGPIECE_BINARY_PREDICATE(cmp,auxcmp)                             \
  	bool operator cmp (const StringPiece& x) const {                         \
    int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_); \
    return ((r auxcmp 0) || ((r == 0) && (length_ cmp x.length_)));          \
  	}
  	STRINGPIECE_BINARY_PREDICATE(<,  <);
  	STRINGPIECE_BINARY_PREDICATE(<=, <);
 	STRINGPIECE_BINARY_PREDICATE(>=, >);
  	STRINGPIECE_BINARY_PREDICATE(>,  >);
#undef STRINGPIECE_BINARY_PREDICATE

 	int compare(const StringPiece& x) const 
 	{
    	int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
    	if (r == 0) 
    	{
      		if (length_ < x.length_) r = -1;
      		else if (length_ > x.length_) r = +1;
    	}
    	return r;
  	}

  	string as_string() const 
  	{
    	return string(data(), size());
  	}

  	void CopyToString(string* target) const 
  	{
  		target->assign(ptr_, length_);
  	}

#ifndef MUDUO_STD_STRING
  	void CopyToStdString(std::string* target) const 
  	{
  		target->assign(ptr_, length_);
  	}
#endif

  // Does "this" start with "x"
  	bool starts_with(const StringPiece& x) const 
  	{
 		return ((length_ >= x.length_) && (memcmp(ptr_, x.ptr_, x.length_) == 0));
  	}
private:
	const char *ptr_;
	int        length_;
	
};

} //namespace server

// allow StringPiece to be logged
std::ostream& operator<<(std::ostream& o, const server::StringPiece& piece);


#endif //SERVER_BASE_STRINGPIECE_H