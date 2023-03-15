#ifndef __artd_ObjectBase_h
#define __artd_ObjectBase_h

// ARTD_HEADER_DESCRIPTION: Base object interface and smart pointer definitions for managed objects.
// ARTD_HEADER_CREATOR: Peter Kennard 
// ARTD_HEADER_CREATED: March 1998

#include "artd/jlib_base.h"
#include "artd/static_assert.h"
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
	INL HackStdShared(ObjT* obj, void* cb) {
		vp[0] = obj;
		vp[1] = cb;
	}
	INL HackStdShared(std::shared_ptr<ObjT>& sp) {
		const void** po = &sp;
		vp[0] = po[0];
		vp[1] = po[1];
	}
	INL ObjectPtr<ObjT>& objptr() {
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
	INL void* obj() {
		return(vp[0]);
	}
	INL void setObj(void* obj) {
		vp[0] = obj;
	}
	INL void setCb(void* cb) {
		vp[1] = cb;
	}
};

struct ObjectPtrType {};

template<class ObjT_>
class ObjectPtr
	: protected ::std::shared_ptr<ObjT_>
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
	INL ObjectPtr(const super& from) : super(from) {}
	INL ObjectPtr(const super&& from) noexcept : super(std::move(from)) {}

	/** @brief assignment from another*/
	INL ThisT& operator=(const super& r) noexcept
	{
		super::operator=(r);
		return(*this);
	}
	/** @brief move assignment from another*/
//	INL ThisT& operator=(const super&& r) noexcept
//	{
//		super::operator=(::std::move(r));
//		return(*this);
//	}

public:

	INL HackStdShared<ObjT>& _stdHack_() {
		return(*reinterpret_cast<HackStdShared<ObjT>*>(this));
	}

	::std::shared_ptr<ObjT>& _myShared_() {
		ARTD_STATIC_ASSERT(sizeof(*this) == sizeof(std::shared_ptr<ObjectBase>));
		return(*reinterpret_cast<::std::shared_ptr<ObjT>*>(this));
	}

	// the various standard constructors
	INL ObjectPtr() : super() {}
	INL ObjectPtr(nullptr_t) : super() {}
	INL ObjectPtr(const ThisT& from) : super(static_cast<const super&>(from)) {}
	INL ObjectPtr(ThisT&& from) noexcept 
		: super(std::move(static_cast<super&>(from))) 
	{}

	template<class OtherT>
	INL ObjectPtr(ObjectPtr<OtherT>& other)
		: std::shared_ptr<ObjT>(std::static_pointer_cast<ObjT, OtherT> (other._myShared_()))
	{}
	template<class OtherT>
	INL ObjectPtr(ObjectPtr<OtherT>&& other)
		: super(std::move(reinterpret_cast<super&>(other._myShared_())))
	{
		_stdHack_().setObj((void*)static_cast<ObjT*>((OtherT*)(_stdHack_().obj())));
	}

	/** @brief assignment from null */
	INL ThisT& operator=(::std::nullptr_t) {
		super::operator=(super());
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

	/** @brief assignment from another*/
	INL ThisT& operator=(const ThisT& r)
	{
		super::operator=(static_cast<const super&>(r));
		return(*this);
	}

	/** @brief move assignment from another*/
	INL ThisT& operator=(ThisT&& r) noexcept
	{
		super::operator=(::std::move(static_cast<const super&&>(r)));
		return(*this);
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
	static INL void onObjectCreated_(T* obj, isAnObjectBase_<false>) {
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

	ObjectBase(CBlock* forThis);

	virtual const ArtdClass* getClass() const {
		return(nullptr);
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
		return(hack.objptr());
	}
	
	static ObjectPtr<ObjectBase> makeHandle(ObjectBase* forThis);

private:

	template<typename T>
	struct hasPostCreateMethod
	{
		template<typename U, void (U::*)() const> struct SFINAE {};
		template<typename U> static char Test(SFINAE<U, &U::onObjectCreated>*);
		template<typename U> static int Test(...);
		static const bool value = sizeof(Test<T>(0)) == sizeof(char);
	};

	template<typename ObjT>
	static void DoPostCreate(ObjT *obj, std::true_type)	{
		obj->onObjectCreated();
	}

	template<typename ObjT>
	static void DoPostCreate(ObjT *obj, std::false_type) {
	}
	template<typename ObjT>
	static void DoPostCreate(ObjT* obj)
	{
		DoPostCreate(obj,
			std::integral_constant<bool, hasPostCreateMethod<ObjT>::value>());
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

	template <class ObjT>
	static ObjectPtr<ObjT> makeFromBase(ObjT* obj, std::false_type) {
		ObjectPtr<ObjectBase> hBase = makeHandle(obj);
		return(hBase);  // uses static cast to get frombase to ObjT
	}

	template <class ObjT>
	static ObjectPtr<ObjT> makeFromBase(ObjT* obj, std::true_type) {
		ObjectBase* base = obj->myObjectBase(); // method returns base when ambiguous
		ObjectPtr<ObjectBase> hBase = makeHandle(base);
		HackStdShared<ObjT> hack(obj, base->cbPtr);
		return(hack.objptr());
	}

public:

	INL ObjectBase()
		: ObjectBase((CBlock*)NOT_SHARED())
	{}

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

	template <class ObjT, class... _Types>
	static ObjectPtr<ObjT> make(_Types&&... args) {

		if (std::is_base_of<ObjectBase, ObjT>::value) {
            ObjAllocatorArg aaa;
			void *objmem = ::operator new(sizeof(ObjT));
			ObjT* obj =  new(objmem) ObjT(std::forward<_Types>(args)...);
			ObjectPtr<ObjT> retval 
				= makeFromBase<ObjT>(obj,
						std::bool_constant<has_myObjectBase<ObjT>::value>());
			DoPostCreate<ObjT>(obj);
			return(retval);
		}
		else {
			return(ObjectPtr<ObjT>());
		}
	}
	
	virtual RcString toString();
};


struct WeakPointerType {};

template<class ObjT_>
class WeakPtr
	: protected std::weak_ptr<ObjT_>
	, public WeakPointerType
{
	typedef std::weak_ptr<ObjT_> super;
public:
	typedef ObjT_ ObjT;

	INL WeakPtr(const WeakPtr<ObjT>& from)
		: super(from)
	{}

	INL WeakPtr(const WeakPtr<ObjT>&& from)
		: super(from)
	{}

	INL WeakPtr(ObjectPtr<ObjT>& from)
		: super(from._myShared_())
	{}

	INL WeakPtr(std::nullptr_t)
		: super()
	{}

	INL WeakPtr<ObjT> &operator=(std::nullptr_t)
	{
		(*this).~WeakPtr<ObjT>();
		new(this) super();
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
	ObjectAllocator(const ObjectAllocator& other) throw() {}

	template<typename U>
	ObjectAllocator(const ObjectAllocator<U>& other) throw() {
	}
	template<typename U>
	ObjectAllocator& operator = (const ObjectAllocator<U>& other) {
		return *this;
	}
	ObjectAllocator<T>& operator = (const ObjectAllocator& other) { return *this; }
	~ObjectAllocator() {}

	pointer allocate(size_type n)
	{
		return(static_cast<T*>(ObjAllocatorArg::heapAllocate(n * sizeof(T))));
	}
	void deallocate(T* ptr, size_type n)
	{
		ObjAllocatorArg::heapDeallocate(ptr);
	}
};


#undef INL // was set to ARTD_FORCE_INLINE

ARTD_END

#endif // __artd_ObjectBase.h

