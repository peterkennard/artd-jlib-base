/*-
 * Copyright (c) 1998-2022 Peter Kennard and aRt&D Lab
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

#ifndef __artd_RcArray_h
#define __artd_RcArray_h

#include "artd/ObjectBase.h"
#include "artd/Logger.h"


ARTD_BEGIN


#pragma pack(push,4) // int alignment
#define INL ARTD_ALWAYS_INLINE

/**
 * Reference counted Byte Array object
 */
class RcArrayBase
	: public ObjectBase
{
private:
	friend class RcString;

protected:

    ARTD_API_JLIB_BASE RcArrayBase(int len);
    ARTD_API_JLIB_BASE virtual ~RcArrayBase() override;
public:

	/** length of buffer in number of elements */
	const int len_;

	INL static int offsetOfArray() { return(sizeof(RcArrayBase)); }
	INL const void *data() const { return(((char *)this)+sizeof(*this)); }
	INL void *data() { return(((char *)this)+sizeof(*this)); }

	INL int length() const { return(len_); }
	/** returns size of Array object in bytes for a specified number of bytes */
	INL static size_t sizeForData(size_t byteSize) { return(offsetOfArray() + byteSize); }
	INL static size_t sizeForElements(int count, int elemsize) { return(offsetOfArray() + (size_t)(elemsize * count)); }

//    static ARTD_API_JLIB_BASE ObjectPtr<RcArrayBase> allocate(int numElems, int elemsize, bool clearIt);
    static ARTD_API_JLIB_BASE ObjectPtr<RcArrayBase> allocate(int numElems, int elemSize);

#if 0
	static ARTD_API_JLIB_BASE RcArrayOwnedObjectBase *constructInstance(void *buf, int count, int elemsize, RefScope scope);
	//** creates new object with refcount of 1 and array length of count for elemsize

	static ARTD_API_JLIB_BASE RcArrayOwnedObjectBase *getStaticInstance(void *buf, int count, int elemsize);
	static ARTD_API_JLIB_BASE int equals(const RcArrayOwnedObjectBase *a, const RcArrayOwnedObjectBase *b);
	ARTD_API_JLIB_BASE bool isAllocated();

    ARTD_API_JLIB_BASE RcArrayOwnedObjectBase *ensureAllocated(int elemsize);
#endif

};


template <class ElemT>
class RcArray;


template <class ElemT>
class RcArrayObj
    : public RcArrayBase
{
    typedef RcArrayBase BaseT;
public:

    friend class RcArrayImpl;
    friend class RcArrayBase;

    static const int OffsetOfElements = sizeof(RcArrayBase);

    INL ElemT *elements() { return(reinterpret_cast<ElemT *>(BaseT::data())); }
    INL const ElemT *elements() const { return(reinterpret_cast<const ElemT *>(BaseT::data())); }

    INL size_t dataSize() const { return(BaseT::len_ * sizeof(ElemT)); }

    static ObjectPtr<RcArrayObj<ElemT>> createInstance(int count) {
        ObjectPtr<RcArrayBase> newOne = RcArrayBase::allocate(count, sizeof(ElemT));
        ElemT* pelem = reinterpret_cast<ElemT *>(newOne->data());

        // TODO: do nothiing here if it is a primitive or has no constructor ?
        const ElemT* maxElem = pelem + count;
        while (pelem < maxElem) {
            new(pelem) ElemT(); // default constructor
            ++pelem;
        }
        return(*reinterpret_cast<ObjectPtr<RcArrayObj<ElemT>> *>(&newOne));
    }

protected:
    // Can't create these directly as they vary in size
    RcArrayObj() {}
//    RcArrayObj(const RcArrayObj &r) {}
//    RcArrayObj(RcArrayObj &&r) {}
//    RcArrayObj(int len) : super(len) { }
};


template<class ElemT>
class PRcArray;


template<class ElemT>
class RcArray
    : public ObjectPtr<RcArrayObj<ElemT>>
{
    typedef ObjectPtr<RcArrayObj<ElemT>> super;
public:

    typedef RcArray<ElemT> NewType;

	friend class PRcArray<ElemT>;

	typedef RcArray<ElemT> ThisT;
	typedef RcArrayObj<ElemT> ObjT;


//	INL static ThisT make(int numElems) {
//        return(RcArrayBase::createInstance(numElems, sizeof(ElemT)));
//    }

    INL RcArray() {}
    INL RcArray(std::nullptr_t) : super() {}
    
    RcArray(int numElems) : super(ObjT::createInstance(numElems)) {
    }

//    RcArray(const ElemT *elems, int numElems) {
//        super::p_ = (RcArrayObj<ElemT>*)RcArrayBase::allocate(numElems, sizeof(ElemT),elems);
//    }

    INL RcArray(const super &r) : super(r) {}
    INL RcArray(const ThisT &r) : super(r) {}

    INL RcArray(super &&r) : super(std::move(r)) {}
    INL RcArray(ThisT &&r) : super(std::move((super&)r)) {}

    // assignment        
    INL ThisT& operator=(std::nullptr_t) { super::operator=(nullptr); return(*this); }
    INL ThisT &operator=(const ObjT *p) { super::operator=(p); return(*this); }
    INL ThisT &operator=(const super &r) { super::operator=(r); return(*this); }
    INL ThisT &operator=(const ThisT &r) { super::operator=(r); return(*this); }

    INL ThisT &operator=(super &&r) { super::operator=(std::move(r)); return(*this); }
    INL ThisT &operator=(ThisT &&r) { super::operator=(std::move((super&)r)); return(*this); }

	INL ElemT *elements() { return((ElemT *)(super::get()->data())); }
	INL const ElemT *elements() const { return((ElemT *)(super::get()->data())); }
    INL int length() const { return(super::get()->len_); }

    INL ElemT &operator[](int ix) { return(((ElemT *)(super::get()->data()))[ix]); }
    INL const ElemT &operator[](int ix) const { return(((ElemT *)(super::get()->data()))[ix]); }

    INL const void *data() const { return(super::get()->data()); }
    INL void  *data() { return(super::get()->data()); }
    INL size_t dataSize() { return(super::length() * sizeof(ElemT)); }

    INL int equals(const ThisT &b) const { return(super::equals(b)); }
};



#if 0

/**
 * template for initializing static or stack based RcArrays
 *
 * use it like this:
 *
 * RcArrayInstance<int,3> myArray = {{}, {1,2,3}};
 *
 * RcArray<int> rcarray = myArray;
 *
 */

template<class ElemT, const int nelems>
class RcArrayInstance
{
public:
	uint8_t _private_[RcArrayObj<ElemT>::OfsetOfElements];
	ElemT  values_[nelems];

	typedef TransferPtr< RcArrayObj<ElemT> > TransPtr;
	typedef PRcArray<ElemT> Ptr;

	ElemT &operator[](int ix) { return(values_[ix]); }

	operator Ptr()
		{
			RcArrayOwnedObjectBase *p = RcArrayOwnedObjectBase::getStaticInstance(this,nelems,sizeof(ElemT));
			return(*(Ptr*)&p);
		}

};

#endif

typedef RcArray<uint8_t> ByteArray;

#undef INL
#pragma pack(pop) // back to prior packing

ARTD_END

#endif // __artd_RcArray_h


