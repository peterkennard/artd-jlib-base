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
 *  $Id$
 */

#ifndef __artd_RcString_h
#define __artd_RcString_h

#include "artd/ObjectBase.h"
#include "artd/RcArray.h"
#include "artd/FormatfArglist.h"

ARTD_BEGIN

#define INL ARTD_ALWAYS_INLINE

template<typename ChT>
class string_object
    : public ObjectBase
{
protected:
    typedef string_object<ChT> ThisT;

    ARTD_API_JLIB_BASE string_object(int len);

private:

    /** length of buffer in chars - not including terminal "nul" */
    int        len_;
#ifdef _DEBUG
    const ChT* chars_; // for debug build
#endif
public:
    virtual ARTD_API_JLIB_BASE ~string_object() override;

    RcString toString() override;

    typedef ChT CharT;
    class Impl;
    friend class Impl;
    Impl& I() { return(static_cast<Impl&>(*this)); }  // internal

    virtual int charSize() {
        return(sizeof(ChT));
    }

    INL static int offsetOfChars() { return(sizeof(ThisT)); }

    INL CharT* chars() { return((CharT*)(((char*)this) + offsetOfChars())); }
    INL const CharT* chars() const { return((CharT*)(((char*)this) + offsetOfChars())); }
    INL const CharT* c_str() const { 
        const char* dis = (const char*)this; 
        return(dis ? (CharT*)((dis + offsetOfChars())) : 0); 
    }

    /** @brief returns length of buffer in chars - not including terminal "nul" */
    INL int length() const { return(len_); }

    /** @brief returns size of string object in bytes for a specified charcount */
    INL static size_t sizeForChars(int numchars) { return((offsetOfChars() + sizeof(CharT)) + (numchars * sizeof(CharT))); }
};


class RcString;
class RcWString;


/** items common to string class handle types */
template<typename SubT, typename ChT>
class ARTD_API_JLIB_BASE RcStrBaseT
    : public ObjectPtr<string_object<ChT> >
{
    typedef ObjectPtr<string_object<ChT>> super;
    // typedef typename RcStrBaseT<SubT, ChT>    MyT;
public:

    typedef string_object<ChT>      ObjT;
    typedef ChT                     CharT;

protected:
    typedef SubT                    ThisT;
    friend SubT;

    static super create(const char* s);
    static super create(const wchar_t* s);
    static RcStrBaseT<SubT, ChT> create(const RcWString& s);
    static RcStrBaseT<SubT, ChT> create(const RcString& s);


    //    typedef string_arg_base<CharT>  ArgT;
    //    typedef string_arg<CharT>       FullArgT;

    //    INL const ObjT* p() const { return(super::p_); }
    //    INL ObjT* p() { return(super::p_); }
public:

    static SubT createForSize(int charcount);
//    static SubT createForSize(int charcount,const ChT *initialValue);

    static SubT vformat(const char* fmt, const FormatfArglist<>& args);
    static SubT vformat(const wchar_t* fmt, const FormatfArglist<>& args);

    template <typename... Args>
    static INL SubT format(const char* fmt, const Args &... args) {
        FormatfArglist<sizeof...(Args)> arglist;
        FormatfArglist<>::addArgs(arglist.args(), args...);
        return(vformat(fmt, arglist));
    }
    template <typename... Args>
    static INL SubT format(const wchar_t* fmt, const Args &... args) {
        FormatfArglist<sizeof...(Args)> arglist;
        FormatfArglist<>::addArgs(arglist.args(), args...);
        return(vformat(fmt, arglist));
    }

protected:
    INL RcStrBaseT() {}
    INL RcStrBaseT(SubT&& r) noexcept : super(std::move(r)) {
    }

    INL RcStrBaseT(const char* from) : super(create(from)) {}
    INL RcStrBaseT(const wchar_t* from) : super(create(from)) {}

    INL RcStrBaseT& operator=(const SubT& s) {
        super::operator=(static_cast<const super&>(s)); return(*this);
    }

    INL RcStrBaseT& operator=(SubT&& s) {
        super::operator=(std::move((super&&)s)); return(*this);
    }

    INL SubT &operator =(const char* from) {
        super::operator=(create(from));
        return(*(SubT*)this);
    }

    INL SubT &operator =(const wchar_t* from) {
        super::operator=(create(from));
        return(*(SubT*)this);
    }
    
public:

    INL int length() const {
        return(super::get()->length());
    }
    INL const ChT* c_str() const {
        return(super::get()->c_str());
    }
    INL ChT* chars() {
        return(super::get()->chars());
    }
    INL ChT& operator[](int ix) {
        return(super::get()->chars()[ix]);
    }
    INL const ChT& operator[](int ix) const {
        return(super::get()->chars()[ix]);
    }
};

class RcWString;

class ARTD_API_JLIB_BASE RcString
    : public RcStrBaseT<RcString, char>
{
    typedef RcStrBaseT<RcString, char> super;
    friend super;
public:

    INL RcString() {}
    INL RcString(const RcString& s) : super(s) {}
    INL RcString(RcString&& s) noexcept : super(std::move(s)) {}
    INL RcString(const char* s) : super(s) {}
    INL RcString(const wchar_t* s) : super(s) {}
    INL RcString(nullptr_t) : super() {}

    RcString(const string_arg<char>& sa);

#if 0
    /** @brief call one argument constructor from base class template */
    template<class S>
#ifndef __GNUC__
    INL
#endif
    RcString(const S& s) : super(s) {}
#endif

    // specialization for format conversion unicode to utf8
    RcString(const RcWString& str);

    /** @brief call assignment from base template */    
    INL RcString& operator=(const wchar_t* s) noexcept {
        super::operator=(s); return(*this);
    }
    INL RcString& operator=(const char* s) noexcept {
        super::operator=(s); return(*this);
    }

    INL RcString& operator=(nullptr_t) noexcept {
        super::operator=(super()); return(*this);
    }

    INL RcString& operator=(const RcString& rc) noexcept {
        super::operator=(rc); return(*this);
    }

    INL RcString& operator=(RcString && rc) noexcept {
        super::operator=(static_cast<super&&>(rc)); return(*this);
    }
    
    INL RcString toString() const {
        return(*this);
    }

    // TODO check lengths for fast fail ??
    INL bool operator==(const char* b) const {
        if (!get() || !b) {
            return(false);
        }
        if (c_str() == b) {
            return(true);
        }
        return(::strcmp(c_str(), b) == 0);
    }

    INL bool operator==(const RcString& b) const noexcept {
        if (get() == b.get()) {
            return(true);
        }
        if (b) {
            return(::strcmp(c_str(), b.c_str()) == 0);
        }
        return(false); 
    }

    bool equals(const string_arg<char> &b) const noexcept;
    struct Less
    {
        bool operator()(const string_arg<char> &a, const string_arg<char> &b) const;
    };

    bool operator()(const string_arg<char> &a, const string_arg<char> &b) const;
};


class ARTD_API_JLIB_BASE RcWString
    : public RcStrBaseT<RcWString, wchar_t>
{
    typedef RcStrBaseT<RcWString, wchar_t> super;
    friend super;
public:

    INL RcWString() {}
    INL RcWString(const RcWString& s) : super(s) {}
    INL RcWString(RcWString&& s) noexcept : super(std::move(s)) {}
    INL RcWString(const char* s) : super(s) {}
    INL RcWString(const wchar_t* s) : super(s) {}
    INL RcWString(nullptr_t) : super() {}

    explicit RcWString(const string_arg<wchar_t>& sa);

    // specialization for format conversion utf8 to unicode
    RcWString(const RcString &str);

    INL RcWString& operator=(const RcWString& rc) noexcept {
        super::operator=(rc); return(*this);
    }

    INL RcWString& operator=(RcWString&& rc) noexcept {
        super::operator=(std::move(rc)); return(*this);
    }

    INL RcString toString() const {
        return(RcString(*this));
    }

    INL RcWString& operator=(nullptr_t) {
        super::operator=(super()); return(*this);
    }

#if 0
    struct Less
    {
        INL bool operator()(const char* a, const char* b) const
        {
            return(::strcmp(a, b) < 0);
        }
    };
#endif

};

// placeholders for MS compiler DLL madness these are never used though.
template<>
INL RcStrBaseT<RcWString, wchar_t> RcStrBaseT<RcWString, wchar_t>::create(const RcWString& s) {
    return(s);
}

template<>
INL RcStrBaseT<RcString, char> RcStrBaseT<RcString, char>::create(const RcString& s) {
    return(s);
}

#undef INL

ARTD_END

#include "artd/Logger.h"
#include "artd/StringArg.h"

ARTD_ALWAYS_INLINE std::ostream& operator<<(std::ostream& os, const artd::RcString& v) {
    if (v) os << v.c_str();
    else os << "null";
    return(os);
}

ARTD_BEGIN

inline bool RcString::Less::operator()(const string_arg<char> &a, const string_arg<char> &b) const {
    return(::strcmp(a.c_str(), b.c_str()) < 0);
}

ARTD_END

namespace std {

// Note: this is for windows - TODO: other std libraries ?
// TODO: is it important for this to be in namespace std:: ??
template<>
struct hash<artd::RcString> {
    size_t operator()(const artd::RcString& _Keyval) const noexcept {
#ifdef _MSC_VER
        return std::_Hash_array_representation(_Keyval.c_str(), _Keyval.length());
#endif
        return(0);
    }
};

}



ARTD_BEGIN

#define INL ARTD_ALWAYS_INLINE

template<typename CharT>
class string_arg;

typedef string_arg<char> StringArg;
typedef string_arg<wchar_t> WStringArg;

template<>
INL int string_arg<char>::lengthFromRc() const {
    return(((RcString*)obj_)->length());
}
template<>
INL int string_arg<wchar_t>::lengthFromRc() const {
    return(((RcWString*)obj_)->length());
}

INL FormatfArgBase::Arg::Arg(const string_arg<char>& arg) : type_(tCHARS) { value_.chars_ = arg.c_str(); }
INL FormatfArgBase::Arg::Arg(const string_arg<wchar_t>& arg) : type_(tWCHARS) { value_.wchars_ = arg.c_str(); }


INL void FormatfArgBase::Arg::initFromObject(ObjectBase *ob) {
    RcString tstr = ob->toString();
    ob = tstr.get();
    new(&tstr) ObjectPtr<ObjectBase>();
}

INL RcString::RcString(const string_arg<char>& sa) {

    if (sa.type() == sa.RC_STRING) {
        HackStdShared<ObjectBase> hsb((ObjectBase*)(sa.getObj()), ((ObjectBase*)(sa.getObj()))->cbPtr);
        *this = *reinterpret_cast<RcString*>(&hsb);
    }
    else {
        ::new(this) super(sa.c_str());
    }
}

INL bool RcString::equals(const string_arg<char> &b) const noexcept {
    return(operator==(b.c_str()));
}

INL bool RcString::operator()(const string_arg<char> &a, const string_arg<char> &b) const {
    return(::strcmp(a.c_str(), b.c_str()) < 0);
}

INL RcWString::RcWString(const string_arg<wchar_t>& sa) {

    if (sa.type() == sa.RC_STRING) {
        HackStdShared<ObjectBase> hsb((ObjectBase*)(sa.getObj()), ((ObjectBase*)(sa.getObj()))->cbPtr);
        *this = *reinterpret_cast<RcWString*>(&hsb);
    }
    else {
        ::new((void*)this) super(sa.c_str());
    }
}

#undef INL

template<>
inline string_arg<char>::string_arg(const RcString& rc) : type_(RC_STRING) {
    s_ = rc.c_str();
    obj_ = rc.get();
}

ARTD_END



#endif // __artd_RcString_h

