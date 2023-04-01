#ifndef ARTD_ASSERT
#include "artd/jlib_base.h"
#include <cassert>

#define ARTD_ASSERT(xx) assert(xx)

ARTD_BEGIN

// ARTD_AssertDialog is defined in artd_assert.cpp
ARTD_API_JLIB_BASE void ARTD_AssertDialog(const char* msg, const char* file, int line);

ARTD_END


#endif // ARTD_ASSERT