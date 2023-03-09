#ifndef __artd_int_types_h
#define __artd_int_types_h

// ARTD_HEADER_AUTHORS: Peter Kennard
// ARTD_HEADER_CREATOR: Peter Kennard
// ARTD_HEADER_CREATED: Mar 07 2009

#ifdef _MSC_VER

	#if _MSC_VER >= 1600
		#include <stdint.h>
	#else
		#ifndef uint8_t
			#define uint8_t uint8_t
			typedef unsigned __int8 uint8_t;
		#endif
		#ifndef int8_t
			#define int8_t int8_t
			typedef __int8 int8_t;
		#endif
		#ifndef uint16_t
			#define uint16_t uint16_t
			typedef unsigned __int16 uint16_t;
		#endif
		#ifndef int16_t
			#define  int16_t int16_t
			typedef __int16 int16_t;
		#endif
		#ifndef uint32_t
			#define uint32_t uint32_t
			typedef unsigned __int32 uint32_t;
		#endif
		#ifndef int32_t
			#define int32_t int32_t
			typedef __int32 int32_t;
		#endif
		#ifndef uint64_t
			#define uint64_t uint64_t
			typedef unsigned __int64 uint64_t;
		#endif
		#ifndef int64_t
			#define int64_t int64_t
			typedef __int64 int64_t;
		#endif
		#ifndef _WCHAR_T_DEFINED
			typedef unsigned short wchar_t;
			#define _WCHAR_T_DEFINED
		#endif
	#endif
	#define U_HAVE_INT8_T
	#define U_HAVE_UINT8_T
	#define U_HAVE_INT16_T
	#define U_HAVE_UINT16_T
	#define U_HAVE_INT32_T
	#define U_HAVE_UINT32_T
	#define U_HAVE_INT64_T
	#define U_HAVE_UINT64_T

#else

#include <stdint.h>

	#ifndef __STDC_CONSTANT_MACROS
		#define __STDC_CONSTANT_MACROS
	#endif
	#ifdef __FreeBSD__
		#if __FreeBSD__ < 5
			#include <inttypes.h>
		#else
			#include <stdint.h>
		#endif
	#endif

	#ifdef __linux__
		#include <stdint.h>
		#include <stddef.h>  // ptrdiff_t size_t wchar_t 
	#endif

#endif

#endif // __artd_int_types_h

