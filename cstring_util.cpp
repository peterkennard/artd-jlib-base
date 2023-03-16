/*-
 * Copyright (c) 1991-2011 Peter Kennard and aRt&D Lab
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
 *
 */

#include "artd/cstring_util.h"
#include "artd/int_types.h"
#include "artd/platform_base.h"
#include "artd/static_assert.h"

// #include "artd/platform_specific.h"
#ifdef ARTD_WINDOWS
	#include "Windows.h"
#endif


ARTD_BEGIN

void strrep(char *str,char src,char dest)
{
char c;

	while((c = *str) != 0)
	{
		if(c == src)
			*str = dest;
		++str;
	}
}

int 
wideCharToMultiByte(const wchar_t *src, char *dest, int len, bool utf8)
{
	#ifdef ARTD_WINDOWS
		int clen = ::WideCharToMultiByte(utf8 ? CP_UTF8 : CP_ACP,0,src,-1,dest,len,NULL,NULL);
	#else
		// TODO: Proper handling of the utf8 parameter
		int clen = wcstombs(dest, src, len) + 1;
	#endif
	return clen;
}

int 
multiByteToWideChar(const char *src, wchar_t *dest, int len, bool utf8)
{
	#ifdef ARTD_WINDOWS
		int clen = ::MultiByteToWideChar(utf8 ? CP_UTF8 : CP_ACP,0,src,-1,dest,len);
	#else
		// TODO: Proper handling of the utf8 parameter
		int clen = mbstowcs(dest, src, len) + 1;
	#endif
	return clen;
}

size_t binToHex(char *hex,const void *bin,int binLen)
{
char *out = hex;
const unsigned char *pbits = (unsigned char *)bin;
const unsigned char *maxbits = pbits + binLen;

static const char chars[] = "0123456789abcdef";

	while(pbits < maxbits)
	{
	unsigned char bits = *pbits++;

		*out++ = chars[bits >> 4];
		*out++ = chars[bits & 0x0F];
	}
	*out = 0;
	return(out - hex);

}
void reverseBytes(uint8_t *start,uint8_t *end)
{
	// makes assumption that "end" is placed one beyond last byte
	--end;
	while(end > start)
	{
		uint8_t swapper = *start;
		*start++ = *end;
		*end-- = swapper;
	}
}
static int intToText(bool isNegative,unsigned int val,char *buf,unsigned int radix)
{
char *out;  
char *first;

	out = buf;

	if(isNegative) // add sign and negate it if requested
	{
		*out++ = '-';
		val = (unsigned int)(-(int)val);
	}

	first = out;

	for(;;)
	{
		unsigned int digit = (unsigned int)(val % radix);
		val /= radix; // next digit

		if (digit > 9)
			*out++ = digit + ('a' - 10); // hex representation
		else
			*out++ = (digit + '0');        // normal digit

		if(val <= 0) // none left of value
			break;
	} 

	*out = 0; // terminate end of buffer
	int len = (int)(out - first);
	--out;

	// reverse buffer since it was built in reverse
	
	for(;;)
	{
		char swaper = *out;
		*out = *first;
		*first = swaper;  // swap chars;
		--out;
		if(++first >= out)  // done at middle
			break;
	}
	return(len);
}

static int int64ToText(bool isNegative,uint64_t val,char *buf,uint64_t radix)
{
char *out;  
char *first;

	out = buf;

	if(isNegative) // add sign and negate it if requested
	{
		*out++ = '-';
		val = (uint64_t)(-(int64_t)val);
	}

	first = out;

	for(;;)
	{
		uint64_t digit = (uint64_t)(val % radix);
		val /= radix; // next digit

		if (digit > 9)
			*out++ = (char) (digit + ('a' - 10)); // hex representation
		else
			*out++ = (char) (digit + '0');        // normal digit

		if(val <= 0) // none left of value
			break;
	} 

	*out = 0; // terminate end of buffer
	int len = (int)(out - first);
	--out;

	// reverse buffer since it was built in reverse
	
	for(;;)
	{
		char swaper = *out;
		*out = *first;
		*first = swaper;  // swap chars;
		--out;
		if(++first >= out)  // done at middle
			break;
	}
	return(len);
}

static int intToText(bool isNegative,unsigned int val,wchar_t *buf,unsigned int radix)
{
wchar_t *out;  
wchar_t *first;

	out = buf;

	if(isNegative) // add sign and negate it if requested
	{
		*out++ = '-';
		val = (unsigned int)(-(int)val);
	}

	first = out;

	for(;;)
	{
		unsigned int digit = (unsigned int)(val % radix);
		val /= radix; // next digit

		if (digit > 9)
			*out++ = digit + ('a' - 10); // hex representation
		else
			*out++ = (digit + '0');        // normal digit

		if(val <= 0) // none left of value
			break;
	} 

	*out = 0; // terminate end of buffer
	int len = (int)(out - first);
	--out;

	// reverse buffer since it was built in reverse
	
	for(;;)
	{
		wchar_t swaper = *out;
		*out = *first;
		*first = swaper;  // swap chars;
		--out;
		if(++first >= out)  // done at middle
			break;
	} 
	return(len);
}

static int int64ToText(bool isNegative,uint64_t val,wchar_t *buf,uint64_t radix)
{
wchar_t *out;  
wchar_t *first;

	out = buf;

	if(isNegative) // add sign and negate it if requested
	{
		*out++ = '-';
		val = (uint64_t)(-(int64_t)val);
	}

	first = out;

	for(;;)
	{
		uint64_t digit = (uint64_t)(val % radix);
		val /= radix; // next digit

		if (digit > 9)
			*out++ = (wchar_t) (digit + ('a' - 10)); // hex representation
		else
			*out++ = (wchar_t) (digit + '0');        // normal digit

		if(val <= 0) // none left of value
			break;
	} 

	*out = 0; // terminate end of buffer
	int len = (int)(out - first);
	--out;

	// reverse buffer since it was built in reverse
	
	for(;;)
	{
		wchar_t swaper = *out;
		*out = *first;
		*first = swaper;  // swap chars;
		--out;
		if(++first >= out)  // done at middle
			break;
	} 
	return(len);
}

int itostr(int num,char *out,int radix)
{
	return(intToText((radix == 10) && (num < 0),(unsigned long)num,out, radix));
}

int ultostr(unsigned long num,char *out,int radix)
{
	#if defined(ARTD_WINDOWS) || defined(ARTD_32BIT)
		ARTD_STATIC_ASSERT(sizeof(unsigned long) == 4);
		return(intToText(false,num,out,radix));
	#else
		ARTD_STATIC_ASSERT(sizeof(unsigned long) == 8);
		return(int64ToText(false,num,out,radix));
	#endif
}

int sizettostr(size_t num,char *out,int radix)
{
	#if defined(ARTD_32BIT)
		ARTD_STATIC_ASSERT(sizeof(size_t) == 4);
		return(intToText(false,num,out,radix));
	#else
		ARTD_STATIC_ASSERT(sizeof(size_t) == 8);
		return(int64ToText(false,num,out,radix));
	#endif
}

int i64tostr(int64_t num,char *out,int radix)
{
	return(int64ToText((radix == 10) && (num < 0),(uint64_t)num,out, radix));
}
int ul64tostr(uint64_t num,char *out,int radix)
{
	return(int64ToText(false,num,out,radix));
}

int itostr(int num,wchar_t *out,int radix)
{
	return(intToText((radix == 10) && (num < 0),(unsigned long)num,out, radix));
}

int ultostr(unsigned long num,wchar_t *out,int radix)
{
	#if defined(ARTD_WINDOWS) || defined(ARTD_32BIT)
		ARTD_STATIC_ASSERT(sizeof(unsigned long) == 4);
		return(intToText(false,num,out,radix));
	#else
		ARTD_STATIC_ASSERT(sizeof(unsigned long) == 8);
		return(int64ToText(false,num,out,radix));
	#endif
}

int sizettostr(size_t num,wchar_t *out,int radix)
{
	#if defined(ARTD_32BIT)
		ARTD_STATIC_ASSERT(sizeof(size_t) == 4);
		return(intToText(false,num,out,radix));
	#else
		ARTD_STATIC_ASSERT(sizeof(size_t) == 8);
		return(int64ToText(false,num,out,radix));
	#endif
}

int i64tostr(int64_t num,wchar_t *out,int radix)
{
	return(int64ToText((radix == 10) && (num < 0),(uint64_t)num,out, radix));
}
int ul64tostr(uint64_t num,wchar_t *out,int radix)
{
	return(int64ToText(false,num,out,radix));
}

// bodge for now, assume decimal integer is no longer than 64
// digits (raltively safe)
int strtoi(const wchar_t *in, int len)
{
char buf[64];
int i;

	if(((unsigned int)len) > 63) {
		len = 63;
	}
	for(i = 0;i < len;++i) 
	{
		wchar_t uc = *in++;
		if((buf[i] = (char)uc) == 0) {
			goto atoiIt;
		}
	}
	buf[i] = 0;
atoiIt:
	return(::atoi(buf));
}
int strtoi(const wchar_t *in)
{
	return(strtoi(in,63));
}
// total bodge using null termination
int strtoi(const char *in, int len)
{
	char *str = const_cast<char *>(in);
	int ochar = in[len];
	str[len] = 0;
	int ret = ::atoi(in);
	str[len] = ochar;
	return(ret);
}

ARTD_END
