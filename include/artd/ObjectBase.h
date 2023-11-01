#ifndef __artd_ObjectBase_h
#define __artd_ObjectBase_h

// ARTD_HEADER_DESCRIPTION: Base object interface and smart pointer definitions for managed objects.
// ARTD_HEADER_CREATOR: Peter Kennard 
// ARTD_HEADER_CREATED: March 1998

#include "artd/jlib_base.h"
#include "artd/static_assert.h"
#include <type_traits>
#include <mutex>

#define ARTD_OBJECT_DECL

#include <memory>

ARTD_BEGIN

#define INL ARTD_ALWAYS_INLINE

class ObjectBase;
class ObjAllocatorArg;

template<class ObjT>
class WeakPtr;

template<class ObjT>
class ObjectPtr;

template<class ObjT>
class HackStdShared {
	void* vp[2];
public:
	INL HackStdShared() {
	}
	INL HackStdShared(ObjT* obj,const void* cb) {
		vp[0] = obj;
        vp[1] = const_cast<void *>(cb);
	}
	INL HackStdShared(std::shared_ptr<ObjT>& sp) {
		const void** po = &sp;
		vp[0] = po[0];
		vp[1] = po[1];
	}
	INL HackStdShared(HackStdShared<ObjT>&& sp) {
		vp[0] = sp.vp[0];
		vp[1] = sp.vp[1];
		sp.vp[0] = nullptr;
		sp.vp[1] = nullptr;
	}
	INL ObjectPtr<ObjT>& objPtr() {
		return(*reinterpret_cast<ObjectPtr<ObjT>*>(&vp));
	}
	INL WeakPtr<ObjT>& weakptr() {
		return(*reinterpret_cast<WeakPtr<ObjT>*>(&vp));
	}
	INL std::shared_ptr<ObjT>& stdptr() {
		return(*reinterpret_cast<std::shared_ptr<ObjT>*>(&vp));
	}
	INL std::weak_ptr<ObjT>& stdweak() {
		return(*reinterpret_cast<std::weak_ptr<ObjT>*>(&vp));
	}
	INL void* cbPtr() {
		return(vp[1]);
	}
	INL const void* cbPtr() const {
		return(vp[1]);
	}
	template<class spType>
	INL static const void* cbPtr(const std::shared_ptr<spType>& sp) {
		return(reinterpret_cast<const HackStdShared<spType>*>(&sp)->cbPtr());
	}
	INL void* obj() const {
		return(vp[0]);
	}
	INL void setObj(void* obj) {
		vp[0] = obj;
	}
	INL void setCb(const void* cb) {
        vp[1] = const_cast<void *>(cb);
	}

};

struct ObjectPtrType {};

template<class ObjT_>
class ObjectPtr
	// to prevent this from truely "being" a shared_ptr change this to : protected
	: public ::std::shared_ptr<ObjT_>
	, public ObjectPtrType
{
public:
	typedef ObjT_ ObjT;
private:

	typedef ObjectPtr<ObjT>  ThisT;
	typedef ::std::shared_ptr<ObjT> super;

	friend class ObjectBase;
	friend class WeakPtr<ObjT>;

	ObjectPtr(ObjAllocatorArg& arg);

public:

	INL HackStdShared<ObjT>& _stdHack_() {
		return(*reinterpret_cast<HackStdShared<ObjT>*>(this));
	}

	template<class typeA>

	::std::shared_ptr<ObjT>& _myShared_() {
		ARTD_STATIC_ASSERT(sizeof(*this) == sizeof(std::shared_ptr<ObjectBase>));
		return(*reinterpret_cast<::std::shared_ptr<ObjT>*>(this));
	}

	INL explicit ObjectPtr(const super& from) : super(from) {}
	INL explicit ObjectPtr(super&& from) noexcept : super(std::move(from)) {}

	// the various standard constructors
	INL ObjectPtr() : super() {}
	INL ObjectPtr(nullptr_t) : super() {}
	INL ObjectPtr(const ThisT& from) : super(static_cast<const super&>(from)) {}
	INL ObjectPtr(ThisT&& from) noexcept 
		: super(std::move(static_cast<super&>(from))) 
	{}

	template<class OtherT>
	INL ObjectPtr(const ObjectPtr<OtherT>& other)
		: super(other)
	{}

	template<class OtherT>
	INL ObjectPtr(ObjectPtr<OtherT>&& other)
		: super(std::move(other))
	{}

	template<class OtherT>
	INL ObjectPtr(const std::shared_ptr<OtherT>& other)
		: super(other)
	{}

	template<class OtherT>
	INL ObjectPtr(std::shared_ptr<OtherT>&& other)
		: super(std::move(other))
	{}

	/** @brief assignment from null */
	INL ThisT& operator=(::std::nullptr_t) {
		super::operator=(super());
		return(*this);
	}

	/** @brief assignment from this type*/
	INL ThisT& operator=(const ThisT& r)
	{
		super::operator=(static_cast<const super&>(r));
		return(*this);
	}

	/** @brief move assignment from this type*/
	INL ThisT& operator=(ThisT&& r) noexcept
	{
		super::operator=(::std::move(static_cast<const super&&>(r)));
		return(*this);
	}

	/** @brief assignment from another type (casts) */
	template<class OtherT>
	INL ThisT& operator=(const std::shared_ptr<OtherT> &r)
	{
		super::operator=(r); //  static_cast<const super&>(r));
		return(*this);
	}

	/** @brief move assignment from another type (casts) */
	template<class OtherT>
	INL ThisT& operator=(std::shared_ptr<OtherT> &&r) noexcept
	{
		super::operator=(std::move(r)); //  ::std::move(static_cast<const super&&>(r)));
		return(*this);
	}

	INL ObjT* operator->() const {
		return(super::operator->());
	}

	INL ObjT* get() const {
		return(super::get());
	}

	/** @brief return pointer from this explicitly */
	INL ObjT* ptr() const { return(super::get()); }

	INL ObjT& operator*() {
		return(super::operator*());
	}

	INL const ObjT& operator*() const {
		return(super::operator*());
	}

	INL operator ObjT&() {
		return(super::operator*());
	}

	INL operator const ObjT&() const {
		return(super::operator*());
	}
	
	INL bool operator==(nullptr_t) const noexcept {
		return(super::get() == nullptr);
	}

	template<class CheckT>
	INL bool operator==(const ObjectPtr<CheckT>& o) const noexcept {
		return(super::get() ==(o.get()));
	}

	template<class CheckT>
	INL bool operator!=(const ObjectPtr<CheckT>& o) const noexcept {
		return(super::get() != (o.get()));
	}

	INL operator bool() const noexcept {
		return(super::operator bool());
	}
	
	INL bool operator !() const noexcept {
		return(!super::operator bool());
	}

	INL int use_count() const {
        // TODO some platforms use int64_t
		return((int)super::use_count());
	}
};

class RcString;
class RcWString;
class ArtdClass;

extern thread_local ObjAllocatorArg* _allocatorArg_;

class ARTD_API_JLIB_BASE ObjAllocatorArg {
    ObjAllocatorArg *prior_;
public:
	size_t extraSize;
	size_t allocatedSize = 0;
	void* allocatedAt = nullptr;

    
	INL ObjAllocatorArg(size_t extraSize = 0)
		: extraSize(extraSize)
	{
        prior_ = _allocatorArg_;
		// TODO: check is this item is on the stack or not.
		_allocatorArg_ = this;
	}

	INL static ObjAllocatorArg* getArg() {
		return(_allocatorArg_);
	}

	INL ~ObjAllocatorArg() {
        _allocatorArg_ = prior_;
	}

	static void* heapAllocate(size_t size);
	static void heapDeallocate(void* ptr);
};

class ARTD_API_JLIB_BASE ObjectBase
{
	template<bool>
	class isAnObjectBase_ {
	};

	template <class T>
	static INL void onObjectCreated_(T* obj, isAnObjectBase_<true>) {
		obj->onObjectCreated();
	};

	template <class T>
	static INL void onObjectCreated_(T* /*obj*/, isAnObjectBase_<false>) {
	};

	static uint8_t initValues[4];
	
	INL static void* NOT_SHARED() {
		return((void*)&initValues[0]);
	}

	void* cbPtr;
protected:

	friend class RcString;
	friend class RcWString;

	class CBlock;
	// this assigns the cbPtr to where the "control block" for this object was allocated
	// and will deal it "embeded" inherited objects in a containing class
	ObjectBase() : cbPtr(_allocatorArg_ != nullptr ? _allocatorArg_->allocatedAt : NOT_SHARED()) {}

	virtual const ArtdClass* getClass() const {
		return(nullptr);
	}

	template<class typeB>
	INL bool sameOwner(const std::shared_ptr<typeB>& b) {
		if (cbPtr != HackStdShared<typeB>::cbPtr(b) || cbPtr == NOT_SHARED()) {
			return(false);
		}
		return(true);
	}
	/** create an ObjectPtr<OwnedT> that hold a reference to this object
	 * so this will not be dereferenced until the Owning object is.
	 * This can be used to deal with multiple inheritance.
	 */
	template<class OwnedT>
	INL ObjectPtr<OwnedT> makeReferencingHandle(OwnedT* owned) {
		if (owned == nullptr || cbPtr == NOT_SHARED()) {
			return(nullptr);
		}
		HackStdShared<OwnedT> hack(owned, cbPtr);
		return(hack.objPtr());
	}
	static ObjectPtr<ObjectBase> makeHandle(ObjectBase* forThis);

private:

// Old one for C++ 11 ?
//	template<typename T>
//	struct hasPostCreateMethod
//	{
//		template<typename U, void (U::*)() const> struct SFINAE {};
//		template<typename U> static char Test(SFINAE<U, &U::onObjectCreated>*);
//		template<typename U> static int Test(...);
//		static const bool value = sizeof(Test<T>(0)) == sizeof(char);
//	};


    template<typename Class, typename Enabled = void>
    struct hasPostCreateMethod_s
    {
        static constexpr bool value = false;
    };

    template<typename Class>
    struct hasPostCreateMethod_s
    <
        Class,
        std::enable_if_t
        <
            std::is_member_function_pointer_v<decltype(&Class::onObjectCreated)>
        >
    >
    {
        static constexpr bool value = std::is_member_function_pointer_v<decltype(&Class::onObjectCreated)>;
    };

    template<typename Class>
    static constexpr bool hasPostCreateMethod()
    {
        return hasPostCreateMethod_s<Class>::value;
    };
private:

	template<typename ObjT>
	static void doPostCreate_(ObjT *obj, std::true_type)	{
		obj->onObjectCreated();
	}

	template<typename ObjT>
	static void doPostCreate_(ObjT * /*obj*/, std::false_type) {
	}
	template<typename ObjT>
	static void DoPostCreate(ObjT* obj)
	{
        doPostCreate_<ObjT>(obj, std::bool_constant<hasPostCreateMethod<ObjT>()>() );
	}
	
	// SFINAE test
	template <typename T>
	class has_myObjectBase
	{
		typedef char one;
		struct two { char x[2]; };
		template <typename C> static one test(decltype(&C::myObjectBase));
		template <typename C> static two test(...);

	public:
		enum { value = sizeof(test<T>(0)) == sizeof(char) };
	};

public:

	static void setKeepAllocatedRefs(bool);  // keeps a map of all outstanding objects
	static size_t getAllocatedCount(bool final=false);
	const char* getCppClassName() const;
	static std::string getPointerId(const void*);
	std::string getCppObjectID() const;

	template<class MyT>
	INL ObjectPtr<MyT> sharedFromThis(MyT* forThis) {
		return(makeReferencingHandle<MyT>(forThis));
	}

	void addRef();
	void release();

	virtual ~ObjectBase();

	// NOTE: this can make non ObjectBase objects and assign to ObjectPtr<xx> handles.
	template <class ObjT, class... _Types>
	static ObjectPtr<ObjT> make(_Types&&... args) {

		if (std::is_base_of<ObjectBase, ObjT>::value) {
			ObjAllocatorArg aaa;
			void* objmem = ::operator new(sizeof(ObjT));
			ObjectPtr<ObjectBase> hBase = makeHandle(reinterpret_cast<ObjectBase *>(objmem));
			ObjT* obj = new(objmem) ObjT(std::forward<_Types>(args)...);
			DoPostCreate<ObjT>(obj);
			return(*(reinterpret_cast<ObjectPtr<ObjT>*>(&hBase)));
		}
		else {
			// std::has_virtual_destructor<T> // TODO: mayube a special case for this ? to use handle pool ?
			ObjAllocatorArg aaa;
            ObjectPtr<ObjT> hObj = std::make_shared<ObjT>(std::forward<_Types>(args)...);
            DoPostCreate<ObjT>(hObj.get());
            return(hObj);
		}
	}
	virtual RcString toString();
};


struct WeakPointerType {};

template<class ObjT_>
class WeakPtr
	: public std::weak_ptr<ObjT_>
	, public WeakPointerType
{
	typedef std::weak_ptr<ObjT_> super;
public:
	typedef ObjT_ ObjT;

	INL WeakPtr() {}

	INL WeakPtr(const WeakPtr<ObjT>& from)
		: super(from)
	{}

	INL WeakPtr(const WeakPtr<ObjT>&& from)
		: super(from)
	{}

	INL WeakPtr(ObjectPtr<ObjT>& from)
		: super(from)
	{}

	INL WeakPtr(std::nullptr_t)
		: super()
	{}

	INL WeakPtr<ObjT>& operator=(std::nullptr_t)
	{
		(*this).~WeakPtr<ObjT>();
		new(this) super();
		return(*this);
	}
	INL WeakPtr<ObjT>& operator=(WeakPtr<ObjT> &&from)
	{
		super::operator=(std::move(from));
		return(*this);
	}

	INL ~WeakPtr() {}

	INL ObjectPtr<ObjT> lock() const {
		return(super::lock());
	}
};

template<typename T>
class ObjectAllocator
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
	struct rebind { typedef ObjectAllocator<U> other; };

	ObjectAllocator() throw() {}
	ObjectAllocator(const ObjectAllocator& /* other */) throw() {}

	template<typename U>
	ObjectAllocator(const ObjectAllocator<U>& /* other */) throw() {
	}
	template<typename U>
	ObjectAllocator& operator = (const ObjectAllocator<U>& /* other */) {
		return *this;
	}
	ObjectAllocator<T>& operator = (const ObjectAllocator& /* other */) { return *this; }
	~ObjectAllocator() {}

	pointer allocate(size_type n)
	{
		return(static_cast<T*>(ObjAllocatorArg::heapAllocate(n * sizeof(T))));
	}
	void deallocate(T* ptr, size_type /* n */)
	{
		ObjAllocatorArg::heapDeallocate(ptr);
	}
};


#undef INL // was set to ARTD_FORCE_INLINE

ARTD_END

#endif // __artd_ObjectBase.h

