#ifndef __artd_StringArg_h
#define __artd_StringArg_h


#include "artd/jlib_base.h"
#include "artd/os_string.h"

#include <ostream>

// forward declaration of std::string types (messy)

#ifdef ARTD_SUPPORT_STD_STRING

#ifdef _MSC_VER

namespace std {

// forward declare std::string (very messy !!)

	template<typename CharT> struct char_traits;
	template<typename AllocT> class allocator;
	template <typename CharT, typename TraitT, typename allocT > class basic_string;

	typedef basic_string<char, char_traits<char>, allocator<char> >
		string;

	typedef basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >
		wstring;
}

#else
	#include <string>
#endif

#endif // ARTD_SUPPORT_STD_STRING

ARTD_BEGIN

#define INL ARTD_ALWAYS_INLINE

template<class T> class ObjectPtr;
class RcString;
class RcWString;
class FilePath;

template<class ChT> class string_object;


/** @brief for easy conversion of various string 
 * types (pre declared) into one argument type 
 * does not hold a reference to any of the string types 
 * can only be used for "const" imutable values
 */

template<typename CharT> 
class string_arg_base {
protected: 
	string_arg_base() {}
};

template<typename CharT>
class string_arg
	:  public string_arg_base<CharT> 
{
public:
	
	enum SType {
		C_STRING = 0,
		STD_STRING,
		RC_STRING
	};
	
protected:
	const CharT *	s_;
	const void  *	obj_;
	SType			type_;
	
	typedef string_arg<CharT> MyType;
	
public:
	
	INL string_arg()
		: s_(0), obj_(0), type_(C_STRING)
	{}
	
	INL string_arg(const int &v)
		: s_(0), obj_(0), type_(C_STRING)
	{}
	
	INL string_arg(CharT *v)
		: s_(v), obj_(0), type_(C_STRING)
	{}
	
	INL string_arg(const CharT *v)
		: s_(v), obj_(0), type_(C_STRING)
	{}

#ifdef ARTD_SUPPORT_STD_STRING
	INL string_arg(const std::basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > &s)
		: s_(s.c_str()), obj_(0), type_(STD_STRING)
	{}
#endif	

	string_arg(const RcString& rc);

	INL const MyType &operator=(const MyType &s)
	{
		this->~MyType();
		return(*new(this) MyType(s));
	}

	int lengthFromRc() const;

public:
	
	INL SType type() const { return(type_); }
	INL const void *getObj() const { return(obj_); }
	INL const CharT *c_str() const { return(s_); }

	int length() const {
		if (type_ == RC_STRING) {
			if (obj_ != nullptr) {
				lengthFromRc();
			}
		}
		if (s_ == NULL) {
			return(0);
		}
		if (sizeof(CharT) == sizeof(char)) {
			return(::strlen((const char *)s_));
		}
		else {
			return(::wcslen((const wchar_t *)s_));
		}
	}

	INL bool operator ==(const MyType &b) const { return(s_ == b.s_); }
	INL bool operator !=(const MyType &b) const { return(s_ != b.s_); }

	INL operator const bool() const { return(s_ != nullptr); }

#ifdef __GNUC__
//	INL bool operator ==(const nullptr_t p) const { return(s_ == 0); }
//	INL bool operator !=(const nullptr_t p) const { return(s_ != 0); }
	INL bool operator ==(const void *p) const { return((void *)s_ == p); }
	INL bool operator !=(const void *p) const { return((void *)s_ != p); }
//	INL operator void*() const { return(s_); }
	INL bool operator !() const { return(s_ == 0); }
//	INL operator bool() const { return(s_ != 0); }
#else 
	INL bool operator ==(const int &b) const { return(s_ == 0); }
	INL bool operator !=(const int &b) const { return(s_ != 0); }
#endif

};

typedef string_arg<char> StringArg;
typedef string_arg<wchar_t> WStringArg;

#undef INL


ARTD_END


ARTD_ALWAYS_INLINE std::ostream& operator<<(std::ostream& os, const artd::StringArg& v) {
	os << v.c_str();
	return(os);
}


#endif // __artd_StringArg_h

