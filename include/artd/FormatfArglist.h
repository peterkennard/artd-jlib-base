#ifndef __artd_FormatfArglist_h
#define __artd_FormatfArglist_h

#include "artd/ObjectBase.h"
#include "artd/StringArg.h"
#include "artd/int_types.h"

#ifdef ARTD_SUPPORT_STD_STRING
    #include <string>
#endif

ARTD_BEGIN

#define INL ARTD_ALWAYS_INLINE

class RcWString;

template<typename CharT>
class string_arg;

// class RcString;

class FormatfArgBase {
public:

	// note ordering and values of thes are important !! DO NOT CHANGE !!
    enum Type {
        tNONE = 0,
        tCHAR = 1,
        tWCHAR = 2,
        tINT = 3,
        tUINT = 4,
        tINT64 = 5,
        tUINT64 = 6,
        tREAL = 7,
        tPOINTER = 8,
        tCHARS = 9,
        tWCHARS = 10,
        tRCSTR = 11,  // first releasable object type
        tRCWSTR = 12,
        tOBJECT = 13,
		tNumTypes = 14
    };

    class Arg {
    
    public:

#ifdef _MSC_VER
    #pragma warning( push )
    #pragma warning( disable : 26495)
#endif
        
        INL Arg() : type_(tNONE) {}
        // note we have both const and non const versions. MSC will not assign a non const * to a const * overload (!)
		INL Arg(const char *v) : type_(tCHARS) { value_.chars_ = v; }
		INL Arg(char *v) : type_(tCHARS) { value_.chars_ = v; }
		INL Arg(const wchar_t *v) : type_(tWCHARS) { value_.wchars_ = v; }
		INL Arg(wchar_t *v) : type_(tWCHARS) { value_.wchars_ = v; }

        explicit INL Arg(const bool &v) : type_(tCHARS) { value_.chars_ = (v ? "true" : "false"); }
        explicit INL Arg(const char &v) : type_(tCHAR) { value_.uint_ = v; }
        explicit INL Arg(const wchar_t &v)   : type_(tWCHAR) { value_.uint_ = v; }
        explicit INL Arg(const uint16_t &v)  : type_(tUINT) { value_.uint_ = v; }
        explicit INL Arg(const int32_t &v)   : type_(tINT) { value_.int_ = v; }
        explicit INL Arg(const uint32_t &v)  : type_(tUINT) { value_.int_ = v; }
#if defined(ARTD_LONG_IS_64BIT)
        explicit INL Arg(const long int &v)  : type_(tINT64) { value_.int64_ = v; }
        explicit INL Arg(const unsigned long int &v)  : type_(tUINT64) { value_.uint64_ = v; }
#else
        explicit INL Arg(const unsigned long int &v)  : type_(tUINT) { value_.int_ = v; }
        explicit INL Arg(const long int &v)  : type_(tINT) { value_.int_ = v; }
#endif
        explicit INL Arg(const int64_t &v)   : type_(tINT64) { value_.int64_ = v; }
        explicit INL Arg(const uint64_t &v)  : type_(tUINT64) { value_.uint64_ = v; }
        explicit INL Arg(const float &v)     : type_(tREAL) { value_.double_ = v; }
        explicit INL Arg(const double &v)    : type_(tREAL) { value_.double_ = v; }
        explicit Arg(const string_arg<char>& arg);
        explicit Arg(const string_arg<wchar_t>& arg);

        // *** these are additioinally specialized in RcString.h  ( seems not to compile )
        template<class T>
        Arg(const T& v) {
            type_ = tRCSTR;
            ObjectPtr<ObjectBase> obj(v.toString());
            value_.ptr_ = obj.get(); // default is to call toString() method which returns an RcString
            // null it out so doesn't destroy
            new(&obj) ObjectPtr<ObjectBase>();
        }

#ifdef _MSC_VER
    #pragma warning( pop )
#endif
        
    #ifdef ARTD_SUPPORT_STD_STRING
      #ifdef _MSC_VER
        INL Arg(const std::string &v) : type_(tCHARS) { value_.chars_ = v.c_str(); }
        INL Arg(const std::wstring &v) : type_(tWCHARS) { value_.wchars_ = v.c_str(); }
      #else
        INL Arg(const std::string &v) : type_(tCHARS) { value_.chars_ = v.c_str(); }
        INL Arg(const std::wstring &v) : type_(tWCHARS) { value_.wchars_ = v.c_str(); }
      #endif
    #endif
        
        INL void releaseObj() {
            if(type_ >= tRCSTR) {
                if (value_.obj_) {
                    (const_cast<ObjectBase *>(value_.obj_))->release();
                    value_.obj_ = nullptr;
                }
            }
        }

        void initFromObject(ObjectBase *ob);
        
        template<class ObjT>
        INL Arg(const ObjectPtr<ObjT> &v) : type_(std::is_base_of<ObjectBase, ObjT>() ? tRCSTR : tPOINTER) {
            if (std::is_base_of<ObjectBase, ObjT>()) {
                ObjectBase* ob = static_cast<ObjectBase*>(v.get());
                if (ob) {
                    initFromObject(ob);
                }
                value_.obj_ = ob;
            }
            else {
                value_.obj_ = v.get();
            }
        }

        template<class ObjT>
        INL Arg(const ObjT * const &v) : type_(tPOINTER) { value_.ptr_ = v; }

        template<class ObjT>
        INL Arg(ObjT * const &v) : type_(tPOINTER) { value_.ptr_ = v; }

        union {
            const void *    ptr_;
            const char *    chars_;
            const wchar_t * wchars_;
            const ObjectBase * obj_;
            //char            char_;  // not used because formatf smallest arg passed is expanded to an int
            //wchar_t         wchar_;
            int             int_;
            int64_t         int64_;
            unsigned int uint_;
            uint64_t        uint64_;
            double          double_;
        } value_;
        Type            type_;
    };

protected:
    INL static void initArg(Arg *arg, Arg t) {
        arg[0] = t;
        t.type_ = tNONE;             
    }  
        
    template<class T>
    INL static void initArg(Arg *arg, const T &t) {
        new(arg) Arg(t);            
    }  
    template<class ObjT>
    INL static void initArg(Arg *arg, const artd::ObjectPtr<ObjT> &t) {
        new(arg) Arg(t);            
    }  

public:

    /** These are used in a variadic template to load the Arglist like a va_list
     *
     *	void logline(const char *fmt, const Args &... args) {  
     *		FormatfArglist<sizeof...(Args)> arglist;
     *		FormatfArglist<>::addArgs(arglist.args(), args...);
     *		outputlog(fmt,arglist);
     *  }  
     */
    INL static void addArgs(Arg * /* arglist */) {
    }

    template <typename First, typename... Rest> 
    static void addArgs(Arg *arglist, const First &first, const Rest &... rest) {            
        initArg(arglist,Arg(first));            
        if(sizeof... (Rest) > 0) {
            addArgs(++arglist, rest...); // recursive call using pack expansion syntax  
        }
    }  
};


template<int ArgCount = 1>
class FormatfArglist 
    : public FormatfArgBase
{
    typedef FormatfArgBase super;

    int currArg_;
    int argCount_;
    uint64_t arglist[   ((ArgCount * sizeof(Arg)) + sizeof(uint64_t))  /sizeof(uint64_t)  ];             
    FormatfArglist(const FormatfArglist<ArgCount> &) {} // non copyable.
public:
    FormatfArglist() : currArg_(0), argCount_(ArgCount) {
    }
    INL  Arg *args() {
        return((Arg*)&arglist);    
    }
    INL  const Arg *args() const {
        return((const Arg*)&arglist);    
    }
    INL int argCount() const {
        return(argCount_);    
    }
    ~FormatfArglist() {
        // release any references held in this list.
        Arg *arg = args();
        Arg *maxArg = arg + ArgCount; 
        while(arg < maxArg) { 
            arg->releaseObj();
            ++arg;
        }
     }
    INL operator FormatfArglist<> &() {
        return(*(FormatfArglist<> *)this);
    } 
    INL operator const FormatfArglist<> &() const {
        return(*(const FormatfArglist<> *)this);
    }
    template<class T>
    INL void addArg(const T &arg)  {
        if(currArg_ < argCount_) {
            super::initArg(&(args()[currArg_++]),arg);
        }
    } 
};

#undef INL

ARTD_END

#endif // __artd_FormatfArglist_h

