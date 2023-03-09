#ifndef __artd_platform_base_h
#define __artd_platform_base_h

// macro to glue two preprocessor symbols together
// 'ARTD_JOIN(x,y)' evaluates x, evaluates y, glues the results together, then evaluates that
// 'evaluate' means replace with what the symbol is defined to, if defined, otherwise the symbol remains unmodified
#define ARTD_JOIN(X,Y) ARTD_DO_JOIN(X,Y)
#define ARTD_DO_JOIN(X,Y) ARTD_DO_JOIN2(X,Y)
#define ARTD_DO_JOIN2(X,Y) X##Y

// macro to turn a preprocessor symbols into a string literal
// ARTD_STRINGIZE(x) evaluates x, and returns the result as a string literal
#define ARTD_STRINGIZE(X) ARTD_DO_STRINGIZE(X)
#define ARTD_DO_STRINGIZE(X) #X

#ifdef ARTD_NO_NAMESPACE
	#define artd
	#define ARTD_BEGIN
	#define ARTD_END
	#define USING_ARTD
#else
	/** @brief namespace definition so can be used optionally */
	#define ARTD_BEGIN namespace artd {
	#define ARTD_END }
	#define USING_ARTD using namespace artd;
#endif

// define exports for dynamic libraries
#ifdef _MSC_VER
	#define ARTD_SHARED_LIBRARY_EXPORT __declspec(dllexport)
	#define ARTD_SHARED_LIBRARY_IMPORT __declspec(dllimport)
#else // __APPLE__ and UNIX
	#define ARTD_SHARED_LIBRARY_EXPORT __attribute ((visibility ("default")))
	#define ARTD_SHARED_LIBRARY_IMPORT
#endif

// macro to encourage the compiler to always inline a function
// If used instead of the 'inline' keyword, the function will be inlined always, if
// possible, even in unoptimized or debug builds.
#ifdef __GNUC__
	#if __GNUC__ > 3
		#define ARTD_ALWAYS_INLINE inline __attribute__ ((always_inline))
	#else
		#define ARTD_ALWAYS_INLINE inline
	#endif
#elif defined(_MSC_VER)
	#define ARTD_ALWAYS_INLINE __forceinline
#else
	#define ARTD_ALWAYS_INLINE inline
#endif


// Some base level handy items that are often needed.
ARTD_BEGIN

/**
 * Because later versions of C++ give warnings or errors when casting null to a reference
 * This is used because one can pass a null pointer in to amethod and it can then
 * pass it into using *ptrArg  which will can set a reference to null.
 * @tparam T
 * @return
 */

template<class T>
ARTD_ALWAYS_INLINE T &null_ref() {
#if defined(_MSC_VER)
    return(*reinterpret_cast<T*>(nullptr));
#else
    T *p = nullptr;
    return(*p);
#endif
}

/**
 * Because later versions of C++ give warnings when comparing null to a reference
 * This is used because one can pass a null pointer in to amethod and it can then
 * pass it into using *ptrArg  which will can set a reference to null.
 * @tparam T
 * @return
 */
template<class T>
ARTD_ALWAYS_INLINE bool isNull(const T& r) {
    const T *p = &r;
    return(p == nullptr);
}

/**
 *  Delete object if non null, and set to null
 */
template<class T>
void deleteZ(T *&p)
{
	if (p)
	{
		delete(p);
		p = 0;
	}
}

ARTD_END


#endif // __artd_platform_base_h
