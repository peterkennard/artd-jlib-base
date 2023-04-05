#define __artd_BaseTypes_cpp

#include "artd/base_types.h"
#include "artd/RcString.h"
#include <math.h>

ARTD_BEGIN

RcString
Integer::toHexString(int val) {
	return(RcString::format("%x", val));
}

RcString
Long::toHexString(long val) {
	return(RcString::format("%lx", val));
}

const double Double::NaN = NAN;

ARTD_END // __artd_BaseTypes_h


