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

#ifndef __artd_StringUtil_h__
#define __artd_StringUtil_h__

#include "artd/jlib_base.h"
#include "artd/int_types.h"
#include "artd/os_string.h"
  
#ifdef ARTD_WINDOWS
ARTD_ALWAYS_INLINE int strcasecmp(const char *a, const char *b) { return(::_strcmpi(a,b)); }
#endif

ARTD_BEGIN

/** replace instances of one character with instances of another 
 * character in a string
 */
ARTD_API_JLIB_BASE void strrep(char *str, char src, char dest);

ARTD_API_JLIB_BASE int wideCharToMultiByte(const wchar_t *src, char *dest, int len, bool utf8);
ARTD_API_JLIB_BASE int multiByteToWideChar(const char *src, wchar_t *dest, int len, bool utf8);

// these return the length of resultant string
ARTD_API_JLIB_BASE int itostr(int num,wchar_t *out,int radix);
ARTD_API_JLIB_BASE int itostr(int num,char *out,int radix);
ARTD_API_JLIB_BASE int i64tostr(int64_t num,wchar_t *out,int radix);
ARTD_API_JLIB_BASE int i64tostr(int64_t num,char *out,int radix);
ARTD_API_JLIB_BASE int ultostr(unsigned long num,wchar_t *out,int radix);
ARTD_API_JLIB_BASE int ultostr(unsigned long num,char *out,int radix);
ARTD_API_JLIB_BASE int ul64tostr(uint64_t num,wchar_t *out,int radix);
ARTD_API_JLIB_BASE int ul64tostr(uint64_t num,char *out,int radix);
ARTD_API_JLIB_BASE int sizettostr(size_t num,wchar_t *out,int radix);
ARTD_API_JLIB_BASE int sizettostr(size_t num,char *out,int radix);

inline int strtoi(const char *in) { return(::atoi(in));}
ARTD_API_JLIB_BASE int strtoi(const wchar_t *str);

ARTD_API_JLIB_BASE int strtoi(const char *in, int len);
ARTD_API_JLIB_BASE int strtoi(const wchar_t *in, int len);

ARTD_API_JLIB_BASE void reverseBytes(void *start,void *end);

ARTD_END


#endif // __artd_StringUtil_h__
