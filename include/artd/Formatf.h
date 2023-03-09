/*-
 * Copyright (c) 1991-2018 Peter Kennard and aRt&D Lab
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of the source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Any redistribution solely in binary form must conspicuously
 *    reproduce the following disclaimer in documentation provided with the
 *    binary redistribution.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'', WITHOUT ANY WARRANTIES, EXPRESS
 * OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  LICENSOR SHALL
 * NOT BE LIABLE FOR ANY LOSS OR DAMAGES RESULTING FROM THE USE OF THIS
 * SOFTWARE, EITHER ALONE OR IN COMBINATION WITH ANY OTHER SOFTWARE.
 * 
 *  $Id$
 */

#ifndef __artd_Formatf_h__
#define __artd_Formatf_h__

#include "artd/jlib_base.h"
#include "artd/int_types.h"
#include <stdarg.h>
#include "artd/FormatfArglist.h"

#define USE_VARIADIC_TEMPLATE 1

#ifdef __EMSCRIPTEN__
#pragma GCC diagnostic ignored "-Wkeyword-macro"
#endif
#define INL ARTD_ALWAYS_INLINE

ARTD_BEGIN

class FormatPtr 
{
public:
	
	inline FormatPtr(int charsize, const void *fmt)
		: fmt_(fmt)
		, charsize_(charsize)
	{}
	INL FormatPtr(const wchar_t *fmt)
		: fmt_(fmt)
		, charsize_(sizeof(wchar_t))
	{}
	INL FormatPtr(const char *fmt)
		: fmt_(fmt)
		, charsize_(sizeof(char))
	{}

	const void *const   fmt_;
	const int           charsize_;
};



/*******
 *
 * This implements a "stream" that interprets posix "printf" style 
 * formatting.  It handles wide and "char" type chars.  the type expected
 * by %s is the type of the format string.
 * Added to the generic posix formatting specs are
 *
 * int64 types are preceeded by LL or ll ie %llu %llx %lld %llb
 * %t explicitly a "char" string
 * %w explicitly a wchar_t string 
 * %b integer as binary base 2 digits
 *          ie: "->%08b<-", 0x071 is formatted as "->01110001<-" 
 */

#pragma warning( push )
#pragma warning( disable : 26495)


class ARTD_API_JLIB_BASE FormatfStreamBase
{
	friend class FormatfPrivate;
public:

    typedef FormatfArgBase::Arg   Arg;

	INL FormatfStreamBase()
		{ }
	INL FormatfStreamBase(const char *fmt, va_list vargs) 
		{ va_init(fmt,vargs); }
	INL FormatfStreamBase(const wchar_t *fmt, va_list vargs) 
		{ va_init(fmt,vargs); }
	INL FormatfStreamBase(const FormatPtr &fp, va_list vargs) 
		{ va_init(fp,vargs); }
	
	INL FormatfStreamBase(const char *fmt, const FormatfArglist<> &arglist) 
    	{ va_init(fmt,arglist); }
	INL FormatfStreamBase(const wchar_t *fmt, const FormatfArglist<> &arglist) 
    	{ va_init(fmt,arglist); }
	INL FormatfStreamBase(const FormatPtr &fp, const FormatfArglist<> &arglist) 
    	{ va_init(fp,arglist); }
	
	INL ~FormatfStreamBase()
		{ }

	/** @brief returns length in characters (not bytes) that would result if 
	 *  this buffer was formatted. formats but does not load a buffer.
	 */
	int		lenf();

	/** @brief returns length in bytes that would result if 
	 *  this buffer was formatted to a 'char' string.
	 *  acounts for output encoding such as utf8.
	 *  formats but does not load a buffer.
	 */
	int		sizef();

	/** @brief gets a single character (not byte or char) 
	 *  from the stream
	 */  
	int		get();

	/** 
	 * reads up to maxlen bytes from stream.
	 * returns the NUMER OF BYTES read - not chars
	 * returns 0 if none are left.  Does not
	 * null terminate. may be called repeatedly 
	 * to read long results.
	 */
	int     read(char  *out,int maxbytes);
	int     read(wchar_t *out,int maxbytes);

	/**
	 * same as read() but null terminates
	 * the output.  It will read up to 
	 * maxlen - 1 chars so as to have room
	 * for the null termination.
	 */
	int     sprintf(char *out,int maxchars);
	int     sprintf(wchar_t *out,int maxchars);
	
	/**
	 * use below like FormatfStream().sprintf(out,max,fmt,...);
	 */
	int     sprintf(wchar_t *out, int maxchars, const char *fmt, ... );
	int     sprintf(wchar_t *out, int maxchars, const wchar_t *fmt, ... );
	int     sprintf(char *out, int maxchars, const char *fmt, ... );
 
	/**
	 * returns total number of *characters* read
	 * from this stream since initialization
	 */
	int     totalRead() { return(count_); }

 #ifdef USE_VARIADIC_TEMPLATE
	/** @brief returns number of characters the current
	 * stream "contains", when it formats.
	 * without putting anything in a buffer.
	 */
	int vlenf(const char *fmt, const FormatfArglist<> &args)
	{
		va_init(fmt,args);
		return(lenf());
	}  
	int vlenf(const wchar_t *fmt, const FormatfArglist<> &args)
	{
		va_init(fmt,args);
		return(lenf());
	} 
	/** @brief returns number of chars (bytes) the current
	 * stream "contains", when it formats.
	 * without putting anything in a buffer.
	 */
	int vsizef(const char *fmt, const FormatfArglist<> &args)
	{
		va_init(fmt,args);
		return(sizef());
	}  
	int vsizef(const wchar_t *fmt, const FormatfArglist<> &args)
	{
		va_init(fmt,args);
		return(sizef());
	} 
	int vsprintf(char *out, int maxchars, const char *fmt,const FormatfArglist<> &args)
	{
		va_init(fmt,args);
		return(sprintf(out,maxchars));
	}
	int vsprintf(wchar_t *out, int maxchars, const wchar_t *fmt, const FormatfArglist<> &args)
	{
		va_init(fmt,args);
		return(sprintf(out,maxchars));
	}

    #endif // end of use FormatfArglist


	/** @brief returns number of characters the current
	 * stream "contains", when it formats.
	 * without putting anything in a buffer.
	 */
	int vlenf(const char *fmt, va_list vargs)
	{
		va_init(fmt,vargs);
		return(lenf());
	}  
	int vlenf(const wchar_t *fmt, va_list vargs)
	{
		va_init(fmt,vargs);
		return(lenf());
	} 
	/** @brief returns number of chars (bytes) the current
	 * stream "contains", when it formats.
	 * without putting anything in a buffer.
	 */
	int vsizef(const char *fmt, va_list vargs)
	{
		va_init(fmt,vargs);
		return(sizef());
	}  
	int vsizef(const wchar_t *fmt, va_list vargs)
	{
		va_init(fmt,vargs);
		return(sizef());
	} 

	int vsprintf(char *out, int maxchars, const char *fmt, va_list vargs)
	{
		va_init(fmt,vargs);
		return(sprintf(out,maxchars));
	}
	int vsprintf(wchar_t *out, int maxchars, const wchar_t *fmt, va_list vargs)
	{
		va_init(fmt,vargs);
		return(sprintf(out,maxchars));
	}

	/**
	 * initialize formatter for retrieving arguments from
	 * a va_list
	 */
	void va_init(const char *fmt, va_list vargs)
	{
		#ifdef __GNUC__
			__va_copy(argbuf_.va, vargs);
		#else
			argbuf_.va  = vargs;
		#endif
		root_       = geta_fmtchar;
		arg_        = &vargGetter_;
		argType_	= 0;
		startFormat(fmt);
	}
	void va_init(const wchar_t *fmt, va_list vargs)
	{
		#ifdef __GNUC__
			__va_copy(argbuf_.va, vargs);
		#else
			argbuf_.va  = vargs;
		#endif
		root_       = geta_fmtchar;
		arg_        = &vargGetter_;
		argType_	= 0;
		startFormat(fmt);
	}
	void va_init(const FormatPtr &fp, va_list vargs)
	{
		#ifdef __GNUC__
			__va_copy(argbuf_.va, vargs);
		#else
			argbuf_.va  = vargs;
		#endif
		root_       = geta_fmtchar;
		arg_        = &vargGetter_;
		argType_	= 0;
		if(fp.charsize_ == sizeof(char)) {
			startFormat((const char *)fp.fmt_);
		} else {
			startFormat((const wchar_t *)fp.fmt_);
		}
	}

	void va_init(const char *fmt, const FormatfArglist<> &arglist)
	{
        argbuf_.args = arglist.args();
        argCount_	= arglist.argCount();
		root_       = geta_fmtchar;
		arg_        = &arglistGetter_;
		startFormat(fmt);
	}
	void va_init(const wchar_t *fmt,const FormatfArglist<> &arglist)
	{
		argbuf_.args = arglist.args();
		argCount_ = arglist.argCount();
		root_ = geta_fmtchar;
		arg_ = &arglistGetter_;
		startFormat(fmt);
	}
	void va_init(const FormatPtr &fp, const FormatfArglist<> &arglist)
	{
        argbuf_.args = arglist.args();
        argCount_	= arglist.argCount();
		root_       = geta_fmtchar;
		arg_        = &arglistGetter_;
		if(fp.charsize_ == sizeof(char)) {
			startFormat((const char *)fp.fmt_);
		} else {
			startFormat((const wchar_t *)fp.fmt_);
		}
	}
	/** @brief special case for initializing with one argument and formatting only it with no 
	 *  added format specifications. ie: "%s" not "%.4s", format type selected by arg type.
	 */
	void va_init_one(FormatfArglist<>::Arg *theOne);

	/**
	 * Once argbuf_.va, root_, and arg_ are initialized 
	 * this will initialize the formatter to begin 
	 * formatting.  
	 * Sets count_ to 0.
	 * and fmt_ to fmt
	 * the default char type "%s" is defined by the type 
	 * of fmt
	 */
	void startFormat(const wchar_t *fmt);
	void startFormat(const char *fmt);
	

	// **************************************************************************
	
	/** Parsing format type values desired by a format string
	 *
	 * use startParse() to initialize the formatter to only parse
	 * the format string to return the data described below.
	 * the formatter must be re-initialized with va_init() for
	 * formatting if the same instance is to be used subsequently
	 * for formatting since initParse will overwrite fetch_ and root_
	 */
	void initParse(const char *fmt);
	void initParse(const wchar_t *fmt);

	/**
	 * After initialized with initParse() get(), read()
	 * and sprintf() will return characters that represent the types desired
	 * by the format. The count field will represent the number of types. The string
	 * will be null terminated as a formatted string.  The data is bit packed and
	 * is extracted with the macros below.
	 *
	 * data type indicator in lower 3 bits if char 
	 */
	enum {
		FT_INT          = 0,
		FT_LONG         = 1,
		FT_DLONG        = 2,

		// double type 

		FT_DOUBLE       = 3,

		// pointer types

		FT_VOID_PTR     = 4,
		FT_CHAR_PTR     = 5,
		FT_WCHAR_PTR    = 6,
		FT_SHORT_PTR    = 7,    
		FT_INT_PTR      = 8,
	
		// special value for a '*' type argument 

		FT_STARTYPE = ((unsigned)(('*'<<4)|FT_INT)),
	};

	/** this extracts the type id from the fa.get() output for format parsing
	 *  fmtType() will extract the type numbers from the returned chars
	 */
	static INL int fmtType(int c) { return((c)&0x0F); }

	/** fmtChar() extracts the format char type ie: "%s" will give 's' 
	 *
	 * The chars map as follows: "*cdieEfgGnopsuxX"
	 *                       to: "jcdieefggnopsuxx"
	 *
	 * "%s abc %*.2s %*.*G %f %n %w" will return chars "sjsjjgfnw"
	 */
	static INL int fmtChar(int c) { return((((unsigned)c)>>4)|0x60); }

	
	class ArgGetter {
	public:
		double  (*getDouble)(FormatfStreamBase *fa);
		int     (*getInt)(FormatfStreamBase *fa);
		short   (*getShort)(FormatfStreamBase *fa);
		long    (*getLong)(FormatfStreamBase *fa);
		const void *(*getPointer)(FormatfStreamBase *fa);
		int64_t (*getInt64)(FormatfStreamBase *fa);
	};

	const void *	fmt_;		// this will contain pointer to next char in format 
								            // string after any get() call
	const ArgGetter *arg_;

protected:

	/**
	 * root level getch for printf() style formats
	 */
	static int geta_fmtchar(FormatfStreamBase *fa);
	static const ArgGetter vargGetter_;
	static const ArgGetter arglistGetter_;

	enum {

		Err_format_invalid  = -2,
		Err_too_many_args   = -3,
		Err_n_args_invalid  = -4,
		Err_bad_argnum      = -5,
		Err_invalid_utf8    = -6,

		// flags for mflags set by formatter - read only by caller 
		PARSE_MODE      = 0x01,  // if set, get() will only return the 
								 // conversion type bytes ie the "*s" for "abc%2.*s"
								 // to pre-scan format strings 

		FORMAT_DONE     = 0x02,  // a format operation was done 
		ABORT_ON_ERROR  = 0x04,  // abort format if an invalid format is 
								 // discovered put error code in error_

		DEFAULT_TO_WCHAR = 0x08,  // If set we default to wchar_t strings for %s
	};


	union Argbuf {
		va_list va;			// buffer of arguments to be formatted depends on type of list initialized with
        const Arg *args;
	} argbuf_;

    int argCount_;  // used for variadic template args "Arg" only
	int argType_;   // valid type for FormatfArglist set to tNONE (0) otherwise

	typedef int (*GetchT)(FormatfStreamBase *fa);  // current 'getchar' function

	GetchT          fgetch_;   // current 'getchar' function
	GetchT          root_;     // root state to return to
	int             count_;    // accumulates count of chars (not bytes) read since start
	const char *    prefix_;   // this will contain a pointer to '%' that starts the
                               // last read format desriptor when in format parse mode 

	unsigned int    mflags_;   // see values above  
	int		        error_;	   // error code if there is an error, only valid when 
                               // return char is 0

	/**
	 * function to the get next char from a string this is set by 
	 * startFormat() to either getBuffChar8 or getBufWchar.
	 * To override it for a particular invocation set the value
	 * of nextfmtchar_ after startFormat()  this is the only 
	 * thing that alters the value in fmt_ once it is set.
	 */

	typedef int	GetcRet; 
	typedef GetcRet (*GetBufChar)(FormatfStreamBase *fa, const void **buf);

	static GetcRet getBufChar8(FormatfStreamBase *fa, const void **buf);
	static GetcRet getBufCharUtf8(FormatfStreamBase *fa, const void **buf);
	static GetcRet getBufWchar(FormatfStreamBase *fa, const void **buf); 

	GetBufChar		nextfmtchar_;
};

class ARTD_API_JLIB_BASE FormatfStream 
	: public FormatfStreamBase
{
	friend class FormatfStreamBase;
public:
	INL FormatfStream() : FormatfStreamBase() { }
	INL FormatfStream(const char *fmt, va_list vargs) : FormatfStreamBase(fmt,vargs) { } 
	INL FormatfStream(const wchar_t *fmt, va_list vargs) : FormatfStreamBase(fmt,vargs) { }
	INL FormatfStream(const FormatPtr &fp, va_list vargs) : FormatfStreamBase(fp,vargs) { }
	INL ~FormatfStream() { }
protected:
	/** private stuff is put in the cpp file to keep the 
	 * public header clean
	 */
	enum {
		#ifdef ARTD_64BIT
			PrivateSize = 88 + (62 * sizeof(char)), 
		#else
			PrivateSize = 46 + (62 * sizeof(char)), 
		#endif
	};
	
	char _private_[PrivateSize];  // used internally, keeps public header clean
};


#pragma warning( pop ) 


/*** to be used like va_start() and va_end() for a FormatfStream ***/

#define FormatfStream_start(fa,fmt)  va_list fa ## _args_; { va_start( fa ## _args_ ,fmt); fa.va_init(fmt, fa ## _args_); }
#define FormatfStream_end(fa) va_end( fa ## _args_); 


/***** a simple looping formatter that uses (and will not overrun) a small buffer ***
 *
 *  void TRACE(const char *fmt, ... )
 *  {
 *  char buf[16];
 *  artd::FormatfStream fa;
 *
 *      FormatfStream_start(fa,fmt);
 *      int len;
 *      while((len = fa.read(buf,sizeof(buf)-1)) > 0)
 *      {
 *          buf[sizeof(buf)-1] = 0;
 *          ::OutputDebugString(buf);
 *      }
 *      FormatfStream_end(fa);
 *  }
 */


ARTD_END


#undef INL

#endif //__artd_Formatf_h__
