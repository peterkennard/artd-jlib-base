#ifndef __artd_IntrusiveList_h
#define __artd_IntrusiveList_h

/*-
 * Truly ancient code - the first version I wrote was C code and assembler only back in 1984
 * before svn and git existed and it was stored on floppy disks !!
 * but very useful features that still aren't in the std: C++ library
 **********
 *
 * Copyright (c) 1996-2023 Peter Kennard and aRt&D Lab
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
 */

#include "artd/jlib_base.h"
#include "artd/static_assert.h"
#include "artd/pointer_math.h"

ARTD_BEGIN

/** @brief Generic node type for internal doubly linked lists
 * Note: This is UNSYCHRONIZED and raw !
 */
class ARTD_API_JLIB_BASE RawDlNode
{
	/** @brief you can't copy these they are linked up! */
	inline void operator=(const RawDlNode &) {}
public:
	inline RawDlNode(const RawDlNode &) { next = prev = this; }
	inline RawDlNode() { next = prev = this; }

	RawDlNode *next;
	RawDlNode *prev;

	inline RawDlNode(bool /*dontInit*/) {}
	inline void init() { next = prev = this; }

	// node is not attached to a list
	inline bool detached() const
	{
		return(next == this);
	}
	// node is attached to a list
	inline bool attached() const
	{
		return(next != this);
	}
	// remove node from list only 
	void rawDetach()
	{
		prev->next = next;
		next->prev = prev;
	}
	// remove and re-initialize node
	void unlink()
	{
		prev->next = next;
		next->prev = prev;
		next = prev = this;
	}
	inline void detach()
	{
		unlink();
	}
	inline void insertBefore(RawDlNode *node)
	// insert this before node
	{
		next = node;
		prev = node->prev;
		prev->next = this;
		node->prev = this;
	}
	inline void insertAfter(RawDlNode *node)
	// insert this after node
	{
		prev = node;
		next = node->next;
		next->prev = this;
		node->next = this;
	}
};

/** @brief node that will detach itself automaticly when destroyed
*/
class DlNode
	: public RawDlNode
{
	/** @brief you can't copy these they are linked up! */
	inline void operator=(const RawDlNode &) {}
public:
	inline DlNode(const RawDlNode &n) : RawDlNode(n) {}
	inline DlNode() : RawDlNode() {}
	inline ~DlNode() { detach(); }
};

/** @brief place holder for internal templated list
*/
template<class ListT>
class DlList
{
	RawDlNode list_;
public:
	~DlList();
#ifndef ARTD_SDK
	inline ListT *operator->() { return(reinterpret_cast<ListT *>(this)); } 
	inline const ListT *operator->() const { return(reinterpret_cast<ListT *>(this)); }
#endif
};

class SimpleDlist;

// a simple circular intrusive (nodes on objects) doubly linked list

template<class BaseT, class SuperT>
constexpr size_t base_offset_test()
{
    union {
        size_t i;
        const SuperT *p;
    } pp;

    pp.i = 8;

    const SuperT * const ps = pp.p; // reinterpret_cast<const SuperT*>((const void *)8);
    const BaseT * const pb = (const BaseT*)(ps);

    return(((const char * const)pb) - ((const char * const)ps)); // return(((size_t)((reinterpret_cast<SuperT*>((void *)8)))-8));
}



template<class _Nt = RawDlNode, class SuperT=SimpleDlist>
class IntrusiveList
{
protected:

	RawDlNode list_;
//	typedef IntrusiveList<_Nt,nodeOffset,SuperT> _Tt;
	typedef IntrusiveList<_Nt,SuperT> _Tt;
	typedef _Nt       _Node;
	typedef RawDlNode _Link;

protected:
	SuperT &tbase() { return(*(SuperT *)this); }

	static _Link *toLink(_Node *n)
	{
        return(static_cast<RawDlNode*>(n));
		// return((_Link *)ARTD_OPTR(n,nodeOffset));
	}
	static _Node *fromLink(_Link *n)
	{
        return(static_cast<_Node*>(n));
		//return((_Node *)ARTD_OPTR(n,-nodeOffset));
	}

	inline _Node *detach_(_Link *n)
	{
		// WARNING this will detach a node no matter where it is attached !!
		n->detach();
		_Node *node = fromLink(n);
		tbase().onDetach(node);
		return(node);
	}
	inline _Link *preAttach_(_Node *node)
	{
		// node must be initialized
		tbase().onAttach(node);
		_Link *n = toLink(node);
		n->rawDetach();
		return(n);
	}

public:

	typedef _Node &reference;
	typedef _Node *pointer;

	class iterator;
	friend class _Tt::iterator;

	class iterator
	{
	protected:

		_Link *_Ptr;
		_Tt	  *_list;

        //	friend class IntrusiveList<_Nt,nodeOffset,SuperT>;
        friend class IntrusiveList<_Nt,SuperT>;

	public:
		iterator() {}
//        iterator(iterator &it) : _Ptr(it._Ptr), _list(it._list) {}
		iterator(_Link *pt, _Tt * /*l*/) : _Ptr(pt) {}

		iterator &operator ++()
					{ _Ptr = _Ptr->next; return(*this); }
		iterator operator++(int)
					{ iterator _Tmp = *this; ++*this; return (_Tmp); }
		reference operator *()  const
			#if defined(_MSC_VER) && (_MSC_VER < 1400)
					{ return *((_Node *)ARTD_OPTR(_Ptr,-nodeOffset)); }
			#else
					// this should  be fine, but doesn't compile under msvc7
					{ return (*_Tt::fromLink(_Ptr)); }
			#endif
		pointer operator &() const
					{ return(_Tt::fromLink(_Ptr)); }
		pointer operator->() const
					{ return(_Tt::fromLink(_Ptr)); }
		bool operator==(const iterator& _X) const
					{ return (_Ptr == _X._Ptr); }
		bool operator!=(const iterator& _X) const
					{ return (_Ptr != _X._Ptr); }

		friend bool operator==(const _Link *l, const iterator &it)
					{ return (l == it._Ptr); }
		friend bool operator!=(const _Link *l, const iterator &it)
					{ return (l != it._Ptr); }

		iterator &remove()
		{
			_Link *p = _Ptr;
			_Ptr = _Ptr->next;
			_list->detach_(p);
			return(*this);
		}
	};

	IntrusiveList() {}
	IntrusiveList(bool dontInit) : list_(dontInit) {}
	IntrusiveList(const _Tt &) {}
    ~IntrusiveList() { list_.init(); }

	// overide for referencing/dereferencing/deleting etc
	void onAttach(void *) {}
	void onDetach(void *) {}

	inline iterator begin() { return(iterator(list_.next,this)); }
	inline iterator end()   { return(iterator(&list_,this)); }
	bool            empty() { return(list_.detached()); }
	bool            isEmpty() { return(list_.detached()); }

    /**
     * moves contents of from to this, deetaches all node from "from"
     * Nodes may not be present in two lists at once 
     */
	void operator =(_Tt &from)
	{
		if(from.empty())
		{
			list_.init();
			return;
		}
		else
		{
			(list_.next = from.list_.next)->prev = &list_;
			(list_.prev = from.list_.prev)->next = &list_;
			from.list_.init();
		}
	}
	/** @brief transfers contents of "from" list to tail of 
	 * this list. Leaves from list empty 
	 * note if called with self will go ka-boom 
     */
	void appendFrom(_Tt &from)
	{
		if(!from.empty())
		{
			RawDlNode *tail = list_.prev;
			(tail->next = from.list_.next)->prev = tail;
			(list_.prev = from.list_.prev)->next = &list_;
			from.list_.init();
		}
	}
	/** @brief insert on head of the list
	 assumes node is not already attached somewhere */
	inline void virginAddHead(_Node *node)
	{
		toLink(node)->insertAfter(&list_);
	}
	/** @brief insert on head of the list
	 safely detaches initialized or attached node first */
	inline void addHead(_Node *node)
	{
		preAttach_(node)->insertAfter(&list_);
	}
	inline void add(_Node *node)
	{
		addHead(node);
	}
	inline void remove(_Node *node)
	{
	   detach_(toLink(node));
	}
	/** @brief returns pointer to head of the list
	 null if empty */
	inline _Node *peekHead()
	{
		return(list_.next == &list_ ? 0 : fromLink(list_.next));
	}
	inline _Node *getHead()
	{
	   _Link *head = list_.next;

	   if(head == &list_)
			return(0);
	   return(detach_(head));
	}
	/** @brief insert on tail of the list
	 assumes node is not already attached somewhere */
	inline void virginAddTail(_Node *node)
	{
		toLink(node)->insertBefore(&list_);
	}
	/** @brief insert on tail of the list
	 safely detaches initialized or attached node first */
	inline void addTail(_Node *node)
	{
		preAttach_(node)->insertBefore(&list_);
	}
	/** @brief returns pointer to tail of the list
	 null if empty */
	inline _Node *peekTail()
	{
		return(list_.prev == &list_ ? 0 : fromLink(list_.prev));
	}
	inline _Node *getTail()
	{
	   _Link *tail = list_.prev;
	   if(tail == &list_)
			return(0);
	   return(detach_(tail));
	}
	void insertBefore(_Node *node, iterator &it)
	{
		preAttach_(node)->insertBefore(it._Ptr);
	}

	/** @brief insert @a node before @a after
	if after is 0 will insert at end (tail) of list
	*/
	void insertBefore(_Node *node, _Node *after)
	{
		_Link *al = after ? toLink(after) : &list_;
		preAttach_(node)->insertBefore(al);
	}

	/** @brief insert @a node after @a before
	if after is 0 will insert atbeginning (head) of list
	*/
	void insertAfter(_Node *node, _Node *before)
	{
		_Link *al = before ? toLink(before) : list_.next;
		preAttach_(node)->insertAfter(al);
	}

	void rotate()
	{
		list_.prev = list_.next;
		list_.next = list_.next->next;
	}
	int size()
	{
		_Link *node;
		int sz = 0;
		for(node = list_.next; node != &list_; node = node->next)
			++sz;
		return(sz);
	}
	bool sizeAtLeast(int count)
	{
		_Link *node;
		for(node = list_.next; node != &list_; node = node->next) {
			if(--count <= 0) {
				return(true);
			}
		}
		return(false);
	}
	void clear()
	{
	_Link *next;
	_Link *node;

		for(node = list_.next; node != &list_; node = next)
		{
			next = node->next;
			tbase().onDetach(detach_(node));
		}
	}
	typedef int forAllFunc(_Node &nd, void *data);

	int forAllVoid(forAllFunc *doNode, void *data)
	{
	_Link *next;
	_Link *node;
	int ret;

		for(node = list_.next; node != &list_; node = next)
		{
			next = node->next;  // allows self removal but not removal of next !!!
			if((ret = doNode(*fromLink(node),data)) != 0)
				return(ret);
		}
		return(0);
	}

	template<class _Ot> inline
	int forAll(int (*_doElem)(_Node &,_Ot *),
			   _Ot *obj)
	{
		return(forAllVoid((forAllFunc *)_doElem, obj));
	}
};

class SimpleDlist
	: public IntrusiveList<RawDlNode,SimpleDlist>
{
};

ARTD_STATIC_ASSERT(sizeof(DlList<SimpleDlist>) >= sizeof(SimpleDlist));

// ***** WARNING! this is not really fully tested! *****

// a simple circular intrusive (nodes on objects) doubly linked list
// compact headered version with one pointer for the header.  You must
// know the list header for the attach and remove operations.


template<class _Nt = RawDlNode, int const nodeOffset = 0>
class ShIntrusiveList
{
protected:

	RawDlNode *list_;
	typedef ShIntrusiveList<_Nt> _Tt;
	typedef _Nt    _Node;
	typedef RawDlNode _Link;

	static _Link *toLink(_Node *n)
	{
		return((_Link *)ARTD_OPTR(n,nodeOffset));
	}
	static _Node *fromLink(_Link *n)
	{
		return((_Node *)ARTD_OPTR(n,-nodeOffset));
	}
	inline _Node *detach_(_Link *n)
	{
		// note this will detach a node no matter where it is attached !!
		n->detach();
		onDetach(fromLink(n));
		return(fromLink(n));
	}
	inline _Link *preAttach_(_Node *node)
	{
		// node must be initialized
		_Link *n = toLink(node);
		onAttach(node);
		n->rawDetach();   // TBD see what to do here
		return(n);
	}

public:

	// std::list<> stuff should likely merge in

	bool            empty()  { return(list_ == 0); }


	ShIntrusiveList() { list_ = 0; }

	void onAttach(_Node *) {}
	void onDetach(_Node *) {}
	void onDelete(_Node *) {}

	inline void virginAddHead(_Node *node)
	{
		_Link *n = toLink(node);
		n->init();
		if(list_)
			n->insertBefore(list_);
		list_ = n;
	}
	inline void addHead(_Node *node)
	{
		_Link *n = preAttach(node);
		n->init();
		if(list_)
			n->insertBefore(list_);
		list_ = n;
	}
	inline _Node *peekHead()
	{
		return(list_ == 0 ? 0 : fromLink(list_));
	}
	inline _Node *getHead()
	{
		_Link *head = list_;
		if(head == 0)
			return(0);
	   if(head->detached())
			list_ = 0;
	   else
			list_ = head->next;
	   return(detach_(head));
	}
	inline void virginAddTail(_Node *node)
	{
		_Link *n = toLink(node);
		if(list_)
			n->insertBefore(list_);
		else
		{
			n->init();
			list_ = n;
		}
	}
	inline void addTail(_Node *node)
	{
		preAttach_(node);
		virginAddTail(node);
	}
	inline _Node *peekTail()
	{
		return(list_ == 0 ? 0 : fromLink(list_->prev));
	}
	inline _Node *getTail()
	{
		_Link *tail = list_;
		if(tail == 0)
			return(0);
		tail = tail->prev;
		if(tail == list_)
			list_ = 0;
		return(detach_(tail));
	}
	unsigned int size()
	{
		unsigned int  sz = 0;
		_Link *node = list_;
		if (node)
		{
			do {
				++sz;
				node = node->next;
			} while (node != list_);
		}
		return sz;
	}
	bool sizeAtLeast(int count)
	{
		_Link *node = list_;
		if (node)
		{
			do {
				if(--count <= 0) {
					return(true);
				}
				node = node->next;
			} while (node != list_);
		}
		return(false);
	}
	void rotate()
	{
		if(list_)
			list_ = list_->next;
	}
	void clear()
	{
	_Link *next;
	_Link *node;

		if((node = list_) == 0)
			return;

		node = node->next;
		for(;;)
		{
			next = node->next;
			this->tbase().onDelete(detach_(node));
			if(node == next) // last node
				break;
			node = next;
		}
		list_ = 0;
	}
	typedef int forAllFunc(_Node &nd, void *data);
	/*
	int forAllVoid(forAllFunc *doNode, void *data)
	{
	_Link *next;
	_Link *node;
	_Link *last;

		if((node = list_) == 0)
			return(0);

		first = node->prev;
		for(;;)
		{
			int ret;
			next = node->next;  // allows self removal but not removal of next or last !!!
			if((ret = doNode(*fromLink(node),data)) != 0)
				return(ret);
			if(next == last)
				break;
			node = next;
		}
		return(0);
	}
	*/
	template<class _Ot> inline
	int forAll(int (*_doElem)(_Node &,_Ot *),
			   _Ot *obj)
	{
		return(forAllVoid((forAllFunc *)_doElem, obj));
	}

	void operator =(_Tt &from)
	{
		from.list_ = list_;
		list_ = 0;
	}

};

typedef ShIntrusiveList<RawDlNode> SmallHeaderDlist;

ARTD_END

#endif // __artd_IntrusiveList_h
