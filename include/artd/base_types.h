#ifndef __artd_base_types_h
#define __artd_base_types_h

#include "artd/jlib_base.h"
#include "artd/ObjectBase.h"

#if defined(_MSC_VER)
    #include <bit>
    #include <bitset>
#endif

ARTD_BEGIN

class Boolean
	: public ObjectBase
{
};

#define INL ARTD_ALWAYS_INLINE

class Number
	: public ObjectBase
{

	friend class PrimFormatter;

protected:

#if 0
	static int numberOfLeadingOnes(const void *start, int size)
	{
        if(std::endian::native == std::endian::little)
        {
            // little endian only
            const uint8_t *p = ((const uint8_t *)start) + size;
            uint8_t c;
            int bits = 0;
            while (p-- > start) {
                c = *p;
                if (c == 0xFF) {
                    bits += 8;
                    continue;
                }
                while (c & 0x80) {
                    ++bits;
                    c <<= 1;
                }
                break;
            }
		    return(bits);
		} else {


		}
	}
	static int numberOfLeadingZeros(const void *start, int size)
	{
        if (std::endian::native == std::endian::little)
        {
            const uint8_t *p = ((const uint8_t *)start) + size;
            uint8_t c;
            int bits = 0;
            while (p-- > start) {
                c = *p;
                if (c == 0) {
                    bits += 8;
                    continue;
                }
                c = ~c;
                while (c & 0x80) {
                    ++bits;
                    c <<= 1;
                }
                break;
            }
		    return(bits);
		} else {
		}
	}
#endif

public:

    INL static int numberOfLeadingZeros(int64_t val) {
        return(std::countl_zero((uint64_t)val));
    }
    INL static int numberOfLeadingZeros(uint64_t val) {
        return(std::countl_zero(val));
    }
    INL static int numberOfLeadingOnes(int64_t val) {
        return(std::countl_one((uint64_t)val));
    }
    INL static int numberOfLeadingOnes(uint64_t val) {
        return(std::countl_one(val));
    }


    INL static int numberOfLeadingZeros(uint32_t val) {
        return(std::countl_zero(val));
    }
    INL static int numberOfLeadingZeros(int32_t val) {
        return(std::countl_zero((uint32_t)val));
    }
    INL static int numberOfLeadingOnes(uint32_t val) {
        return(std::countl_one(val));
    }
    INL static int numberOfLeadingOnes(int32_t val) {
        return(std::countl_one((uint32_t)val));
    }

    INL static int numberOfLeadingZeros(uint16_t val) {
        return(std::countl_zero(val));
    }
    INL static int numberOfLeadingZeros(int16_t val) {
        return(std::countl_zero((uint16_t)val));
    }
    INL static int numberOfLeadingOnes(uint16_t val) {
        return(std::countl_one(val));
    }
    INL static int numberOfLeadingOnes(int16_t val) {
        return(std::countl_one((uint16_t)val));
    }
};


class Short
	: public Number
{
public:
	static const int16_t MAX_VALUE = (int)(((uint16_t)~0) >> 1);
	static const int16_t MIN_VALUE = (int)~(((uint16_t)~0) >> 1);
	static const int SIZE = 16;

    /**
      * Returns -1 or 1 depending on sign of operand
      */
    INL static int32_t signum(int16_t i) {
        return((i >> (SIZE - 1)) | (((uint32_t)(-i)) >> (SIZE - 1)));
    }

};

class Integer
	: public Number
{
public:
	static const int32_t MAX_VALUE = (int)(((uint32_t)~0) >> 1);
	static const int32_t MIN_VALUE = (int)~(((uint32_t)~0) >> 1);
	static const int SIZE = 32;

   /**
     * Returns -1 or 1 depending on sign of operand
     */
    INL static int lowestOneBit(int i) {
        return(i & -i);
    }

    /**
      * Returns -1 or 1 depending on sign of operand
      */
    static int32_t signum(int32_t i) {
        return((i >> (SIZE - 1)) | (((uint32_t)(-i)) >> (SIZE - 1)));
    }

	static ARTD_API_JLIB_BASE RcString toHexString(int val);
};

class Long
	: public Number
{
public:

	static const int64_t MAX_VALUE = (int64_t)(((uint64_t)~0) >> 1);
	static const int64_t MIN_VALUE = (int64_t)~(((uint64_t)~0) >> 1);
	static const int SIZE = 64;

   /**
     * Returns -1 or 1 depending on sign of operand
     */
    INL static int32_t signum(int64_t i) {
        return((i >> (SIZE-1)) | (((uint64_t)(-i)) >> (SIZE-1)));
    }

	static ARTD_API_JLIB_BASE RcString toHexString(long val);
};

class Float 
	: public Number
{
};

class Double 
	: public Number
{
public:
	static ARTD_API_JLIB_BASE const double NaN;
};

#undef INL

ARTD_END

#endif // __artd_BaseTypes_h


