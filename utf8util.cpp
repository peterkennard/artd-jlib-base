#include "artd/utf8util.h"

ARTD_BEGIN

int
Utf8::decodedSize(const char *src, int len)
{
	int size = 0;
	for(;;)
	{
		int c = *src;
		int sz = utfInSize(c);
		if(len < 0) {
			for(int i = sz;;)
			{
				if(!c) {
					goto done;
				}
				++src;
				if(--i <= 0) {
					break;
				}
				c = *src;
			}
		} else {
			src += sz;
			if((len -= sz) <= 0) {
				if(len == 0) {
					size += sz;
				}
				break;
			}
		}
		size += sz;
	}
done:
	return(size);
}

/**
 * decode utf8 string into wchar_t buffer
 */
const unsigned char *
Utf8::decode(wchar_t *out, const unsigned char *src, unsigned int charLen)
{
	const unsigned char *p = (const unsigned char *)src;
	const wchar_t *max = out + charLen;
	while(out < max)
	{
		unsigned int c = *p++;
		if(!(c & 0x80)) {
			goto doneWithChar;
		}
		if(!(c & 0x40)) {
			c &= ~0xC0;
			goto add1;
		}
		if(!(c & 0x20)) {
			c &= ~0xE0;
			goto add2;
		}
		if(!(c & 0x10)) {
			c &= ~0xF0;
			goto add3;
		}
		if(!(c & 0x08)) {
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
		c |= (*p++ & 0x3f);
	doneWithChar:
		*out++ = c;
	}
	return(p);
}

int
Utf8::encode(char *out, int maxout, const wchar_t **pin, int inlen )
{
	const wchar_t *in = *pin;

	char *p = out;
	char *omax = p + (maxout - 5);
	wchar_t c = 0;
	while(p < omax) {
		c = *in;
		if(inlen < 0) {
			if(!c) {
				goto done;
			}
		} else if (--inlen <= 0) {
			goto done;
		}
		p = encode1(p,c);
		++in;
	}

	int olen;
	olen = utfOutSize(c);
	for(;;) {
		if(olen > 4) {
			goto done;
		}
		p = encode1(p,c);
		if(inlen < 0) {
			if(!c) {
				goto done;
			}
		} else if (--inlen <= 0) {
			goto done;
		}
		c = *(++in);
		olen += utfOutSize(c);
	}

done:
	*pin = in;
	*p = 0;
	return((int)(p - out));
}


ARTD_END
