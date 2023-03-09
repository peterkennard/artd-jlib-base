/*-
 * Copyright (c) 1991-2022 Peter Kennard and aRt&D Lab
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
 * 	$Id$
 */

#include "artd/pointer_math.h"
#include "artd/Formatf.h"

#include <ctype.h>
#include <math.h>
#include <cmath>
#include <stdlib.h>

#if defined(_WIN32) || defined(ARTD_IOS)
	#include <float.h>
#endif

// defining this will compile for utf8 encoding on char *strings.
// otherwise they are treated as full 8 bit chars.
#define FORMATF_UTF8

#include <string.h>
#include <stdio.h>


#include "artd/cstring_util.h"
#include "artd/RcString.h"
#include "artd/utf8util.h"

ARTD_BEGIN

#define INL ARTD_ALWAYS_INLINE

static const char nillstr[] = { '(','n','i','l',')',0 };

FormatfStreamBase::GetcRet FormatfStreamBase::getBufWchar(FormatfStreamBase* fa, const void** buf)
{
	GetcRet ret = *((wchar_t*)*buf);
	*buf = &(((wchar_t*)*buf)[1]);
	return(ret);
}

FormatfStreamBase::GetcRet FormatfStreamBase::getBufCharUtf8(FormatfStreamBase *fa, const void **buf)
{
	return(Utf8::decode1((const char **)(buf)));
}

FormatfStreamBase::GetcRet FormatfStreamBase::getBufChar8(FormatfStreamBase *fa, const void **buf)
{
	GetcRet ret = *((char *)*buf);
	*buf = &(((char *)*buf)[1]);
	return(ret);
}

static int StrLen(const char *str)
// return length of string
{
const char *pchar = str;

	while(*pchar) ++pchar;
	return((int)(pchar - str));
}
static void StrSet(char *str, int val, int count)
// return length of string
{
char *pend = str + count;

	while(str < pend) {
		*str++ = val;
	}
}

class FormatfPrivate
	: public FormatfStreamBase
{
public:

	inline static FormatfPrivate *fa(FormatfStreamBase *fa_)
	{
		return(static_cast<FormatfPrivate *>(fa_));
	}

	GetBufChar		nextstrchar_;
	inline void setNextBchar(GetBufChar getch)
	{
		nextfmtchar_ = getch;
	}

	unsigned short  parse_stars_;   // only used in format mode
	unsigned short  pflags_;	    // flags internal to do_fmtchar

	short	        width_;
	short	        zeropad_;
	int		        precis_;
	int		        strlen_;
	char *	        str_;       // string pointer when outputting string chars.
	double          darg_;
	short			outflags_;  // for output encoding when reading to char *buffer
	short			outatom_;   // length of remaining output atom (for utf8)
	char	        strbuf_[68];// needs to be big enough to hold largest formatted
								// double or 64 bit integer (as a string)

	INL RcString& bufRcString() {
		return(*reinterpret_cast<RcString*>(&strbuf_[50]));
	}

	/* ignore all chars and arguments, returning the format type chars only */
	static GetcRet do_parsechar(FormatfPrivate *fa)
	{
	const void *fmt = fa->fmt_;

		for(;;)
		{
			const char *prior = (char *)fmt;
			GetcRet nextchar = fa->nextfmtchar_(fa,&fmt);

			if(nextchar == '%')
			{
				fa->prefix_ = prior;
				fa->fmt_ = prior;
				return(fa->geta_fmtchar(fa));
			}
			if(nextchar == 0) {
				return(0);
			}
		}
	}

private:

	friend class FormatfStreamBase;

	inline void _sizecheck_()
	{
		class _check_ {
	    protected:
			char tst[ (sizeof(FormatfStream)) < sizeof(FormatfPrivate)
				? -1 : 1 ];
		};
	}

	typedef GetcRet (*GetchPriv)(FormatfPrivate *fa);
	inline void setGetch(GetchPriv getch)
	{
		fgetch_ = (GetchT)getch;
	}
	inline void setGetch(GetchT getch)
	{
		fgetch_ = getch;
	}

	// all this stuff is private to the class and no reason to put it in the public header file

	enum {
		FSPEC_PLUS    = 0x0001, // '+' flag was specified
		FSPEC_MINUS   = 0x0002, // '-' flag was specified
		FSPEC_SPACE   = 0x0004, // ' ' flag was specified
		FSPEC_PSIGN   = 0x0008, // '#' flag was specified
		FSPEC_ZERO    = 0x0010, // '0' flag was specified
		FSPEC_FDONE   = 0x0020, // no more flags accepted
		FSPEC_GOTWID  = 0x0040, // got a valid field width
		FSPEC_0WID    = 0x0080, // width spec'd is 0
		FSPEC_GOTDOT  = 0x0100, // found precision delimiter dot
		FSPEC_GOTPCIS = 0x0200, // got valid precision
		FSPEC_0PCIS   = 0x0400, // precision spec'd is 0
		FSPEC_WPDONE  = 0x0800, // no more 999.999 accepted
		FSPEC_SHORT   = 0x1000, // arg is treated as a short (16 bit)
		FSPEC_LONG    = 0x2000, // arg is treated as a long (32 bit)
		FSPEC_DLONG   = 0x4000, // arg is treated as a double long (64 bit)
		FSPEC_POINTER = 0x8000, // arg is treated as a pointer
	};

	enum { // outflags_
		OUTFLAG_HASATOM = 0x0001
	};


	// this may seem excessive, but having these functions as a virtual
	// interface alows this object to be used in other situations where
	// the arguments are not comming from a va_list but from items like
	// an arbitrary memory buffer


	#define getArgDouble(fa)  fa->arg_->getDouble(fa)
	#define getArgInt64(fa)   fa->arg_->getInt64(fa)
	#define getArgInt(fa)     fa->arg_->getInt(fa)
	#define getArgShort(fa)   fa->arg_->getShort(fa)
	#define getArgLong(fa)    fa->arg_->getLong(fa)
	#define getArgPointer(fa) fa->arg_->getPointer(fa)


	static int get_fmtint(FormatfPrivate *fa, GetcRet firstdig)

	// gets an int from the format string
	{
	const void *nextdigit;
	wchar_t digits[32];
	wchar_t *digit;
	wchar_t indig = firstdig;

		nextdigit = fa->fmt_;
		digit = &digits[0];
		for(;;)
		{
			*digit++ = indig;
			if(digit >= &digits[31])
				break;

			const void *prior = nextdigit;
			indig = fa->nextfmtchar_(fa,&nextdigit);
			if(!isdigit(indig))
			{
				nextdigit = prior;
				break;
			}
		}
		*digit = 0;
		fa->fmt_ = nextdigit;
		return(strtoi(digits));
	}
	static GetcRet trail_spaces(FormatfPrivate *fa)
	{
		if(!(--fa->width_))
			fa->setGetch(fa->root_);
		return(' ');
	}
	static GetcRet do_char(FormatfPrivate *fa)
	{
		if(fa->width_ > 1)
		{
			--fa->width_;
			if(fa->pflags_ & FSPEC_MINUS) // char first then spaces
				fa->setGetch(trail_spaces);
			else if(fa->width_)
				return(' ');
		}
		else
			fa->setGetch(fa->root_);

		return((wchar_t)getArgInt(fa));
	}

	static GetcRet ljust_limit_str(FormatfPrivate *fa)
	// limits string to precision length
	{
		void *ppstr = &(fa->str_);
		int nextc = fa->nextstrchar_(fa,(const void **)ppstr);

		if((nextc == 0) || (fa->precis_ == 0))
		{
			if(fa->width_ > 0)
				fa->setGetch(trail_spaces);
			else
				fa->setGetch(fa->root_);
			nextc = fa->fgetch_(fa);
		}
		return(nextc);
	}
	static GetcRet get_len_str(FormatfPrivate *fa)
	// gets string until length is exhausted.
	{
		if((--fa->strlen_) < 0) {
			fa->setGetch(fa->root_);
			return(fa->root_(fa));
		}
		void *ppstr = &fa->str_;
		return(fa->nextstrchar_(fa,(const void **)ppstr));
	}
	static GetcRet rjust_get_str(FormatfPrivate *fa)
	// assumes precision is length of string and width is leading spaces
	{
		if(fa->width_-- > 0)
			return(' ');
		fa->setGetch(get_len_str);
		return(get_len_str(fa));
	}
	static GetcRet doErrorString(FormatfPrivate *fa, const char *str)
	{
		fa->pflags_ &= ~(FSPEC_GOTPCIS|FSPEC_0PCIS|FSPEC_GOTDOT|FSPEC_MINUS);
		fa->str_ = (char *) str;
		fa->nextstrchar_ = getBufChar8;
		return(do_string(fa));
	}
	static GetcRet do_string(FormatfPrivate *fa)
	{
		// if no width set to limit only to the length of the string.
		if(!(fa->pflags_ & (FSPEC_GOTPCIS|FSPEC_0PCIS|FSPEC_GOTDOT))) {
			fa->precis_ = (int)(((unsigned )~0) >> 1); // really big
		}

		if(fa->pflags_ & FSPEC_MINUS)
		{
			// left justify
			fa->setGetch(ljust_limit_str);
			return(ljust_limit_str(fa));
		}
		// right justify
		const void *str = fa->str_;
		int len = 0;
		int maxlen = fa->precis_;
		while(len < maxlen && fa->nextstrchar_(fa,&str)) {
			++len;
		}
		if((fa->width_ -= len) < 0) {
			fa->width_ = 0;
		}
		fa->strlen_ = len;
		fa->setGetch(rjust_get_str);
		return(rjust_get_str(fa));
	}
	static GetcRet get_str(FormatfPrivate *fa)
	// returns locally formatted chars in fa->str_
	// fa->str_ must have a char first time called
	{
		--fa->width_;
		if(fa->str_[1] == 0)
		{
			if(fa->width_ > 0)
				fa->setGetch(trail_spaces);
			else
				fa->fgetch_ = fa->root_;
		}
		return(*(fa->str_)++);
	}
	static GetcRet get_zero_pad(FormatfPrivate *fa)
	// only called first time if fa->zeropad > 0
	{
		if(--fa->zeropad_ <= 0)
			fa->setGetch(get_str);
		--fa->width_;
		return('0');
	}
	static GetcRet pfix_str(FormatfPrivate *fa)
	{
		if(!(*(fa->prefix_)))
		{
			if(fa->zeropad_)
			{
				fa->setGetch(get_zero_pad);
				return(get_zero_pad(fa));
			}
			fa->setGetch(get_str);
			return(get_str(fa));
		}
		--fa->width_;
		return(*(fa->prefix_)++);
	}
	static GetcRet rjust_pfix_str(FormatfPrivate *fa)
	{
		if(fa->width_-- > 0)
			return(' ');
		fa->setGetch(pfix_str);
		return(pfix_str(fa));
	}
	static int upper_str(char *sstr)

	// returns length of string too
	{
	char *str = sstr;
	int c;

		while((c = *str) != 0)
		{
			if(c >= 'a' && c <= 'z')
				*str -= 'a' - 'A';
			++str;
		}
		return((int)(str - sstr));
	}
	static GetcRet finish_num_string(FormatfPrivate *fa)
	{
		if(fa->pflags_ & FSPEC_MINUS) // left justified
		{
			fa->setGetch(pfix_str);
			return(pfix_str(fa));
		}

		// right justified

		fa->width_ -= fa->strlen_; // now width left after strings and zero pad

		if(fa->pflags_ & FSPEC_ZERO) // whole field lead with zeros
		{
			if(fa->width_ > 0)
				fa->zeropad_ += fa->width_;
			fa->width_ = 0; // no space pad
		}
		fa->nextstrchar_ = getBufChar8;
		fa->setGetch(rjust_pfix_str);
		return(rjust_pfix_str(fa));
	}

	enum {
		INT_BASEMASK  = 0x00FF,
		INT_UNSIGNED  = 0x0100,
		INT_UCHEX	  = 0x0200
	};

	static GetcRet do_pointer(FormatfPrivate *fa)
	{
		fa->str_ = fa->strbuf_;
		fa->prefix_ = fa->strbuf_;

		union {
			const void *ptr;
			size_t num;
		} val;

		val.ptr = getArgPointer(fa);
		fa->zeropad_ = 0; // no zeros

		if(val.ptr == 0) {
			*fa->str_++ = 0;
			memmove(fa->str_,nillstr,sizeof(nillstr));
			fa->precis_ -= 3;
		} else {
			*fa->str_++ = '0';
			*fa->str_++ = 'x';
			*fa->str_++ =  0; // terminate prefix
			 --fa->precis_;   // a zero used

			 size_t shl = val.num;
			 while((shl & ((size_t)0x0F0 << ((sizeof(shl)-1)<<3))) == 0) {
				shl <<= 4;
				++fa->zeropad_;
			 }
			 sizettostr(val.num,fa->str_,16);
		}

		fa->strlen_ = StrLen(fa->str_); // length of digits

		if(fa->pflags_ & FSPEC_GOTPCIS) // got a precision, minimum digits
		{
			if(fa->strlen_ < fa->precis_)
			{
				fa->zeropad_ = fa->precis_ - fa->strlen_;
				fa->strlen_ = fa->precis_;
			}
		}
		fa->strlen_ += StrLen(fa->prefix_);
		return(finish_num_string(fa));
	}
	static GetcRet do_int(FormatfPrivate *fa,unsigned int base)
	{

	// this at least on MS compiler optimises to where the only the code required
	// is used. I'm not sure about GCC

#ifdef _MSC_VER
	#define INT_IS_SHORT (((unsigned int)~0) == ((unsigned int)((unsigned short)~0)))
	#define INT_IS_LONG  (((unsigned long)~0) == ((unsigned long)((unsigned int)~0)))
#else
#pragma GCC diagnostic ignored "-Wsign-compare"
	#define INT_IS_SHORT (((unsigned int)~0) == ((unsigned int)((unsigned short)~0)))
	#define INT_IS_LONG  (((unsigned long)~0) == ((unsigned long)((unsigned int)~0)))
#endif


		long val = 0;
		int64_t val64 = 0;

		fa->str_    = fa->strbuf_;
		fa->prefix_ = fa->strbuf_;

		// get arg of right size

		if(base & INT_UNSIGNED) // unsigned
		{
			if(fa->pflags_ & FSPEC_DLONG)
			{
				val64 = getArgInt64(fa);
				goto check_prefix;
			}

			if (!INT_IS_SHORT)
			{
				if(fa->pflags_ & FSPEC_SHORT)
				{
					val = (unsigned short)getArgShort(fa);
					goto check_prefix;
				}
			}
			if (!INT_IS_LONG)
			{
				if(fa->pflags_ & FSPEC_LONG)
				{
					val = (unsigned long)getArgLong(fa);
					goto check_prefix;
				}
			}

			val = (unsigned int)getArgInt(fa);
			goto check_prefix;
		}
		else // signed
		{
			if(fa->pflags_ & FSPEC_DLONG)
			{
				val64 = getArgInt64(fa);
				if(val64 < 0)
				{
					*fa->str_++ = '-';
					val64 = -val64;
					goto sign_done;
				}
				goto check_prefix;
			}

			if (!INT_IS_SHORT)
			{
				if(fa->pflags_ & FSPEC_SHORT)
				{
					val = getArgShort(fa);
					goto got_signed;
				}
			}
			if (!INT_IS_LONG)
			{
				if(fa->pflags_ & FSPEC_LONG)
				{
					val = getArgLong(fa);
					goto got_signed;
				}
			}

			val = getArgInt(fa);

		got_signed:

			if(val < 0)
			{
				*fa->str_++ = '-';
				val = -val;
				goto sign_done;
			}
		}

	check_prefix:

		if(fa->pflags_ & FSPEC_PLUS)
			*fa->str_++ = '+';
		else if(fa->pflags_ & FSPEC_SPACE)
			*fa->str_++ = ' ';

	sign_done:

		if((fa->pflags_ & FSPEC_PSIGN) && val != 0)
		{
			switch(base & INT_BASEMASK)
			{
				case 8:
					*fa->str_++ = '0';
					--fa->precis_;   // a zero used
					break;
				case 16:
					*fa->str_++ = '0';
					if(base & INT_UCHEX)
						*fa->str_++ = 'X';
					else
						*fa->str_++ = 'x';
					fa->precis_ -= 2;  // 2 "zeros" used
				default:
					break;
			}
		}

		*fa->str_++ = 0;  // terminate prefix

		// different formatter for different sizes
		if(fa->pflags_ & FSPEC_DLONG) {
			ul64tostr(val64,fa->str_,base & INT_BASEMASK); // Val positive at this point. Get digits
		} else {
			ultostr(val,fa->str_,base & INT_BASEMASK); // Val positive at this point. Get digits
		}

		if(base & INT_UCHEX) // fudge but works
			fa->strlen_ = upper_str(fa->str_);
		else
			fa->strlen_ = StrLen(fa->str_); // length of digits

		fa->zeropad_ = 0; // start with no zeros

		if(fa->pflags_ & FSPEC_GOTPCIS) // got a precision, minimum digits
		{
			if(fa->strlen_ < fa->precis_)
			{
				fa->zeropad_ = fa->precis_ - fa->strlen_;
				fa->strlen_ = fa->precis_;
			}
		}

		fa->strlen_ += StrLen(fa->prefix_);
		return(finish_num_string(fa));

		#undef INT_IS_SHORT
		#undef INT_IS_LONG
	}

	static void start_double(FormatfPrivate *fa)
	{
		fa->str_ = fa->strbuf_;
		fa->prefix_ = fa->strbuf_;
		if(fa->darg_ < 0.0)
		{
			*fa->str_++ = '-';
			fa->darg_ = -fa->darg_;
		}
		else
		{
			if(fa->pflags_ & FSPEC_PLUS)
				*fa->str_++ = '+';
			else if(fa->pflags_ & FSPEC_SPACE)
				*fa->str_++ = ' ';
		}

		fa->strlen_ = (int)(fa->str_ - fa->strbuf_);
		*fa->str_++ = 0; // terminate prefix
	}

	static inline const double *pcismult()
	{
		static const double pmult[] = {
			1.0,
			10.0,
			100.0,
			1000.0,
			10000.0,
			100000.0,
			1000000.0,
			10000000.0,
			100000000.0,
			1000000000.0,
			10000000000.0,
			100000000000.0,
			1000000000000.0,
			10000000000000.0,
			100000000000000.0
		};
		return(pmult);
	}

	static void pcis_roundit(FormatfPrivate *fa)
	{
		if(!(fa->pflags_ & (FSPEC_GOTPCIS|FSPEC_0PCIS)))
			fa->precis_ = 6;
		else if(((unsigned)fa->precis_) > 15)
			fa->precis_ = 15;

		// since converting to a long will round down (truncate)
		// adding the .(sigdig)5 will round on both negative and positive
		// side

		fa->darg_ += 0.5/(pcismult()[fa->precis_]);
	}
	static char *add_dubldigits(FormatfPrivate *fa,bool gmode)
	{
	double frac;
	double ival;
	unsigned long lval;
	int sigdig;
	int len;
	char *suffix;

		suffix = fa->str_;
		fa->zeropad_ = 0;

		if(std::isnan(fa->darg_)) {

		}

		if((sigdig = fa->precis_) == 0)
		{
			ultostr((unsigned long)(fa->darg_),suffix,10);
			len = StrLen(suffix);
			fa->strlen_ += len;
			return(suffix + len);
		}
		else
		{
			frac = modf(fa->darg_,&ival);
			lval = (unsigned long)ival;
			ultostr(lval,suffix,10);
			suffix += len = StrLen(suffix);
			fa->strlen_ += len;
			*suffix++ = '.';

			frac *= pcismult()[sigdig];

			ultostr(((unsigned long)frac),suffix,10);

			// ** 'G' mode chops off trailing zeros **
			if(gmode && (suffix[1] == 0) && (suffix[0] == '0'))
			{
				*(--suffix) = 0;
				return(suffix);
			}

			if((len = StrLen(suffix)) < sigdig)
			{
				memmove(suffix + (sigdig - len),suffix,len*sizeof(*suffix));
				StrSet(suffix,'0',sigdig - len);
			}

			suffix += sigdig;
			if(gmode) // chop zeros
			{
				while(*(--suffix) == '0')
					--sigdig;
				++suffix;
			}
			*suffix = 0;
			fa->strlen_ += sigdig + 1;
			return(suffix);
		}
	}
	static GetcRet checkDoubleType(FormatfPrivate *fa)
	{
		const char *str = 0;
	#ifdef ARTD_IOS
		if(fa->darg_ != fa->darg_) {
	#else
		if(std::isnan(fa->darg_)) {
	#endif
			if((*(int8_t *)&(fa->darg_)) & 0x80)
				str = "-1.#NAN";
			else
				str = "1.#NAN";
		}
	#ifdef ARTD_IOS
		else if(fa->darg_ < -DBL_MAX) {
			str = "-1.#INF";            
		} else if(fa->darg_ > DBL_MAX) {
			str = "1.#INF";
		}
	#else
		else if(!std::isfinite(fa->darg_)) {
			if((*(int8_t *)&(fa->darg_)) & 0x80)
				str = "-1.#INF";
			else
				str = "1.#INF";
		}
	#endif
		else {
			return(0);
		}
		return(doErrorString(fa,str));
	}
	static GetcRet do_fdouble(FormatfPrivate *fa)
	{
		{
		GetcRet ret;
			if((ret = checkDoubleType(fa))) {
				return(ret);
			}
		}
		start_double(fa);
		pcis_roundit(fa);
		add_dubldigits(fa,0);
		return(finish_num_string(fa));
	}
	static GetcRet do_edouble(FormatfPrivate *fa, bool gmode, GetcRet lastchar)
	{
	double darg;
	double odarg;
	char *suffix;
	int exp;
	int len;

		if((len = checkDoubleType(fa))) {
			return(len);
		}
		exp = 0;
		odarg = fa->darg_;
		start_double(fa);
		darg = fa->darg_;

		// shift over by base 10 radix until we get to d.dddd fit

		if(darg != 0.0)
		{
			if(darg < 1.0)
			{
				for(;;)
				{
					--exp;
					darg *= 10.0;
					if(darg >= 1.0)
						break;
				}
			}
			else while(darg >= 10.0)
			{
				++exp;
				darg /= 10.0;
			}
			fa->darg_ = darg;
		}
		else
		{
			exp = 1;
		}

		pcis_roundit(fa);
		if(fa->darg_ >= 10.0)
		{
			fa->darg_ /= 10.0;
			++exp;
		}

		if(gmode && (exp >= -4 && exp < fa->precis_))
		{
			fa->darg_ = odarg;
			start_double(fa);
			pcis_roundit(fa);
			add_dubldigits(fa,1);
			return(finish_num_string(fa));
		}

		suffix = add_dubldigits(fa,gmode);

		if(isupper(lastchar))
			*suffix++ = 'E';
		else
			*suffix++ = 'e';

		if(exp < 0)
		{
			*suffix++ = '-';
			exp = -exp;
		}
		else
			*suffix++ = '+';

		len = itostr(exp,suffix,10);
		if(len < 3)
		{
			memmove(suffix + (3 - len),suffix,len*sizeof(wchar_t));
			StrSet(suffix,'0',(3 - len));
			suffix[3] = 0;
		}
		fa->strlen_ += 5;
		return(finish_num_string(fa));
	}
	static GetcRet do_parse_stars(FormatfPrivate *fa)

	// this function is only used in parse mode, so it returns
	// two items, the STARTYPE, and next the composite type and format char
	{
		if(fa->parse_stars_)
		{
			--fa->parse_stars_;
			return(fa->FT_STARTYPE);
		}
		fa->setGetch(fa->root_);
		return((fa->strbuf_[0]<<4)|fa->strbuf_[1]);
	}

	static GetcRet returnNullChar(FormatfPrivate *fa)
	{
		return(0);
	}

public:

	static GetcRet geta_fmtchar(FormatfPrivate *fa)

	// outputs char or gets the format spec and calls function for argument type
	{
	GetcRet retchar;
	unsigned short flags;
	int typechar = 0;

	get_another_char: // here from below to reset things and dump format chars

		retchar = fa->nextfmtchar_(fa,&fa->fmt_);

		if(retchar == 0)
		{
			fa->setGetch(returnNullChar);
			return(0);
		}
		if(retchar != '%')
		{
			return(retchar);
		}

		const void *percentStart = fa->fmt_;

		// get optional leading flags
		fa->pflags_ = 0;
		fa->width_ = 0;
		fa->precis_ = 0;

		for(;;)
		{
			retchar = fa->nextfmtchar_(fa,&fa->fmt_);

			switch(retchar)
			{
	   // ********* conversion "flag" specifiers *************
				case ' ':
					flags = FSPEC_SPACE;
					goto check_fflags;
				case '-':
					flags = FSPEC_MINUS;
					goto check_fflags;
				case '+':
					flags = FSPEC_PLUS;
					goto check_fflags;
				case '#':
					flags = FSPEC_PSIGN;
					goto check_fflags;
				case '0':
					if(fa->pflags_ & FSPEC_GOTDOT)
						goto get_fmt_precis;
					flags = FSPEC_ZERO;

				check_fflags:
					if(flags & fa->pflags_ || fa->pflags_ & FSPEC_FDONE)
						goto error;
					break;

	   // ******** conversion field width and precision specifiers ******

				case '.':
					flags = FSPEC_GOTDOT;
					goto check_ctflags;
				case '*':
					if(fa->pflags_ & FSPEC_GOTDOT)
					{
						flags = FSPEC_GOTPCIS;
						if(fa->mflags_ & fa->PARSE_MODE)
							goto inc_pstars;

						else if(0 == (fa->precis_ = getArgInt(fa)))
							flags = FSPEC_0PCIS;
					}
					else
					{
						flags = FSPEC_GOTWID;
						if(fa->mflags_ & fa->PARSE_MODE)
							goto inc_pstars;

						if(0 == (fa->width_ = getArgInt(fa)))
							flags = FSPEC_0WID;
					}
					goto check_ctflags;
				inc_pstars:
					++fa->parse_stars_;
					goto check_ctflags;
				default:

					if(!isdigit(retchar))
					{
						if(fa->pflags_)
							goto error;
						goto done;
					}

					if(fa->pflags_ & FSPEC_GOTDOT)
					{
				get_fmt_precis:
						flags = FSPEC_GOTPCIS;
						if(0 == (fa->precis_ = get_fmtint(fa,retchar)))
							flags = FSPEC_0PCIS;
					}
					else
					{
						flags = FSPEC_GOTWID;
						if(0 == (fa->width_ = get_fmtint(fa,retchar)))
							flags = FSPEC_0WID;
					}

				check_ctflags:
					if(flags & fa->pflags_ || fa->pflags_ & FSPEC_WPDONE)
						goto error;
					flags |= FSPEC_FDONE;
					break;
				//** argument size specifiers ***

				case 'h':
					flags = FSPEC_SHORT;
					goto check_szflags;
				case 'l':
				case 'L':
				{
					flags = FSPEC_LONG;
					const void *fmt = fa->fmt_;
					int rc = fa->nextfmtchar_(fa,&fmt);
					if(rc == retchar) {
						flags = FSPEC_DLONG;
						fa->fmt_ = fmt;
					}
					goto check_szflags;
				}
				check_szflags:
					if(flags & fa->pflags_)
						goto error;
					flags |= FSPEC_WPDONE;
					break;

		//*** finally  --  phew~~ conversion types ***

				case 'n':
				{
					if(fa->pflags_ & ~(FSPEC_SHORT|FSPEC_WPDONE))
						goto error;

					if(fa->mflags_ & fa->PARSE_MODE)
					{
						if(fa->pflags_ & FSPEC_SHORT) {
							typechar = fa->FT_SHORT_PTR;
						} else {
							typechar = fa->FT_INT_PTR;
						}
						goto got_type;
					}
					// note that fa->count_ is pre-incremented
					const void *pval = getArgPointer(fa);

					if(pval == 0) {
						goto do_null_pointer; // bolow after case 's'
					}

					if(fa->pflags_ & FSPEC_SHORT) {
						*((short *)pval) = fa->count_ - 1;
					} else {
						*((int *)pval) = fa->count_ - 1;
					}
					goto reset; // we don't need char, output only
				}

	   //*************** floating point types *****************

				case 'g':
				case 'G':
					if(fa->mflags_ & fa->PARSE_MODE)
						goto got_double_type;
					fa->darg_ = getArgDouble(fa);
					retchar = do_edouble(fa,1,retchar);
					goto done;
				case 'e':
				case 'E':
					if(fa->mflags_ & fa->PARSE_MODE)
						goto got_double_type;
					fa->darg_ = getArgDouble(fa);
					retchar = do_edouble(fa,0,retchar);
					goto done;
				case 'f':
					if(fa->mflags_ & fa->PARSE_MODE)
						goto got_double_type;
					fa->darg_ = getArgDouble(fa);
					retchar = do_fdouble(fa);
					goto done;

	   //*************** integer types *****************

				case 'b': // binary format 0xF0 = 11110000
					retchar = fa->INT_UNSIGNED | 2;
					goto do_format_integer;
				case 'c':
					if(fa->pflags_ & ~(FSPEC_MINUS|FSPEC_GOTWID|FSPEC_FDONE))
						goto error;
					if(fa->mflags_ & fa->PARSE_MODE)
						goto got_int_type;
					fa->setGetch(do_char);
					retchar = do_char(fa);
					goto done;
				case 'd':
				case 'i':
					retchar = 10;
					goto do_format_integer;
				case 'u':
					retchar = fa->INT_UNSIGNED | 10;
					goto do_format_integer;
				case 'o':
					retchar = fa->INT_UNSIGNED | 8;
					goto do_format_integer;
				case 'p': // pointer
					if (fa->mflags_ & fa->PARSE_MODE)
					{
						retchar = fa->FT_VOID_PTR;
						goto got_type;
					}
					retchar = do_pointer(fa);
					goto done;
				case 'X':
				case 'x':
					if(retchar == 'X')
						retchar = fa->INT_UNSIGNED | 16 | INT_UCHEX;
					else
						retchar = fa->INT_UNSIGNED | 16;

				do_format_integer:

					if(fa->mflags_ & fa->PARSE_MODE)
						goto got_int_type;
					retchar = do_int(fa,retchar);
					goto done;

	  // ***************** string types ******************

				case 's':
				case 'S':
					if(fa->mflags_ & fa->DEFAULT_TO_WCHAR) {
						goto do_wstring_type;
					}
				case 't': // text, explicit 'char' *
					#ifdef FORMATF_UTF8
						fa->nextstrchar_ = getBufCharUtf8;
					#else
						fa->nextstrchar_ = getBufChar8;
					#endif
					goto do_string_type;
				case 'w': // text explicit wchar_t *
				do_wstring_type:
					fa->nextstrchar_ = getBufWchar;
					goto do_string_type;
				do_string_type:
				{
					if(fa->pflags_ & ~(FSPEC_MINUS|FSPEC_FDONE
									 |FSPEC_GOTDOT
									 |FSPEC_GOTWID|FSPEC_0WID
									 |FSPEC_GOTPCIS|FSPEC_0PCIS
									 |FSPEC_WPDONE))
					{
						goto error;
					}
					if(fa->mflags_ & fa->PARSE_MODE)
					{
						if(fa->nextstrchar_ == getBufWchar) {
							typechar = fa->FT_WCHAR_PTR;
						} else {
							typechar = fa->FT_CHAR_PTR;
						}
						goto got_type;
					}

					const void *pstr = getArgPointer(fa);
					if(pstr != 0)
					{
						switch(fa->argType_) {
							case 0:
								if (retchar == 'S') // check for an 'Object'
								{
									const ObjectBase* pobj = static_cast<const ObjectBase*>(pstr);
									const RcString::ObjT *cstr = dynamic_cast<const RcString::ObjT*>(pobj);
									const RcWString::ObjT *wstr;
									if (cstr != 0) {
										pstr = cstr->c_str();
										fa->nextstrchar_ = getBufCharUtf8;
									}
									else if ((wstr = dynamic_cast<const RcWString::ObjT*>(pobj)) != 0) {
										pstr = wstr->c_str();
										fa->nextstrchar_ = getBufWchar;
									}
								}
								break;
							case FormatfArgBase::tRCSTR:  // first releasable object type
								pstr = static_cast<const RcString::ObjT *>(pstr)->c_str();
							case FormatfArgBase::tCHARS:
								fa->nextstrchar_ = getBufCharUtf8;
								break;
							case FormatfArgBase::tRCWSTR:
								pstr = static_cast<const RcWString::ObjT *>(pstr)->c_str();
							case FormatfArgBase::tWCHARS:
								fa->nextstrchar_ = getBufWchar;
								break;
							case FormatfArgBase::tOBJECT: {
								break;
							}
							default:
								fa->str_ = const_cast<char *>("(?)");
								fa->nextstrchar_ = getBufChar8;
								goto finish_string;
						}
						fa->str_ = (char *)pstr;
						if(fa->nextfmtchar_(fa,(const void **)&pstr) == 0)
						{
							if(!(fa->pflags_ & FSPEC_GOTWID)) // print nothing
								goto reset;
							fa->setGetch(trail_spaces);
							retchar = trail_spaces(fa);
							goto done;
						}
						goto finish_string;
					}
				}
			do_null_pointer:
				fa->str_ = const_cast<char *>(nillstr);
				fa->nextstrchar_ = getBufChar8;
			finish_string:
				retchar = do_string(fa);
				goto done;
			}
			fa->pflags_ |= flags;
			continue;
		reset:
			fa->mflags_ |= fa->FORMAT_DONE;
			goto get_another_char; //* go back to top and do next one *
		}

	done:
		fa->mflags_ |= fa->FORMAT_DONE;
		return(retchar);
	got_double_type:
		typechar = fa->FT_DOUBLE;
		goto got_type;
	got_int_type:

		if(fa->pflags_ & FSPEC_LONG)
			typechar = fa->FT_LONG;
		else if(fa->pflags_ & FSPEC_DLONG)
			typechar = fa->FT_DLONG;
		else
			typechar = fa->FT_INT;

	got_type:
		if(fa->parse_stars_)
		{
			fa->strbuf_[0] = retchar;
			fa->strbuf_[1] = typechar;
			fa->setGetch(do_parse_stars);
			return(do_parse_stars(fa));
		}
		return((retchar<<4) | typechar);

	error:

		fa->fmt_ = percentStart;
		if(fa->mflags_ & fa->ABORT_ON_ERROR)
		{
			fa->error_ = fa->Err_format_invalid;
			return(0);
		}
		if(fa->mflags_ & fa->PARSE_MODE)
		{
			fa->parse_stars_ = 0;
			return(geta_fmtchar(fa)); // note recursion, this will only go one level
									  // since the next char will NOT be a '%',
									  // a %% will be passed through as '%'
		}
		return('%');
	}

public:

	#undef getArgDouble
	#undef getArgInt
	#undef getArgShort
	#undef getArgLong
	#undef getArgPointer

// a very big internal class of static functions!
// this was originally written in C and only requires one entry point
// function.  It is a state machine that operates by swapping the
// pointer to the current "getchar" function.


// va_list arg getter functions used in vargGetter

	static double getvArgDouble(FormatfStreamBase *fa)
	{
		return(va_arg(fa->argbuf_.va, double));
	}
	static int getvArgInt(FormatfStreamBase *fa)
	{
		return(va_arg(fa->argbuf_.va, int));
	}
	static short getvArgShort(FormatfStreamBase *fa)
	{
#ifdef _MSC_VER
		return(va_arg(fa->argbuf_.va, short));
#else
		return(va_arg(fa->argbuf_.va, int));
#endif
	}
	static long getvArgLong(FormatfStreamBase *fa)
	{
		return(va_arg(fa->argbuf_.va, long));
	}
	static const void *getvArgPointer(FormatfStreamBase *fa)
	{
		return(va_arg(fa->argbuf_.va, void *));
	}
	static int64_t getvArgInt64(FormatfStreamBase *fa) {
		return(va_arg(fa->argbuf_.va, int64_t));
	}

	// va_list arg getter functions used in arglistGetter
	// TODO check arg array overrun as we have the info in this case

	static double getArglistDouble(FormatfStreamBase *fa) {
		const Arg *arg = fa->argbuf_.args;
		fa->argType_ = arg->type_;
		fa->argbuf_.args = arg + 1;
		return(arg->value_.double_);
	}
	static int getArglistInt(FormatfStreamBase *fa) {
		const Arg *arg = fa->argbuf_.args;
		fa->argType_ = arg->type_;
		fa->argbuf_.args = arg + 1;
		return(arg->value_.int_);
	}
	static short getArglistShort(FormatfStreamBase *fa) {
		const Arg *arg = fa->argbuf_.args;
		fa->argType_ = arg->type_;
		fa->argbuf_.args = arg + 1;
		return((short)(arg->value_.int_));
	}
	static long getArglistLong(FormatfStreamBase *fa) {
		const Arg *arg = fa->argbuf_.args;
		fa->argType_ = arg->type_;
		fa->argbuf_.args = arg + 1;
		return((int32_t)(arg->value_.int_));
	}
	static const void *getArglistPointer(FormatfStreamBase *fa) {
		const Arg *arg = fa->argbuf_.args;
		fa->argType_ = arg->type_;
		fa->argbuf_.args = arg + 1;
		return(arg->value_.ptr_);
	}
	static int64_t getArglistInt64(FormatfStreamBase *fa) {
		const Arg *arg = fa->argbuf_.args;
		fa->argType_ = arg->type_;
		fa->argbuf_.args = arg + 1;
		return(arg->value_.int64_);
	}

}; // end of FormatfPrivate;


const FormatfStreamBase::ArgGetter FormatfStreamBase::vargGetter_ =
{
	FormatfPrivate::getvArgDouble,
	FormatfPrivate::getvArgInt,
	FormatfPrivate::getvArgShort,
	FormatfPrivate::getvArgLong,
	FormatfPrivate::getvArgPointer,
	FormatfPrivate::getvArgInt64,
};

const FormatfStreamBase::ArgGetter FormatfStreamBase::arglistGetter_ =
{
	FormatfPrivate::getArglistDouble,
	FormatfPrivate::getArglistInt,
	FormatfPrivate::getArglistShort,
	FormatfPrivate::getArglistLong,
	FormatfPrivate::getArglistPointer,
	FormatfPrivate::getArglistInt64,
};


FormatfStreamBase::GetcRet FormatfStreamBase::geta_fmtchar(FormatfStreamBase *fa)
{
	return(FormatfPrivate::geta_fmtchar((FormatfPrivate *)fa));
}


void FormatfStreamBase::startFormat(const char *fmt)
{
	FormatfPrivate *pfa = FormatfPrivate::fa(this);
	FormatfPrivate &fa = *pfa;

	fgetch_ = root_;
	#ifdef FORMATF_UTF8
		fa.setNextBchar(getBufCharUtf8);
	#else
		fa.setNextBchar(getBufChar8);
	#endif
	fa.mflags_		= 0;
	fa.fmt_			= fmt;
	fa.outflags_	= 0;
	fa.count_		= 0;
	fa.error_		= 0;
}
void FormatfStreamBase::startFormat(const wchar_t *fmt)
{
	FormatfPrivate &fa = *FormatfPrivate::fa(this);
	startFormat((const char *)fmt);
	fa.mflags_ |= fa.DEFAULT_TO_WCHAR;
	fa.setNextBchar(getBufWchar);
}

static const char *const ArgFormats[
	(FormatfArgBase::tNONE == 0
		&& FormatfArgBase::tCHAR == 1
		&& FormatfArgBase::tWCHAR == 2
		&& FormatfArgBase::tINT == 3
		&& FormatfArgBase::tUINT == 4
		&& FormatfArgBase::tINT64 == 5
		&& FormatfArgBase::tUINT64 == 6
		&& FormatfArgBase::tREAL == 7
		&& FormatfArgBase::tPOINTER == 8
		&& FormatfArgBase::tCHARS == 9
		&& FormatfArgBase::tWCHARS == 10
		&& FormatfArgBase::tRCSTR == 11
		&& FormatfArgBase::tRCWSTR == 12
		&& FormatfArgBase::tOBJECT == 13
		&& FormatfArgBase::tNumTypes == 14)
	 ? 16 : -1] = 
{
	"",   // tNONE = 0,
	"%c", // tCHAR,
	"%c", // tWCHAR,
	"%d", // tINT,
	"%u", // tUINT,
	"%Ld", // tINT64,
	"%Lu", // tUINT64,
	"%f", // tREAL,
	"%p", // tPOINTER,
	"%s", // tCHARS,
	"%s", // tWCHARS,
	"%s", // tRCSTR,
	"%s", //tRCWSTR,
	"%s", //tOBJECT,
	"",
	""
};


void 
FormatfStreamBase::va_init_one(FormatfArglist<>::Arg *theOne) {
	argbuf_.args = theOne;
	argCount_ = 1;
	root_ = geta_fmtchar;
	arg_ = &arglistGetter_;
	startFormat(ArgFormats[theOne->type_ & 0x0F]);
}

void FormatfStreamBase::initParse(const char *fmt)
{
	ARTD_STATIC_ASSERT(FormatfStream::PrivateSize <= sizeof(FormatfPrivate));

	root_ = (GetchT)FormatfPrivate::do_parsechar;
	FormatfPrivate &fa = *FormatfPrivate::fa(this);
	startFormat(fmt);
	fa.mflags_ |= fa.PARSE_MODE;
	fa.parse_stars_ = 0;
}
void FormatfStreamBase::initParse(const wchar_t *fmt)
{
	FormatfPrivate &fa = *FormatfPrivate::fa(this);
	initParse((const char *)fmt);
	fa.mflags_ |= fa.DEFAULT_TO_WCHAR;
	fa.setNextBchar(getBufWchar);
}


int FormatfStreamBase::get()
{
	++count_;
	return(fgetch_(this));
}
int FormatfStreamBase::lenf()
{
int len = 0;

	while(get() != 0)
		++len;
	return(len);
}
int FormatfStreamBase::sizef()
{
int len = 0;

#ifdef FORMATF_UTF8
	int got;
	while((got = get()) != 0) {
		got = Utf8::utfOutSize(got);
		len += got;
	}
#else
	while(get() != 0)
		++len;
#endif
	return(len);
}

#ifdef FORMATF_UTF8
	int FormatfStreamBase::read(char *buf,int maxbytes)
	// does not null terminate !! and can be called multiple times until "empty"
	// returns number of *bytes* read (utf8) not number of chars. Will not split
	// utf8 atoms
	{
		FormatfPrivate &fa = *FormatfPrivate::fa(this);
		
		int cnt = 0;
		int bytesleft = maxbytes;

		for(;;)
		{
			if(fa.outflags_ & fa.OUTFLAG_HASATOM) 
			{
				if(fa.outatom_ > bytesleft) {
					goto done;
				}
				memcpy(buf,fa.strbuf_,fa.outatom_);
				buf += fa.outatom_;
				fa.outflags_ &= ~fa.OUTFLAG_HASATOM;
				bytesleft -= fa.outatom_;
			} 
			
			for(;;) 
			{
				if(bytesleft <= 0) {
					goto done;
				}
				int got = get();
				if(got < 0x80) 
				{		
					if(got == 0) {
						goto done;
					}
					*buf++ = (char)got;
					++cnt;
					--bytesleft;
					continue;
				}
				int utflen = Utf8::utfOutSize(got);
				if((bytesleft -= utflen) < 0) 
				{
					// defer putting in output until next read()				
					fa.outflags_ |= fa.OUTFLAG_HASATOM;
					Utf8::encode1(fa.strbuf_,got);
					goto done;
				}
				if(utflen < 0) {
					fa.error_ = Err_invalid_utf8;
					return(-1);
				}
				cnt += utflen;
				buf = Utf8::encode1(buf,got);
			}
		}
	done:
		return(cnt);
	}
#else
	int FormatfStreamBase::read(char *buf,int maxbytes)
	// does not null terminate !! and can be called multiple times until "empty"
	// returns number of *bytes* read not number of chars.
	{
	int cnt = 0;

		while(cnt < maxbytes)
		{
			if((*buf = get()) == 0)
				break;
			++buf;
			cnt += sizeof(*buf);
		}
		return(cnt);
	}
#endif // FORMATF_UTF8

int FormatfStreamBase::read(wchar_t *buf,int maxbytes)
// does not null terminate !! and can be called multiple times until "empty"
// returns number of *bytes* read not number of chars.
{
int cnt = 0;

	maxbytes  -= (maxbytes % sizeof(*buf));
	while(cnt < maxbytes)
	{
		if((*buf = get()) == 0)
			break;
		++buf;
		cnt += sizeof(*buf);
	}
	return(cnt);
}
/*
int FormatfStreamBase::read(int *buf,int maxbytes)
// does not null terminate !! and can be called multiple times until "empty"
// returns number of *bytes* read not number of chars.
{
int cnt = 0;

	maxbytes  -= (maxbytes % sizeof(*buf));
	while(cnt < maxbytes)
	{
		if((*buf = get()) == 0)
			break;
		++buf;
		cnt += sizeof(*buf);
	}
	return(cnt);
}
*/
int FormatfStreamBase::sprintf(wchar_t *buf,int maxchars)
// a self limiting sprintf that limits output size to maxlen.
// it can be called multiple times to get all the data if a length of 0 is passed
// in it will insert a null anyway;  Will never return maxlen only maximum of maxlen - 1
{
	int maxlen = (maxchars - 1) * sizeof(wchar_t);
	int len = read(buf,maxlen);
	len /= sizeof(wchar_t);
	buf[len] = 0;
	return(len);
}
int FormatfStreamBase::sprintf(char *buf,int maxchars)
// a self limiting sprintf that limits output size to maxlen.
// it can be called multiple times to get all the data if a length of 0 is passed
// in it will insert a null anyway;  Will never return maxlen only maximum of maxlen - 1
{
	int len = read(buf,maxchars-1);
	buf[len] = 0;
	return(len);
}

int
FormatfStreamBase::sprintf(char *out, int maxchars, const char *fmt, ... )
{
	va_list args;
	va_start(args,fmt);
	va_init(fmt,args);
	int ret = sprintf(out,maxchars);
	va_end(args);
	return(ret);
}
int
FormatfStreamBase::sprintf(wchar_t *out, int maxchars, const char *fmt, ... )
{
	va_list args;
	va_start(args,fmt);
	va_init(fmt,args);
	int ret = sprintf(out,maxchars);
	va_end(args);
	return(ret);
}
int
FormatfStreamBase::sprintf(wchar_t *out, int maxchars, const wchar_t *fmt, ... )
{
	va_list args;
	va_start(args,fmt);
	va_init(fmt,args);
	int ret = sprintf(out,maxchars);
	va_end(args);
	return(ret);
}

ARTD_END

