#ifndef __artd_HexFormmater_h
#define __artd_HexFormmater_h

#include "artd/jlib_base.h"
#include "artd/RcString.h"

ARTD_BEGIN

class HexFormatter {
	HexFormatter() {}
public:

	static ARTD_ALWAYS_INLINE int charValue(int c)  {
		if(c >= '0' && c <= '9')
			c -= '0';
		else if(c <= 'f' && c >= 'a')
			c -= ('f' - 0x0f); 
		else if(c <= 'F' && c >= 'A')
			c -= ('F' - 0x0f); 
		else    
			c = -1;
		return(c);
	}

    static ARTD_API_JLIB_BASE size_t binToHex(char *hex, const void *bin, int binLen);
    static ARTD_API_JLIB_BASE RcString binToHex(const void *bin,int binLen);
	static ARTD_API_JLIB_BASE size_t hexToBin(void *bin,const char *hex,unsigned int hexLen = ~0);
};


ARTD_END

#endif // __artd_HexFormmater_h

