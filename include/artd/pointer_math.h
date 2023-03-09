#ifndef __artd_pointer_math_h
#define __artd_pointer_math_h

// ARTD_HEADER_CREATED: June 1989

#include "artd/jlib_base.h"
#include <new> // for size_t ptrdiff_t and new(void *) in Windows
#include "artd/int_types.h"

ARTD_BEGIN

// ***** common pointer offset math *****

// offset a pointer byte address wise
#define ARTD_OPTR(base,offset) ((void*)((char*)((void *)base)+(offset)))

// gives size of memory in bytes given pointer to start and one byte beyond end
#define ARTD_SIZE(beg,end) \
	((char*)((void *)(end))-(char*)((void *)(beg)))

// gives inclusive size between two fields in a structure
#define INCLUSIVE_SIZE(f0,f1) ARTD_SIZE(&(f0),&(&(f1))[1])

#define ARTD_ARRAY_SIZE(a) \
	((sizeof(a) / sizeof(*(a))) / \
	static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

/** @brief align size value up to (power of 2) boundary, leave unchanged if already aligned */
template<class T> 
ARTD_ALWAYS_INLINE T ARTD_ALIGN_UP(T size, int boundary)
{
	return((size+(boundary-1)) & (~(boundary-1)));
}

/** @brief align size value down to (power of 2) boundary, leave unchanged if already aligned */
template<class T> 
ARTD_ALWAYS_INLINE T ARTD_ALIGN_DOWN(T size, int boundary)
{
	return(size&(~(boundary-1)));
}

/** @brief align pointer up to (power of 2) boundary, leave unchanged if already aligned */
#define ARTD_ALIGN(p,boundary) ARTD_OPTR(p,ARTD_ALIGN_UP((ptrdiff_t)p,boundary)


// offset generating macros.  Note: these will not always work with classes
// that have any non public members and/or base classes if the compilers do
// things like put different parts of objects in different memory areas.

// returns syntax reference to a field within a given struct name;
// useful for sizeof(ARTD_MEMBER(s,f)) or sizeof(*ARTD_MEMBER(s,f))

#define ARTD_MEMBER(Class,Field) \
	((Class*)(void *)8)->Field

// return pointer to class given pointer to field in class
#define ArtdToClass(Class,FieldName,Fieldptr) \
	((Class *)ARTD_OPTR(Fieldptr,-(ptrdiff_t)ARTD_OFFSET_OF(Class,FieldName)))

template<class T>
inline T *addOffset(T *pt, int bytes) {
	return((T *)ARTD_OPTR(pt, bytes));
}

template<class T>
inline T &offsetCast(void *pt, int bytes) {
	return(*(T *)ARTD_OPTR(pt, bytes));
}


template<class BaseT, class SuperT>
constexpr size_t base_offset() {
	return(((size_t)(static_cast<BaseT*>(reinterpret_cast<SuperT*>((void *)8)))-8));
}

template<class ToBase, class Root, class ThisBase>
inline size_t relative_offset() {
	return( ((size_t)(static_cast<ToBase*>((Root*)8))-8)
			 -((size_t)(static_cast<ThisBase*>((Root*)8))-8) );
}

// Cast from one base to another base in the same Root class.
// Good for templates and Com "queryInterface" building.

template<class ToBase, class Root, class ThisBase>
inline ToBase *relative_cast(ThisBase *p)
{
	const size_t oset = ((size_t)(static_cast<ToBase*>((Root*)8))-8)
			-((size_t)(static_cast<ThisBase*>((Root*)8))-8);

	return(reinterpret_cast<ToBase *>(reinterpret_cast<char *>(p)+oset));
}

// Returns pointer to Root object from a pointer
// to a base.

template<class Root, class Base> inline
Root *root_cast(Base *p)
{
	const ptrdiff_t offset = ((ptrdiff_t)(static_cast<Base*>((Root*)8))-8);
	return(reinterpret_cast<Root *>(reinterpret_cast<char *>(p)-offset));
}

ARTD_END


#endif // __artd_pointer_math_h
