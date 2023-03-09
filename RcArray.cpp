/*-
 * Copyright (c) 1998-2011 Peter Kennard and aRt&D Lab
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of the source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Any redistribution solely in binary form must conspicuously
 *    reproduce the following disclaimer in documentation provided with the
 *    binary redistribution.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'', WITHOUT ANY WARRANTIES, EXPRESS
 * OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  LICENSOR SHALL
 * NOT BE LIABLE FOR ANY LOSS OR DAMAGES RESULTING FROM THE USE OF THIS
 * SOFTWARE, EITHER ALONE OR IN COMBINATION WITH ANY OTHER SOFTWARE.
 *
 * 	$Id$
 */

//#include "artd/platform_specific.h"
#include "artd/RcArray.h"
#include "stdlib.h"
#include <string>



ARTD_BEGIN

RcArrayBase::RcArrayBase(void * allocatedAt, int len)
	: ObjectBase((ObjectBase::CBlock *)allocatedAt), len_(len)
{
}
RcArrayBase::~RcArrayBase() {
}

#if 0
ObjectPtr<RcArrayBase>
RcArrayBase::allocate(int numElems, int elemsize, bool clear)
{
	size_t size = sizeForData(numElems * elemsize);
	return(nullptr);
}
#endif

ObjectPtr<RcArrayBase>
RcArrayBase::allocate(int numElems, int elemsize)
{
	size_t size = sizeForData(numElems * elemsize);

	ObjAllocatorArg allocArg(size - sizeof(int));
	std::shared_ptr<int> sptr = std::allocate_shared<int>(ObjectAllocator<int>());

	void* me = sptr.get();
	RcArrayBase* obj = ::new(me) RcArrayBase(allocArg.allocatedAt, numElems);

	return(*reinterpret_cast<ObjectPtr<RcArrayBase>*>((void*)&sptr));
}

#if 0



class RcArrayImpl
	: public RcArrayOwnedObjectBase
{
public:

	RcArrayImpl(int len, int elemsize, int refs)
		: RcArrayOwnedObjectBase(len,refs)
	{
	}
};

// static RcArrayImpl refbase(1,1,__static_refcount_init);


RcArrayOwnedObjectBase *
RcArrayOwnedObjectBase::constructInstance(void *buf,int count, int elemsize,RefScope scope)
{
	memset(buf,0,sizeForElements(count,elemsize));
	return(new(buf) RcArrayImpl(count,elemsize,__scope_init(scope)));
}

/*
RcArrayOwnedObjectBase *
RcArrayOwnedObjectBase::getStaticInstance(void *buf,int count, int elemsize)
{
	if( (*(ptrdiff_t *)buf) != (*(ptrdiff_t *)(void *)&refbase)) {
		new(buf) RcArrayImpl(count,elemsize,__static_refcount_init);
	}
	return((RcArrayOwnedObjectBase *)buf);
}
*/

bool
RcArrayOwnedObjectBase::isAllocated()
{
	// TODO: a cheezy test in case somehow some allocated object gets > staticRefs0
	// but you'd have to have a gig of references to it.
	return(_unsafeGetRefCount_() < (__static_refcount_init / 4));
}
RcArrayOwnedObjectBase *
RcArrayOwnedObjectBase::ensureAllocated(int elemsize)
{
	if(isAllocated()) {
		return(this);
	}
	return(RcArrayOwnedObjectBase::allocate(len_,elemsize,data()));
}


int
RcArrayOwnedObjectBase::equals(const RcArrayOwnedObjectBase *a, const RcArrayOwnedObjectBase *b)
{
	if(!a || !b) {
		return(false);
	}
	if(a->len_ != b->len_) {
		return(false);
	}
	if(a == b) {
		return(true);
	}
	return(::memcmp(a->data(),b->data(),b->len_) == 0);
}

#endif // 0

ARTD_END

