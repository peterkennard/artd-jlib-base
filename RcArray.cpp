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

RcArrayBase::RcArrayBase(int len)
	: len_(len)
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
	RcArrayBase* obj = ::new(me) RcArrayBase(numElems);

	return(*reinterpret_cast<ObjectPtr<RcArrayBase>*>((void*)&sptr));
}

#if 0

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

