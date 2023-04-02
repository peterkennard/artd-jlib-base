#include "artd/HexFormatter.h"


ARTD_BEGIN


static const char map16uc_[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
// static const char map16lc_[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };


size_t 
HexFormatter::hexToBin(void *bin,const char *hex,unsigned int hexLen)
{
unsigned char *pbits = (unsigned char *)bin;
unsigned char *maxbits = pbits + (hexLen >> 1);
bool highnib = true;

	// TODO: this could be faster !! Pk.

	for(;;)
	{
	unsigned char val;

		char c = *hex++;
		if(c == 0)
			break;

		if(c >= '0' && c <= '9')
			val = c - '0';
		else if(c <= 'f' && c >= 'a')
			val = c - ('f' - 0x0f); 
		else if(c <= 'F' && c >= 'A')
			val = c - ('F' - 0x0f); 
		else    
			break;

		if(highnib)
		{
			*pbits = val;
			highnib = false;
			continue;
		}
		
		highnib = true;
		*pbits |= val;
		++pbits;    
		if(pbits == maxbits)
			break;
	}
	return(pbits - (unsigned char *)bin);
}

size_t
HexFormatter::binToHex(char *pout, const void * bytes, int length) {
	char *out = pout; 
	const unsigned char *pbytes = (const unsigned char *)bytes;
	
	for(const unsigned char *maxIn = pbytes + length; pbytes < maxIn;) {
		unsigned char b = *pbytes++;
		*out++ = (char)map16uc_[(b >> 4) & 0x0F];
		*out++ = (char)map16uc_[b & 0x0F];
	}
	return(out - pout);
}

RcString
HexFormatter::binToHex(const void *bytes, int length) {
	RcString rc = RcString::createForSize(length * 2);
	binToHex(rc->chars(),bytes,length);
	return(rc);
}


ARTD_END