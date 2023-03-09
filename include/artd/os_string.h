#ifndef __artd_string_h
#define __artd_string_h

#include "artd/jlib_base.h"

#ifdef _MSC_VER
    #define ARTD_SUPPORT_STD_STRING 1
#endif

#include <string>

#ifdef __linux__
	#include <cstring> // for strlen
	#include <wchar.h> // for wcslen
#endif


#endif // __artd_string_h

