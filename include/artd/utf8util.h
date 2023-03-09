/*-
 * Copyright (c) 1998-2022 Peter Kennard and aRt&D Lab
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
#ifndef __artd_utf8util_h
#define __artd_utf8util_h

#include "artd/jlib_base.h"

ARTD_BEGIN

class ARTD_API_JLIB_BASE Utf8
{
public:

	static char *encode1(char *buf, unsigned int c)
	{
		if (c < 0x80) {
			*buf++ = (c);
			goto done;
		}
		if (c < 0x800) {
			*buf++ = (0xC0 | (c>>6));
			goto add1;
		}
		if (c < 0x10000) {
			*buf++ = (0xE0 | (c>>12));
			goto add2;
		}
		if (c < 0x200000) {
			*buf++ = (0xF0 | (c>>18));
			goto add3;
		}
		if (c < 0x4000000) {
			*buf++ = (0xF8 | (c>>24));
			goto add4;		
		}
		if(c < 0x80000000) {
			*buf++ = (0xFC | (c>>30));
		}	
		*buf++ = 0x80 | ((c>>24) & 0x3F);
	add4:
		*buf++ = 0x80 | ((c>>18) & 0x3F);
	add3:
		*buf++ = 0x80 | ((c>>12) & 0x3F);
	add2:
		*buf++ = 0x80 | ((c>>6) & 0x3F);
	add1:
		*buf++ = 0x80 | (c & 0x3F);	
	done:
		return(buf);
	}
	static unsigned int decode1(const char **src)
	{
		const unsigned char *p = *(const unsigned char **)src;

		unsigned int c = *p++;
		if(!(c & 0x80)) {
			goto doneWithChar;
		}
		if(!(c & 0x20)) {
			c &= ~0xC0;
			goto add1;	
		}
		if(!(c & 0x10)) {
			c &= ~0xE0;
			goto add2;	
		}
		if(!(c & 0x08)) {
			c &= ~0xF0;
			goto add3;	
		}
		if(!(c & 0x04)) {
			c &= ~0xF8;
			goto add4;	
		}
		c <<= 6;
		c |= *p++ & 0x3f;
	add4:
		c <<= 6;
		c |= *p++ & 0x3f;
	add3:
		c <<= 6;
		c |= *p++ & 0x3f;
	add2:
		c <<= 6;
		c |= *p++ & 0x3f;
	add1:
		c <<= 6;
		c |= *p++ & 0x3f;
	doneWithChar:
		*src = (const char *)p;
		return(c);
	}

	static int utfOutSize(unsigned int c)
	{
		// no not binary search because smaller
		// chars are more frequent.
		if (c < 0x80) {
			return(1);
		}
		else if (c < 0x800) {
			return(2);
		}
		else if (c < 0x10000) {
			return(3);
		}
		else if (c < 0x200000) {
			return(4);
		}
		else if (c < 0x4000000) {
			return(5);
		}
		else if(c < 0x80000000) {
			return(6);
		}
		return(-1); // overflow
	}
	static const int MaxCharBytes = 6;

	static int utfInSize(int utfc)
	{
		if(!(utfc & 0x80))
			return(1);
		if(!(utfc & 0x20))
			return(2);
		if(!(utfc & 0x10))
			return(3);
		if(!(utfc & 0x08))
			return(4);
		if(!(utfc & 0x04))
			return(5);
		return(6);
	}
	/**
	 * @brief converts from wchar_t buffer to utf8 buffer
	 * @param out the output buffer
	 * @param maxout available size for output in buffer 
	 * including any terminal null (0) 
	 * @param psrc the next character to convert in the input
	 *     when complete *psrc is assigned to the next input
	 *     character in the buffer.
	 * @param srclen if srclen is < 0 it checks for null 
	 * termination otherwise not and encode at most srclen
	 * chars and will write incomming nulls to the output.
	 * assignes the pointer to the next input char to be encoded
	 * into *psrc.
	 * @return number of bytes added to output utf8 buffer
	 */
	static int encode(char *out, int maxout, const wchar_t **psrc, int srclen=-1 );

	static int decodedSize(const char *src, int len=-1);
    ARTD_ALWAYS_INLINE static int decodedSize(const unsigned char *src, int len=-1) {
        return(decodedSize((const char *)src,len));
    }

    /**
	 * @brief decode utf8 buffer into wchar_t buffer
	 * @param out buffer to receive decoded chars
	 * @param psrc pointer to source bytes
	 * @param size count of characters to decode
	 * @return the pointer to the next undecoded source byte
	 */
    static const unsigned char *decode(wchar_t *out, const unsigned char *psrc, unsigned int size);
    ARTD_ALWAYS_INLINE static const char *decode(wchar_t *out, const char *psrc, unsigned int size) {
        return((char *)decode(out, (const unsigned char *)psrc, size));
    }

	// the below were used to encode integers to send to Flash
	// to use their XML socket for binary values.
	static char *addUint7(char *buf,unsigned int val)
	{
		val += 1;
		*buf++ = (val & 0x7F);
		return(buf);
	}
	static char *addInt16(char *buf,int val)
	{
		val += 0x07FFF;
		val &= 0x0FFFF;
		return(encode1(buf,val));
	}
	static char *addUint16(char *buf,unsigned int val)
	{
		// note: the maximum value 
		// representable is 0x0FFFE
		val += 1;
		return(encode1(buf,val & 0x0FFFF));
	}
	static char *addInt20(char *buf,int val)
	{
		val += 0x07FFFF;
		val &= 0x0FFFFF;
		return(encode1(buf,val));
	}
	static char *addUint20(char *buf,unsigned int val)
	{
		// note: the maximum value 
		// representable is 0x0FFFFE
		val += 1;
		return(encode1(buf,val & 0x0FFFFF));
	}
};

ARTD_END


#endif // __artd_utf8util_h
