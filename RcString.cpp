#include "artd/RcString.h"
#include "artd/Formatf.h"

ARTD_BEGIN

template<>
RcStrBaseT<RcString, char>::super RcStrBaseT<RcString, char>::create(const wchar_t* from) {
	RcString str = format("%w", from);
	return(*reinterpret_cast<super*>(&str));
}

template<>
RcStrBaseT<RcString, char> RcStrBaseT<RcString, char>::create(const RcWString& s) {

	RcString str = format("%w", s->chars());
	return(*reinterpret_cast<RcStrBaseT<RcString, char>*>(&str));
}

template<>
RcStrBaseT<RcWString, wchar_t> RcStrBaseT<RcWString, wchar_t>::create(const RcString& s) {
	RcWString str = format("%s", s->chars());
	return(*reinterpret_cast<RcStrBaseT<RcWString, wchar_t>*>(&str));
}


RcString::RcString(const RcWString& str) : super(super::create(str)) {}

RcWString::RcWString(const RcString &str) : super(super::create(str)) {}


template<class ChT>
inline string_object<ChT>::string_object(int len)
	: len_(len)
{
#ifdef ENABLE_RCSTRING_VIEW
    const char* dis = (const char*)this;
    chars_ = (dis ? (ChT*)((dis + offsetOfChars())) : 0);
#endif
}

template<class ChT>
string_object<ChT>::~string_object()
{
}

template<class ChT>
class string_object<ChT>::Impl
	: public string_object<ChT>
{
public:
	Impl(int len)
		: string_object<ChT>(len)
	{}
};

template<>
RcString string_object<char>::toString() {
	return(RcString("wstr not done"));
}

template<>
RcString string_object<wchar_t>::toString() {
	return(RcString("wstr not done"));
}

template<>
RcString RcStrBaseT<RcString, char>::createForSize(int sLen) {

	ObjAllocatorArg allocArg(ObjT::sizeForChars(sLen) - sizeof(ObjT));
	std::shared_ptr<ObjT::Impl> sptr = std::allocate_shared<ObjT::Impl>(ObjectAllocator<ObjT::Impl>(), (int)sLen);
	return(*reinterpret_cast<RcString*>((void*)&sptr));
}

template<>
RcStrBaseT<RcString, char>::super RcStrBaseT<RcString, char>::create(const char* from) {

	const int sLen = (int)::strlen(from);
	RcString str = createForSize(sLen);
	CharT* pout = str->chars();

	const CharT* pin = from;
	const CharT* const pmax = pout + sLen; // obj->length();
	while (pout < pmax) {
		*pout++ = *pin++;
	}
	*pout = 0;
	return(*reinterpret_cast<super*>(&str));
}

template<>
RcString RcStrBaseT<RcString, char>::vformat(const char* fmt, const FormatfArglist<>& args) {

	FormatfStream fa;
	int sLen = fa.vlenf(fmt, args);
	fa.va_init(fmt, args);

	RcString str = createForSize(sLen);
	char* pchars = str->chars();
	fa.sprintf(pchars, sLen + 1);
	return(str);
}

template<>
RcString RcStrBaseT<RcString, char>::vformat(const wchar_t* fmt, const FormatfArglist<>& args) {
	FormatfStream fa;
	int sLen = fa.vlenf(fmt, args);
	fa.va_init(fmt, args);

	RcString str = createForSize(sLen);
	char* pchars = str->chars();
	fa.sprintf(pchars, sLen + 1);
	return(str);
}

// ********* WString

template<>
RcWString RcStrBaseT<RcWString, wchar_t>::createForSize(int sLen) {

	ObjAllocatorArg allocArg(ObjT::sizeForChars(sLen) - sizeof(ObjT));
	std::shared_ptr<ObjT::Impl> sptr = std::allocate_shared<ObjT::Impl>(ObjectAllocator<ObjT::Impl>(), (int)sLen);
	return(*reinterpret_cast<RcWString*>((void*)&sptr));
}

template<>
RcStrBaseT<RcWString, wchar_t>::super RcStrBaseT<RcWString, wchar_t>::create(const wchar_t* from) {

	const int sLen = (int)::wcslen(from);

	RcWString str = createForSize(sLen);
	CharT* pout = str->chars();

	const CharT* pin = from;
	const CharT* const pmax = pout + sLen; // obj->length();
	while (pout < pmax) {
		*pout++ = *pin++;
	}
	*pout = 0;
	return(*reinterpret_cast<super*>(&str));
}

template<>
RcWString RcStrBaseT<RcWString, wchar_t>::vformat(const char* fmt, const FormatfArglist<>& args) {

	FormatfStream fa;
	int sLen = fa.vlenf(fmt, args);
	fa.va_init(fmt, args);

	RcWString str = createForSize(sLen);
	wchar_t* pchars = str->chars();
	fa.sprintf(pchars, sLen + 1);
	return(str);
}

template<>
RcWString RcStrBaseT<RcWString, wchar_t>::vformat(const wchar_t* fmt, const FormatfArglist<>& args) {
	FormatfStream fa;
	int sLen = fa.vlenf(fmt, args);
	fa.va_init(fmt, args);

	RcWString str = createForSize(sLen);
	wchar_t* pchars = str->chars();
	fa.sprintf(pchars, sLen + 1);
	return(str);
}


template<>
RcStrBaseT<RcWString, wchar_t>::super RcStrBaseT<RcWString, wchar_t>::create(const char* from) {
	RcWString str = format("%s", from);
	return(*reinterpret_cast<super*>(&str));
}


#ifdef _MSC_VER
template class RcStrBaseT<RcString, char>;
template class RcStrBaseT<RcWString, wchar_t>;
#endif

ARTD_END

#include "artd/StringArg.h"

namespace {

using namespace artd;

	class TestClass
	{
	public:

		TestClass();
	};

	static TestClass testclass;

	TestClass::TestClass()
	{

#if 0
		StringArg sa("string arg");

		RcString test1; 		
		RcString test2("");
		RcString test3("test string");
		RcString test4(sa);

		if(test1) AD_LOG(debug) << "str1 \"" << test1->c_str() << "\"";
		if (test2) AD_LOG(debug) << "str2 \"" << test2->c_str() << "\"";
		if (test3) AD_LOG(debug) << "str3 \"" << test3->c_str() << "\"";

		if(test1 == "blah") {
		}
		test1 = sa;
		if (test1 == test2) {
		}
		// this wont work
		// if (test1 == sa) {}

		test1.equals(sa);
		test1.equals(test2);
		test1.equals("blah blah");

		StringArg sa2(test1);

		RcWString w4;
		RcWString w5(L"test W string");
		RcString w6(w5);

//		if (test4) AD_LOG(info) << "str4 \"" << (const wchar_t *)(test4->c_str()) << "\"";
//		if (test5) AD_LOG(info) << "str5 \"" << (const wchar_t *)(test5->c_str()) << "\"";
//		if (test6) AD_LOG(info) << "str6 \"" << (const wchar_t *)(test6->c_str()) << "\"";
#endif
	}

} // anonymous


#if 0

#include "ObjectInternals.h"
#include <artd/Formatf.h>
#include <artd/pointer_math.h>
#include <stdio.h>
#include <artd/PlatformSpecific.h>

#ifdef ARTD_LINUX
	#include <wchar.h>
#endif

#include "artd/StringArg.h"
#include <artd/StringUtil.h>
#include "artd/Logger.h"

wchar_t *test_global;

ARTD_BEGIN

/********* string_object *************/

template<class ChT>
inline string_object<ChT>::string_object(int len, int refs)
	: len_(len)
{
	chars_ = chars();
    Object::_unsafeSetRefCount_(refs);
}

template<class ChT>
inline string_object<ChT>::~string_object()
{~
}

template<class ChT>
class string_object<ChT>::Impl
	: public string_object<ChT>
{
public:
	Impl(int len,int refs)
		: string_object<ChT>(len,refs)
	{}
};


template<class ChT>
string_object<ChT> *
string_object<ChT>::constructInstance(void *buf, int charcount, RefScope scope)
{
	Impl *s = new(buf) Impl(charcount,__scope_init(scope));
	s->chars()[charcount] = 0;
	return(s);
}

template<class ChT>
string_object<ChT> *
string_object<ChT>::createNew(int charcount)
{
	void *p = operator new(sizeForChars(charcount));
	return(constructInstance(p,charcount,RC_HEAP));
}

template<class ChT>
string_object<ChT>*
string_object<ChT>::vformat1ref(const char* fmt, const FormatfArglist<>& args)
{
	FormatfStream fa;
	int len = fa.vlenf(fmt, args);
	fa.va_init(fmt, args);
	ThisT* s = createNew(len);
	CharT* pchars = s->chars();
	fa.sprintf(pchars, len + 1);
	return(s);
}
template<class ChT>
string_object<ChT> *
string_object<ChT>::vformat1ref(const wchar_t *fmt,const FormatfArglist<> &args)
{
	FormatfStream fa;
	int len = fa.vlenf(fmt,args);
	fa.va_init(fmt,args);
	ThisT *s = createNew(len);
	CharT *pchars = s->chars();
	fa.sprintf(pchars,len+1);
    return(s);
}

template<>
string_object<char> * 
string_object<char>::create1ref(const char *src)
{
	ThisT *rc = 0;
	if (src) {
		int len = (int)::strlen(src);
		rc = createNew(len);
		::memcpy(rc->chars(), src, len + 1);
	}
    return(rc);
}

template<>
string_object<wchar_t> *
string_object<wchar_t>::create1ref(const char *src)
{
    if (src) {
        return(format1ref("%s", src));
    }
    return(nullptr);
}

template<class ChT>
string_object<ChT> * 
string_object<ChT>::create1ref(const wchar_t *src)
{
    if (src) {
        return(format1ref("%s", src));
    }
    return(nullptr);
}

template<class ChT>
string_object<ChT> *
string_object<ChT>::createOrAddRef(const string_arg_base<char> &sb)
{
	const string_arg<char> &s = (string_arg<char>&)sb; 
	if (sizeof(ChT) == sizeof(char)) {
		if (s.type() == s.RC_STRING) {
            string_object<ChT> *obj = const_cast<ThisT *>((ThisT*)(s.getObj()));
            if (obj != nullptr) {
                obj->addRef();
            }
            return(obj);
        }
	}
    return(create1ref(s.c_str()));
}
template<class ChT>
string_object<ChT> *
string_object<ChT>::createOrAddRef(const string_arg_base<wchar_t> &sb)
{
	const string_arg<wchar_t> &s = (string_arg<wchar_t>&)sb; 
	if (sizeof(ChT) == sizeof(wchar_t)) {
		if (s.type() == s.RC_STRING) {
            string_object<ChT> *obj = const_cast<ThisT *>((ThisT*)(s.getObj()));
            if (obj != nullptr) {
                obj->addRef();
            }
            return(obj);
        }
	}
    return(create1ref(s.c_str()));
}

// _MSC_VER instantiate here for string_object
template class string_object<char>;
template class string_object<wchar_t>;


/***** AddedStringItem support for building strings with chains of + *****/
static 
const AddedStringItem *getSizeAndReverseList(const AddedStringItem *head, int &outsize)
{
	int size = 0;
	const AddedStringItem *prev = 0;
	FormatfStream fa;

	// reverse the list and count up total length
	// as list is built in reverse order of concatenations
	for(;;) 
	{
		fa.va_init_one(&(const_cast<AddedStringItem*>(head)->arg_));
		int lastSize = fa.sizef();
		size += lastSize;
		const AddedStringItem *next = head->next_;
		const_cast<AddedStringItem*>(head)->next_ = prev;	
		if(!next) {
			// if first object is null and a string don't print (nil)
			if(head->arg_.value_.obj_ == 0 && head->arg_.type_ >= head->tRCSTR) {
				size -= lastSize;
				head = prev;
			}
			break;
		}
		prev = head; 
		head = next;
	};
	outsize = size;
	return(head);
}
static void
formatToBuffer(char *pout, int size, const AddedStringItem *head )
{	
	FormatfStream fa;
	// format source into output
	for(const AddedStringItem *it = head;it;it = it->next_)
	{
		fa.va_init_one(&(const_cast<AddedStringItem*>(it)->arg_));
		int done = fa.sprintf(pout, size);
		size -= done;
		pout += done;
	}
}

static 
const AddedStringItem *getCountAndReverseList(const AddedStringItem *head, int &outsize)
{
	FormatfStream fa;
	int size = 0;
	const AddedStringItem *prev = 0;

	// reverse the list and count up total length
	// as list is built in reverse order of concatenations
	for(;;) 
	{
		fa.va_init_one(&(const_cast<AddedStringItem*>(head)->arg_));
		int lastSize = fa.lenf();
		size += lastSize;
		const AddedStringItem *next = head->next_;
		const_cast<AddedStringItem*>(head)->next_ = prev;	
		if(!next) {
			// if first object is null and a string don't print (nil)
			if(head->arg_.value_.obj_ == 0 && head->arg_.type_ >= head->tRCSTR) {
				size -= lastSize;
				head = prev;
			}
			break;
		}
		prev = head; 
		head = next;
	};
	outsize = size;
	return(head);
}

static 
void formatToBuffer(wchar_t *pout, int size, const AddedStringItem *head )
{	
	FormatfStream fa;
	// format source into output, may modify arg if "toString()" is called on an object.
	for(const AddedStringItem *it = head;it;it = it->next_)
	{
		fa.va_init_one(&(const_cast<AddedStringItem*>(it)->arg_));
		int done = fa.sprintf(pout, size);
		size -= done;
		pout += done;
	}
}
void AddedStringItemBase::build(string_object<char> *&out) const
{
    const AddedStringItem *me = (AddedStringItem*)this;
	out = 0;
	int count;
	const AddedStringItem *head = getSizeAndReverseList(me,count);
	out = &(string_object<char>::createNew(count)->I());
	formatToBuffer(out->chars(),count+1,head);
}
void AddedStringItemBase::build(string_object<wchar_t> *&out) const
{
    const AddedStringItem *me = (AddedStringItem*)this;
	out = 0;
	int count;
	const AddedStringItem *head = getCountAndReverseList(me,count);
	out = &(string_object<wchar_t>::createNew(count)->I());
	formatToBuffer(out->chars(),count+1,head);
}

#if 0
template class XStringTptrT<RcString, char>;
template class XStringTptrT<RcWString, wchar_t>;
#endif

template<>
bool
RcStrBaseT<RcWString,wchar_t>::equals(const WStringArg &sp) const
{
	if(!sp || !p_) {
		goto notequal;
	}
	if(::wcscmp(sp, p_->chars())) {
		goto notequal;
	}
	return(true);
notequal:
	return(false);
}

template<>
bool
RcStrBaseT<RcWString,wchar_t>::equalsIgnoreCase(const WStringArg &sp) const
{
	if(!sp || !p_) {
		goto notequal;
	}
	if(::_wcsicmp(sp, p_->chars())) {
		goto notequal;
	}
	return(true);
notequal:
	return(false);
}

template<>
bool
RcStrBaseT<RcWString,wchar_t>::startsWith(const WStringArg &sp) const
{
	if(!sp || !p_) {
		goto notequal;
	}
	if(::wcsncmp(sp, p_->chars(), ::wcslen(sp))) {
		goto notequal;
	}
	return(true);
notequal:
	return(false);
}

/*
int
RcWString::getBytes(int8_t *buf, int maxlen) const
{
	if(maxlen > p_->len_) {
		maxlen = p_->len_+1;
	}
	const wchar_t *src = p_->chars();
	const wchar_t *max = src + maxlen;

	while(src < max) {
		*buf++ = (int8_t)*src++;
	}
	return(p_->len_);
}
RcArray<int8_t>::TransPtr
RcWString::getBytes() const
{
	RcArrayOwnedObjectBase *a = RcArrayOwnedObjectBase::allocate(p_->len_,sizeof(int8_t));
	getBytes((int8_t *)(a->data()),a->len_);
	return(RcArray<int8_t>::TransPtr(static_cast<RcArrayObj<int8_t>*>(a),Object::NO_ADDREF));
}
RcString::TransPtr
ToCString(const wchar_t *wstr, char *stackbuf, int sbufSize)
{
	int namelen = wideCharToMultiByte(wstr,0,0,false);
	RcStringObj *p = 0;
	if(namelen > 0) {
		if(stackbuf && (sbufSize + RcArrayObj<char>::OffsetOfElements) >= namelen) {
			p = RcStringObj::constructInstance(stackbuf,namelen-1,RC_SCOPED);
		} else {
			p = RcStringObj::createNew(namelen-1);
		}
		char *name = p->chars();
		wideCharToMultiByte(wstr,name,namelen,false);
	}
	return(RcString::TransPtr(p,Object::NO_ADDREF));
}

RcString::TransPtr
RcWString::getCString() const
{
	return(ToCString(w_str(),0,0));
}

void
RcWString::copyOrClone(const RcWStringObj *sp)
{
	if(!sp) {
		setnull();
		return;
	}

	if(p_) {

		int diff = p_->len_ - sp->len_;

		if(diff < 0 || diff > 16) {
			setnull();
		} else {
			goto copyIt;
		}
	}

	// create new buffer
	p_ = RcWStringImpl::create(sp->len_);

copyIt:
	*const_cast<int *>(&p_->len_) = sp->len_;
	::memcpy(p_->chars(),sp->chars(),(sp->len_ * sizeof(wchar_t)) + sizeof(wchar_t));
}
*/


template<>
bool
RcStrBaseT<RcString,char>::equals(const StringArg &sp) const
{
	if(!sp || !p_) {
		goto notequal;
	}
	if(::strcmp(sp, p_->chars())) {
		goto notequal;
	}
	return(true);
notequal:
	return(false);
}

template<>
bool
RcStrBaseT<RcString,char>::equalsIgnoreCase(const StringArg &sp) const
{
	if(!sp || !p_) {
		goto notequal;
	}
	if(::_stricmp(sp, p_->chars())) {
		goto notequal;
	}
	return(true);
notequal:
	return(false);
}

template<>
bool
RcStrBaseT<RcString,char>::startsWith(const StringArg &sp) const
{
	if(!sp || !p_) {
		goto notequal;
	}
	if(::strncmp(sp, p_->chars(), ::strlen(sp))) {
		goto notequal;
	}
	return(true);
notequal:
	return(false);
}


template<class SubT, class ChT>
int 
RcStrBaseT<SubT,ChT>::lastIndexOf(unsigned int ch) const
{ 
#ifdef _MSC_VER
	const ChT *pc = p_->chars();
	int i = p_->length();
#else // __GNUC__
	const ChT *pc = super::p_->chars();
	int i = super::p_->length();
#endif

	while(--i >= 0)
	{
		if(pc[i] == ch) {
			break;
		}
	}
	return(i);
}

// instantiate here ??

// #ifdef _MSC_VER
	template class RcStrBaseT<RcString, char>;
	template class RcStrBaseT<RcWString, wchar_t>;
// #endif

ARTD_END

#if 1 // ndef ARTD_DEBUG


#include <string>

#ifdef __GNUC__
	#pragma GCC diagnostic ignored "-Wunused-function"
	#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

ARTD_BEGIN

// Try all the permutations of assignments to test overloading.

namespace {
   
typedef char CharArray[];
	
class C 
{
public:
	C() {}
	static void doIt(const wchar_t *c) {}
};
	
class C2
{
public:
	
	void doIt(const char *c) {}
	void doIt(char *c) { doIt((const char *)c); }
	
	template<class T>
	void doIt(const T &c)
	{
		C::doIt(c);
	}
	
};
	
void func()
{
	C2 c2;
	c2.doIt("a string");
}
	
static RcString cstr = "test";
static RcString cstr2 = 0;
static RcString cstr3 = NULL;
static RcString cstr4 = "init";
static RcString cstr5 = RcString::format("xxx");

	
static RcWString wstr = RcWString("test");
static RcWString wstr1 = L"test";
static RcWString wstr2 = 0;
static RcWString wstr3 = NULL;
static RcWString wstr4 = L"xx";
static RcWString wstr5 = RcWString::format("init");


class XTest {
public:

	XTest(RcString s, StringArg sa) 
		: cstr1_(s) 
		, cstr2_(0)
		, cstr3_("init")
		, cstr4_(RcString::format("init"))
		, cstr5_(sa)

		, wstr2_(0)
		, wstr3_(L"init")
		, wstr4_(RcWString::format("init"))
	{
		cstr1_ = sa;
	}

	void saFunc(StringArg arg)
	{
		StringArg sa(arg);
		AD_LOG(debug) << "string is %s", sa.c_str());
		if(arg == NULL) {
		    AD_LOG(debug) << "sa is NULL");
		}
		if(arg != NULL) {
		    AD_LOG(debug) << "sa is NOT NULL");
		}
		if(!arg) {
		    AD_LOG(debug) << "sa is NULL");
		}
		if(arg) {
		    AD_LOG(debug) << "sa is NOT NULL");
		}
	}

	void func1()
	{
		ObjectPtr<string_object<char> > tp;
		
		cstr1_ = "test2";
		saFunc("string");
		saFunc(cstr1_);
		tp = RcString::format("blah");
		saFunc(tp);
		saFunc(tp);
		
		saFunc(RcString::format("xc"));
	}

#ifdef ARTD_SUPPORT_STD_STRING
	void func2(std::string s);
#else
	void func2(const RcString &s);
#endif

	RcString cstr1_;
	RcString cstr2_;
	RcString cstr3_;
	RcString cstr4_;
	RcString cstr5_;


	RcWString wstr1_;
	RcWString wstr2_;
	RcWString wstr3_;
	RcWString wstr4_;
};

class HasAString {
public:
	RcString rc_;
	HasAString() : rc_("test text") {
		
	}
	operator RcString &() {
		return(rc_);
	}
};

class HasToString  {
public:
    RcString toString() const {
        return("blah blah blah");
    }
};

#ifdef ARTD_SUPPORT_STD_STRING
void
XTest::func2(std::string str)
#else
void
XTest::func2(const RcString &str)
#endif
{
	std::string xx("1234");

	cstr1_ = "test2";
	cstr2_ = str.c_str();
	cstr3_ = 0;	
	cstr1_ = "xx";
	//wstr2_ = RcString::format("yy");

	if(cstr1_ == 0) {
		xx = "8765";
	}

	if(cstr1_ != 0) {
		xx = "8765";
	}

	// NOTE: NULL comparisons in GCC3 generate a compiler warning
	
	if(cstr1_ == nullptr) {
		xx = "8765";
	}

	if(cstr1_ != nullptr) {
		xx = "8765";
	}

	if(!cstr1_) {
		xx = "5678";
	}

	if(cstr1_) {
		xx = "5678";
	}

	if(cstr1_ == cstr2_) {
		xx = "5678";
	}

	if(cstr1_ != cstr2_) {
		xx = "5678";
	}

	wstr1_ = "test2";
	wstr2_ = str.c_str();
	wstr3_ = 0;
	wstr1_ = L"xx";
	wstr3_ = L"xx";
	wstr2_ = RcWString::format("yy");

	if(wstr1_ == 0) {
		xx = "5678";
	}

	if(wstr1_ != 0) {
		xx = "5678";
	}

	if(wstr1_ == nullptr) {
		xx = "5678";
	}

	if(wstr1_ != nullptr) {
		xx = "5678";
	}

	if(!wstr1_) {
		xx = "5678";
	}
	
	if(wstr1_) {
		xx = "5678";
	}

	if(wstr1_ == wstr2_) {
		xx = "5678";
	}

	if(wstr1_ != wstr2_) {
		xx = "5678";
	}
	
	HasAString hasS;
	
	RcString rc; // (hasS);
	rc = (RcString &)hasS;
	
	
	const char *cstr = cstr1_;
	const wchar_t *wstr2 = wstr1_;
	const unsigned int uint321 = 1;
	const uint32_t uint322 = 1;
		
	RcString sres = cstr1_
		+ cstr
		+ " int is "
		+ 22 + L" wchars "
		+ 128.1
		+ false
		+ " "
		+ 'a'
		+ uint321
		+ uint322
		+ wstr2
          + wstr2_
          + HasToString();

	RcWString sres2 = cstr1_
		+ cstr
		+ " int is "
		+ 22 + L" wchars "
		+ 128.1
		+ false
		+ " "
		+ 'a'
		+ uint321
		+ uint322
		+ wstr2
        + wstr2_
        + HasToString();
}


}

ARTD_END

#endif // ARTD_DEBUG

#endif // 0
