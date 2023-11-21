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
 *  $Id$
 */

#include "artd/ObjectBase.h"
#include "artd/Logger.h"
#include "artd/RcString.h"
#include <set>
#include <map>

#include <iostream>

ARTD_BEGIN

#define INL ARTD_ALWAYS_INLINE

std::map<ObjectBase *, std::string> remainingObjs;

//std::set<ObjectBase*> remainingObjs;

static bool trackOutstanding = false;
void ObjectBase::setKeepAllocatedRefs(bool on) {
    trackOutstanding = on;
}


uint8_t ObjectBase::initValues[4] = { 0,1,2,3 };

static size_t allocCount = 0;

const char* ObjectBase::getCppClassName() const
{
    if (std::abs((int64_t)((void*)this) - (int64_t)((void*)nullptr)) < 300) {
        return("null");
    }
#ifdef _MSC_VER
    //	int status;
    const auto& ref = *this;
    const auto name = typeid(ref).name();
    //const auto name = abi::__cxa_demangle(typeid(animal_ref).name(), 0, 0, &status);
    if (!strncmp("artd::", &name[5], 5)) {
        return(&name[11]);
    }
    return(&name[5]);
#else 
    return("unimplemented");
#endif
}

std::string ObjectBase::getPointerId(const void* p) {
    char buf[128];

    snprintf(buf, sizeof(buf) - 2, "%p", p);
    const char* pp = buf;
    while (*pp == '0') ++pp;
    if (!*pp) {
        return("null");
    }
    return(pp);
}

std::string ObjectBase::getCppObjectID() const {
    return(std::string(getCppClassName()) + "@" + getPointerId(this));
}


size_t ObjectBase::getAllocatedCount(bool final) {

    if (final) {

        AD_LOG(print) << "\n\n### unfreed objs [" << remainingObjs.size() << "]";
        size_t retSize = remainingObjs.size();
        for (auto it = remainingObjs.begin(); it != remainingObjs.end(); ++it) {
            const std::string &str = (it->second);
            //  .second;
          // int* pint = (int*)(obj->cbPtr);
          //   int val = *pint;
             AD_LOG(print) << "     fobj: " << str.c_str() << "\"";
        }
        AD_LOG(print) << "###\n";
        return(retSize);
    }
    if(trackOutstanding) {
        return(remainingObjs.size());
    }
    return(allocCount);
}

thread_local ObjAllocatorArg* _allocatorArg_ = nullptr;

ObjectBase::~ObjectBase() {
    remainingObjs.erase(this);
    cbPtr = nullptr;
    --allocCount;
}


void* ObjAllocatorArg::heapAllocate(size_t size) {
    ObjAllocatorArg* a = _allocatorArg_;
    if (a) {
        size += a->extraSize;
        a->allocatedSize = size;
   //     AD_LOG(info) << "allocating " << size << " bytes\n";
        return(a->allocatedAt = ::operator new(size));
    }
//    AD_LOG(info) << "allocating " << size << " bytes\n";
    return(::operator new(size));
}
void ObjAllocatorArg::heapDeallocate(void* ptr) {
//    AD_LOG(info) << "freeing obj @" << ((void*)ptr);
    return(::operator delete(ptr));
}

class ObjectBaseHolder {
    ObjectBase* pBase_;
public:
    INL ObjectBaseHolder(ObjectBase* pBase)
        : pBase_(pBase)
    {
    }
    INL ~ObjectBaseHolder() {
        if (pBase_) {
            delete(pBase_);
            pBase_ = nullptr;
        }
    }
    static void* poolAllocate(size_t size);
    static void poolDeallocate(void* ptr);
};


template<class T>
class PooledCBlockAllocator
{
public:

    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;


    template<typename U>
    struct rebind { typedef PooledCBlockAllocator<U> other; };

    PooledCBlockAllocator() throw() {}
    PooledCBlockAllocator(const PooledCBlockAllocator& other) throw() {}

    template<typename U>
    PooledCBlockAllocator(const PooledCBlockAllocator<U>& other) throw() {
    }
    template<typename U>
    PooledCBlockAllocator& operator = (const PooledCBlockAllocator<U>& other) {
        return *this;
    }
    PooledCBlockAllocator<T>& operator = (const PooledCBlockAllocator& other) { return *this; }
    ~PooledCBlockAllocator() {}

    pointer allocate(size_type n)
    {
        return(static_cast<T*>(ObjectBaseHolder::poolAllocate(n * sizeof(T))));
    }
    void deallocate(T* ptr, size_type n)
    {
        ObjectBaseHolder::poolDeallocate(ptr);
    }
};


void* ObjectBaseHolder::poolAllocate(size_t size) {
    ObjAllocatorArg* a = _allocatorArg_;
    if (a) {
        a->allocatedSize = size;
        // AD_LOG(info) << "allocating from pool " << size << " bytes\n";
        return(a->allocatedAt = ::operator new(size));
    }
    // AD_LOG(info) << "allocating pool block " << size << " bytes\n";
    return(::operator new(size));

}

void ObjectBaseHolder::poolDeallocate(void* ptr) {
    // AD_LOG(info) << "freeing pool allocated block";
    return(::operator delete(ptr));
}


void ObjectBase::addRef() {
    
    if (cbPtr == nullptr) {
        return; // TODO: assert this is a shared object !!!
    }
    HackStdShared<ObjectBase> buf(this, cbPtr);
    HackStdShared<ObjectBase> dbuf;
    new(&dbuf) ObjectPtr<ObjectBase>(buf.objPtr());
}

void ObjectBase::release() {
    
    if (cbPtr == nullptr) {
        return;
    }
    if (cbPtr == NOT_SHARED()) {
        return; // TODO delete self here ??
    }
    // this shoudle be ok as if we have a this it hasn't been deleted yet
    HackStdShared<ObjectBase> buf(this, cbPtr);
    if (buf.objPtr().use_count() > 0) {
        buf.objPtr() = nullptr;
    }
}

ObjectPtr<ObjectBase> ObjectBase::_makeHandle_(ObjectBase* forThis) {
    
    ++allocCount;
    ObjAllocatorArg& allocArg = *_allocatorArg_;
    std::shared_ptr<ObjectBaseHolder> sptr = std::allocate_shared<ObjectBaseHolder>(PooledCBlockAllocator<ObjectBaseHolder>(), forThis);
    ((void**)&sptr)[0] = (void*)forThis; // replace object with base object
    return(ObjectPtr<ObjectBase>(*reinterpret_cast<std::shared_ptr<ObjectBase>*>((void*)&sptr)));
}


RcString ObjectBase::toString() {
    return(RcString::format("Object@%p", (void*)this));
}

ARTD_END
