#pragma once

#ifndef CONVENTION_KIT_STD_INSTANCE_H
#define CONVENTION_KIT_STD_INSTANCE_H

#include "Convention/instance/Interface.h"

template<typename TType>
struct ViewIndicator
{
	using tag = TType;
	constexpr static bool value = true;
};

namespace ConventionKit
{
	template<typename TType>
	using view = ViewIndicator<TType>;
}

template<typename TElement, typename TAlloc>
class instance<std::vector<TElement, TAlloc>, true>;
template<typename TElement, typename TAlloc>
class instance<ViewIndicator<std::vector<TElement, TAlloc>>, true>;

template<typename TInstance, typename... TArgs>
auto MakeView(instance<TInstance> data, TArgs&&...args)
{
	return instance<ViewIndicator<TInstance>>(data, std::forward<TArgs>(args)...);
}
template<typename TInstance, typename... TArgs>
auto MakeView(instance<ViewIndicator<TInstance>> data, TArgs&&...args)
{
	return instance<ViewIndicator<TInstance>>(data, std::forward<TArgs>(args)...);
}

#define copy_func(name) auto name() const {this->get()->name();}
#define copy_func_with_noexcept(name) auto name() const noexcept {this->get()->name();}
#define copy_func_return_auto(name) auto name() const {return this->get()->name();}
#define copy_func_return_auto_with_noexcept(name) auto name() const noexcept {return this->get()->name();}
#define copy_func_return_declauto(name) decltype(auto) name() const {return this->get()->name();}
#define copy_func_return_declauto_with_noexcept(name) decltype(auto) name() const noexcept {return this->get()->name();}

#pragma region std::vector

template<typename TElement, typename TAlloc>
class instance<std::vector<TElement, TAlloc>, true> :public instance<std::vector<TElement, TAlloc>, false>
{
public:
	using TMybase = instance<std::vector<TElement, TAlloc>, false>;
	using ElementType = TElement;
	using AllocatorType = TAlloc;
	using tag = std::vector<TElement, TAlloc>;
	using Iterator = typename tag::iterator;
	using ConstIterator = typename tag::const_iterator;
	using ViewInstance = instance<ViewIndicator<tag>, true>;

	instance() :TMybase(new tag()) {}
	instance(nullptr_t) :TMybase(nullptr) {}
	instance(tag* data) :TMybase(data) {}
	instance(typename TMybase::TShared& data) :TMybase(data) {}
	instance(typename TMybase::TShared&& data) :TMybase(std::move(data)) {}
	explicit instance(tag&& data) :TMybase(new tag(std::move(data))) {}
	explicit instance(std::initializer_list<TElement> data) :TMybase(new tag(data)) {}
	template<typename... TArgs>
	instance(TArgs&&... args) : TMybase(new tag(std::forward<TArgs>(args)...)) {}
	instance(instance& data) :TMybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		TMybase::operator=(other);
		return *this;
	}
	instance_move_operator(public) {}

	copy_func_return_auto_with_noexcept(Begin);
	copy_func_return_auto_with_noexcept(End);
	copy_func_return_auto_with_noexcept(CBegin);
	copy_func_return_auto_with_noexcept(CEnd);
	copy_func_return_auto_with_noexcept(RBegin);
	copy_func_return_auto_with_noexcept(REnd);
	copy_func_return_auto_with_noexcept(CRBegin);
	copy_func_return_auto_with_noexcept(CREnd);

	void Reserve(size_t size) const
	{
		this->get()->reserve(size);
	}
	void Resize(size_t size) const
	{
		this->get()->resize(size);
	}
	template<typename... TArgs>
	void PushBack(TArgs... args) const
	{
		this->get()->push_back(std::forward<TArgs>(args)...);
	}
	void PopBack() const noexcept
	{
		return this->get()->pop_back();
	}
	copy_func_return_auto_with_noexcept(Size);
	copy_func_with_noexcept(Clear);
	decltype(auto) At(int index) const
	{
		if (index < 0)
			index = this->Size() + index;
		return this->get()->at(index);
	}
	copy_func_return_declauto_with_noexcept(Front);
	copy_func_return_declauto_with_noexcept(Back);
	copy_func_return_auto_with_noexcept(Capacity);
	auto Data() const
	{
		return this->get()->data();
	}
	const auto CData() const
	{
		return this->get()->data();
	}
	bool IsEmpty() const noexcept
	{
		return this->get()->empty();
	}
	auto Erase(ConstIterator iter) const
	{
		return this->get()->erase(iter);
	}
	auto Erase(ConstIterator head, ConstIterator end) const
	{
		return this->get()->erase(head, end);
	}
	size_t Erase(const TElement& value, size_t ignore = 0, size_t countdown = static_cast<size_t>(-1)) const
	{
		int counter = ignore;
		counter = -counter;
		auto start = this->Begin();
		while (counter < countdown)
		{
			auto iter = std::find(start, this->End(), value);
			if (iter != this->End())
			{
				if (counter >= 0)
				{
					this->Erase(iter);
				}
				else
				{
					start = ++iter;
				}
				counter++;
			}
			else break;
		}
		return counter;
	}
	template<typename... TArgs>
	auto Insert(ConstIterator frontIter, TArgs&&... args)
	{
		return this->get()->insert(frontIter, std::forward<TArgs>(args)...);
	}
	template<typename... TArgs>
	auto Insert(size_t frontPos, TArgs&&... args)
	{
		return this->get()->insert(this->Begin() + frontPos, std::forward<TArgs>(args)...);
	}
	decltype(auto) operator[](int index) const
	{
		return this->At(index);
	}
	copy_func(ShrinkToFit);
};

template<typename TElement, typename TAlloc>
class instance<ViewIndicator<std::vector<TElement, TAlloc>>, true> :public instance<std::vector<TElement, TAlloc>, false>
{
private:
	size_t mHead, mTail;
public:
	template<typename TInside>
	friend int MoveView(instance<ViewIndicator<TInside>, true>& data, int offset);
	using TMybase = instance<std::vector<TElement, TAlloc>, false>;
	using ElementType = TElement;
	using AllocatorType = TAlloc;
	using tag = std::vector<TElement, TAlloc>;
	using Iterator = typename tag::iterator;
	using ConstIterator = typename tag::const_iterator;
	using SharedFromInstance = instance<std::vector<TElement, TAlloc>, true>;

	instance() :TMybase(nullptr) {}
	instance(typename TMybase::TShared& data, int head, int tail)
		:TMybase(data),
		mHead(head < 0 ? std::max<size_t>(0, data->size() - head) : std::max<size_t>(0, head)),
		mTail(tail < 0 ? std::max<size_t>(0, data->size() + tail) : std::min<size_t>(data->size(), tail))
	{
		mHead = std::min(mHead, data->size());
		mTail = std::min(mTail, data->size());
		if (mHead > mTail)
			std::swap(mHead, mTail);
	}
	instance(const instance& data) :TMybase(data), mHead(data.mHead), mTail(data.mTail) {}
	instance_move_operator(public)
	{
		this->mHead = other.mHead;
		this->mTail = other.mTail;
	}
	virtual ~instance() {}

	void Rebind(typename TMybase::TShared& data, int head, int tail)
	{
		TMybase::operator=(data);
		mHead = head < 0 ? std::max<size_t>(0, data->size() - head) : std::max<size_t>(0, head);
		mTail = tail < 0 ? std::max<size_t>(0, data->size() + tail) : std::min<size_t>(data->size(), tail);
		mHead = std::min(mHead, data->size());
		mTail = std::min(mTail, data->size());
		if (mHead > mTail)
			std::swap(mHead, mTail);
	}

	size_t Size() const noexcept
	{
		return mTail - mHead;
	}
	void Clear() noexcept
	{
		mHead = mTail = 0;
	}
	decltype(auto) At(int index) const
	{
		if (index < 0)
			index = this->Size() + index;
		return this->get()->at(mHead + index);
	}
	bool IsEmpty() const noexcept
	{
		return mHead == mTail;
	}
	auto Erase(ConstIterator head, ConstIterator end)
	{
		mTail -= std::distance(head, end);
		return this->get()->erase(head, end);
	}
	size_t Erase(const TElement& value, size_t ignore = 0, size_t countdown = static_cast<size_t>(-1))
	{
		int counter = ignore;
		counter = -counter;
		auto start = this->Begin();
		while (counter < countdown)
		{
			auto iter = std::find(start, this->End(), value);
			if (iter != this->End())
			{
				if (counter >= 0)
				{
					this->Erase(iter);
				}
				else
				{
					start = ++iter;
				}
				counter++;
			}
			else break;
		}
		return counter;
	}
	decltype(auto) operator[](int index) const
	{
		return this->At(index);
	}
};

#pragma endregion

#pragma region std::list

template<typename TElement, typename TAlloc>
class instance<std::list<TElement, TAlloc>, true> :public instance<std::list<TElement, TAlloc>, false>
{
public:
	using tag = std::list<TElement, TAlloc>;
	using TMybase = instance<tag, false>;
	using ElementType = TElement;
	using AllocatorType = TAlloc;
	using Iterator = typename tag::iterator;
	using ConstIterator = typename tag::const_iterator;
	using ViewInstance = instance<ViewIndicator<tag>, true>;

	instance() :TMybase(new tag()) {}
	instance(tag* data) :TMybase(data) {}
	instance(typename TMybase::TShared& data) :TMybase(data) {}
	instance(typename TMybase::TShared&& data) :TMybase(std::move(data)) {}
	explicit instance(tag&& data) :TMybase(new tag(std::move(data))) {}
	template<typename... TArgs>
	instance(TArgs&&... args) : TMybase(new tag(std::forward<TArgs>(args)...)) {}
	instance(instance& data) :TMybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		TMybase::operator=(other);
		return *this;
	}
	instance_move_operator(public) {}

	copy_func_return_auto_with_noexcept(Begin);
	copy_func_return_auto_with_noexcept(End);
	copy_func_return_auto_with_noexcept(CBegin);
	copy_func_return_auto_with_noexcept(CEnd);
	copy_func_return_auto_with_noexcept(RBegin);
	copy_func_return_auto_with_noexcept(REnd);
	copy_func_return_auto_with_noexcept(CRBegin);
	copy_func_return_auto_with_noexcept(CREnd);

	template<typename... TArgs>
	void PushBack(TArgs... args) const
	{
		this->get()->push_back(std::forward<TArgs>(args)...);
	}

	template<typename... TArgs>
	void PushFront(TArgs... args) const
	{
		this->get()->push_front(std::forward<TArgs>(args)...);
	}

	void PopBack() const noexcept
	{
		this->get()->pop_back();
	}

	void PopFront() const noexcept
	{
		this->get()->pop_front();
	}

	copy_func_return_auto_with_noexcept(Size);
	copy_func_with_noexcept(Clear);

	decltype(auto) Front() const
	{
		return this->get()->front();
	}

	decltype(auto) Back() const
	{
		return this->get()->back();
	}

	bool IsEmpty() const noexcept
	{
		return this->get()->empty();
	}

	auto Erase(ConstIterator iter) const
	{
		return this->get()->erase(iter);
	}

	auto Erase(ConstIterator head, ConstIterator end) const
	{
		return this->get()->erase(head, end);
	}

	template<typename... TArgs>
	auto Insert(ConstIterator pos, TArgs&&... args)
	{
		return this->get()->insert(pos, std::forward<TArgs>(args)...);
	}

	auto Splice(ConstIterator pos, tag& other)
	{
		return this->get()->splice(pos, other);
	}

	auto Splice(ConstIterator pos, tag& other, ConstIterator it)
	{
		return this->get()->splice(pos, other, it);
	}

	auto Splice(ConstIterator pos, tag& other, ConstIterator first, ConstIterator last)
	{
		return this->get()->splice(pos, other, first, last);
	}

	void Remove(const ElementType& value)
	{
		this->get()->remove(value);
	}

	template<typename TPred>
	void RemoveIf(TPred pred)
	{
		this->get()->remove_if(pred);
	}

	void Unique()
	{
		this->get()->unique();
	}

	template<typename TBinaryPred>
	void Unique(TBinaryPred pred)
	{
		this->get()->unique(pred);
	}

	void Sort()
	{
		this->get()->sort();
	}

	template<typename TCompare>
	void Sort(TCompare comp)
	{
		this->get()->sort(comp);
	}

	void Reverse() noexcept
	{
		this->get()->reverse();
	}

	void Merge(tag& other)
	{
		this->get()->merge(other);
	}

	template<typename TCompare>
	void Merge(tag& other, TCompare comp)
	{
		this->get()->merge(other, comp);
	}
};

template<typename TElement, typename TAlloc>
class instance<ViewIndicator<std::list<TElement, TAlloc>>, true> :public instance<std::list<TElement, TAlloc>, false>
{
public:
	using tag = std::list<TElement, TAlloc>;
	using TMybase = instance<tag, false>;
private:
	typename tag::iterator mHead;
	typename tag::iterator mTail;
	size_t mSize;
public:
	using ElementType = TElement;
	using AllocatorType = TAlloc;
	using Iterator = typename tag::iterator;
	using ConstIterator = typename tag::const_iterator;
	using SharedFromInstance = instance<tag, true>;

	instance() :TMybase(nullptr), mSize(0) {}
	instance(typename TMybase::TShared& data, Iterator head, Iterator tail)
		:TMybase(data), mHead(head), mTail(tail)
	{
		mSize = std::distance(head, tail);
	}

	instance(const instance& data)
		:TMybase(data), mHead(data.mHead), mTail(data.mTail), mSize(data.mSize) {}

	instance_move_operator(public)
	{
		this->mHead = other.mHead;
		this->mTail = other.mTail;
		this->mSize = other.mSize;
	}

	auto Begin() const noexcept { return mHead; }
	auto End() const noexcept { return mTail; }
	auto CBegin() const noexcept { return mHead; }
	auto CEnd() const noexcept { return mTail; }

	size_t Size() const noexcept { return mSize; }

	bool IsEmpty() const noexcept { return mSize == 0; }

	decltype(auto) Front() const { return *mHead; }
	decltype(auto) Back() const { return *std::prev(mTail); }

	auto Erase(ConstIterator pos)
	{
		if (pos == mHead) ++mHead;
		if (pos == mTail) --mTail;
		--mSize;
		return this->get()->erase(pos);
	}

	template<typename... TArgs>
	auto Insert(ConstIterator pos, TArgs&&... args)
	{
		if (pos == mHead) --mHead;
		if (pos == mTail) ++mTail;
		++mSize;
		return this->get()->insert(pos, std::forward<TArgs>(args)...);
	}
};

#pragma endregion

#pragma region std::map

template<typename TKey, typename TValue, typename TCompare, typename TAlloc>
class instance<std::map<TKey, TValue, TCompare, TAlloc>, true> :public instance<std::map<TKey, TValue, TCompare, TAlloc>, false>
{
public:
	using tag = std::map<TKey, TValue, TCompare, TAlloc>;
	using TMybase = instance<tag, false>;
	using KeyType = TKey;
	using ValueType = TValue;
	using CompareType = TCompare;
	using AllocatorType = TAlloc;
	using Iterator = typename tag::iterator;
	using ConstIterator = typename tag::const_iterator;
	using ViewInstance = instance<ViewIndicator<tag>, true>;

	instance() :TMybase(new tag()) {}
	instance(tag* data) :TMybase(data) {}
	instance(typename TMybase::TShared& data) :TMybase(data) {}
	instance(typename TMybase::TShared&& data) :TMybase(std::move(data)) {}
	explicit instance(tag&& data) :TMybase(new tag(std::move(data))) {}
	template<typename... TArgs>
	instance(TArgs&&... args) : TMybase(new tag(std::forward<TArgs>(args)...)) {}
	instance(instance& data) :TMybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		TMybase::operator=(other);
		return *this;
	}
	instance_move_operator(public) {}

	copy_func_return_auto_with_noexcept(Begin);
	copy_func_return_auto_with_noexcept(End);
	copy_func_return_auto_with_noexcept(CBegin);
	copy_func_return_auto_with_noexcept(CEnd);
	copy_func_return_auto_with_noexcept(RBegin);
	copy_func_return_auto_with_noexcept(REnd);
	copy_func_return_auto_with_noexcept(CRBegin);
	copy_func_return_auto_with_noexcept(CREnd);

	copy_func_return_auto_with_noexcept(Size);
	copy_func_with_noexcept(Clear);

	bool IsEmpty() const noexcept
	{
		return this->get()->empty();
	}

	auto Erase(ConstIterator iter) const
	{
		return this->get()->erase(iter);
	}

	auto Erase(ConstIterator head, ConstIterator end) const
	{
		return this->get()->erase(head, end);
	}

	template<typename... TArgs>
	auto Insert(TArgs&&... args)
	{
		return this->get()->insert(std::forward<TArgs>(args)...);
	}

	template<typename... TArgs>
	auto InsertOrAssign(TArgs&&... args)
	{
		return this->get()->insert_or_assign(std::forward<TArgs>(args)...);
	}

	template<typename... TArgs>
	auto TryEmplace(TArgs&&... args)
	{
		return this->get()->try_emplace(std::forward<TArgs>(args)...);
	}

	auto Find(const KeyType& key) const
	{
		return this->get()->find(key);
	}

	auto LowerBound(const KeyType& key) const
	{
		return this->get()->lower_bound(key);
	}

	auto UpperBound(const KeyType& key) const
	{
		return this->get()->upper_bound(key);
	}

	auto EqualRange(const KeyType& key) const
	{
		return this->get()->equal_range(key);
	}

	size_t Count(const KeyType& key) const
	{
		return this->get()->count(key);
	}

	bool Contains(const KeyType& key) const
	{
		return this->get()->contains(key);
	}

	ValueType& operator[](const KeyType& key) const
	{
		return (*this->get())[key];
	}

	ValueType& At(const KeyType& key) const
	{
		return this->get()->at(key);
	}
};

template<typename TKey, typename TValue, typename TCompare, typename TAlloc>
class instance<ViewIndicator<std::map<TKey, TValue, TCompare, TAlloc>>, true> :public instance<std::map<TKey, TValue, TCompare, TAlloc>, false>
{
public:
	using tag = std::map<TKey, TValue, TCompare, TAlloc>;
	using TMybase = instance<tag, false>;
private:
	typename tag::iterator mHead;
	typename tag::iterator mTail;
	size_t mSize;
public:
	using KeyType = TKey;
	using ValueType = TValue;
	using CompareType = TCompare;
	using AllocatorType = TAlloc;
	using Iterator = typename tag::iterator;
	using ConstIterator = typename tag::const_iterator;
	using SharedFromInstance = instance<tag, true>;

	instance() :TMybase(nullptr), mSize(0) {}
	instance(typename TMybase::TShared& data, Iterator head, Iterator tail)
		:TMybase(data), mHead(head), mTail(tail)
	{
		mSize = std::distance(head, tail);
	}

	instance(const instance& data)
		:TMybase(data), mHead(data.mHead), mTail(data.mTail), mSize(data.mSize) {}

	instance_move_operator(public)
	{
		this->mHead = other.mHead;
		this->mTail = other.mTail;
		this->mSize = other.mSize;
	}

	auto Begin() const noexcept { return mHead; }
	auto End() const noexcept { return mTail; }
	auto CBegin() const noexcept { return mHead; }
	auto CEnd() const noexcept { return mTail; }

	size_t Size() const noexcept { return mSize; }

	bool IsEmpty() const noexcept { return mSize == 0; }

	decltype(auto) Front() const { return *mHead; }
	decltype(auto) Back() const { return *std::prev(mTail); }

	auto Erase(ConstIterator pos)
	{
		if (pos == mHead) ++mHead;
		if (pos == mTail) --mTail;
		--mSize;
		return this->get()->erase(pos);
	}

	template<typename... TArgs>
	auto Insert(TArgs&&... args)
	{
		auto result = this->get()->insert(std::forward<TArgs>(args)...);
		if (result.second)
		{
			if (result.first < mHead) --mHead;
			if (result.first >= mTail) ++mTail;
			++mSize;
		}
		return result;
	}
};

#pragma endregion

#pragma region std::set

template<typename _Key, typename _Compare, typename _Alloc>
class instance<std::set<_Key, _Compare, _Alloc>, true> :public instance<std::set<_Key, _Compare, _Alloc>, false>
{
public:
	using tag = std::set<_Key, _Compare, _Alloc>;
	using _Mybase = instance<tag, false>;
	using key_type = _Key;
	using value_type = _Key;
	using key_compare = _Compare;
	using allocator_type = _Alloc;
	using iterator = typename tag::iterator;
	using const_iterator = typename tag::const_iterator;
	using view_instance = instance<view_indicator<tag>, true>;

	instance() :_Mybase(new tag()) {}
	instance(tag* data) :_Mybase(data) {}
	instance(typename _Mybase::_shared& data) :_Mybase(data) {}
	instance(typename _Mybase::_shared&& data) :_Mybase(std::move(data)) {}
	explicit instance(tag&& data) :_Mybase(new tag(std::move(data))) {}
	template<typename... _Args>
	instance(_Args&&... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
	instance(instance& data) :_Mybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		_Mybase::operator=(other);
		return *this;
	}
	instance_move_operator(public) {}

	copy_func_return_auto_with_noexcept(begin);
	copy_func_return_auto_with_noexcept(end);
	copy_func_return_auto_with_noexcept(cbegin);
	copy_func_return_auto_with_noexcept(cend);
	copy_func_return_auto_with_noexcept(rbegin);
	copy_func_return_auto_with_noexcept(rend);
	copy_func_return_auto_with_noexcept(crbegin);
	copy_func_return_auto_with_noexcept(crend);

	copy_func_return_auto_with_noexcept(size);
	copy_func_with_noexcept(clear);

	bool data_empty() const noexcept { return this->get()->empty(); }

	auto find(const key_type& key) const { return this->get()->find(key); }
	size_t count(const key_type& key) const { return this->get()->count(key); }
	auto lower_bound(const key_type& key) const { return this->get()->lower_bound(key); }
	auto upper_bound(const key_type& key) const { return this->get()->upper_bound(key); }
	auto equal_range(const key_type& key) const { return this->get()->equal_range(key); }

	template<typename... _Args>
	auto insert(_Args&&... args) { return this->get()->insert(std::forward<_Args>(args)...); }

	template<typename... _Args>
	auto emplace(_Args&&... args) { return this->get()->emplace(std::forward<_Args>(args)...); }

	auto erase(const_iterator pos) { return this->get()->erase(pos); }
	auto erase(const_iterator first, const_iterator last) { return this->get()->erase(first, last); }
	size_t erase(const key_type& key) { return this->get()->erase(key); }
};

#pragma endregion

#pragma region std::unordered_map

template<typename _Key, typename _Ty, typename _Hash, typename _KeyEq, typename _Alloc>
class instance<std::unordered_map<_Key, _Ty, _Hash, _KeyEq, _Alloc>, true>
    :public instance<std::unordered_map<_Key, _Ty, _Hash, _KeyEq, _Alloc>, false>
{
public:
    using tag = std::unordered_map<_Key, _Ty, _Hash, _KeyEq, _Alloc>;
    using _Mybase = instance<tag, false>;
    using key_type = _Key;
    using mapped_type = _Ty;
    using value_type = std::pair<const _Key, _Ty>;
    using hasher = _Hash;
    using key_equal = _KeyEq;
    using allocator_type = _Alloc;
    using iterator = typename tag::iterator;
    using const_iterator = typename tag::const_iterator;
    using local_iterator = typename tag::local_iterator;
    using const_local_iterator = typename tag::const_local_iterator;
    using view_instance = instance<view_indicator<tag>, true>;

    instance() :_Mybase(new tag()) {}
    instance(tag* data) :_Mybase(data) {}
    instance(typename _Mybase::_shared& data) :_Mybase(data) {}
    instance(typename _Mybase::_shared&& data) :_Mybase(std::move(data)) {}
    explicit instance(tag&& data) :_Mybase(new tag(std::move(data))) {}
    template<typename... _Args>
    instance(_Args&&... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		_Mybase::operator=(other);
		return *this;
	}
    instance_move_operator(public) {}

    copy_func_return_auto_with_noexcept(begin);
    copy_func_return_auto_with_noexcept(end);
    copy_func_return_auto_with_noexcept(cbegin);
    copy_func_return_auto_with_noexcept(cend);

    copy_func_return_auto_with_noexcept(size);
    copy_func_with_noexcept(clear);

    bool data_empty() const noexcept { return this->get()->empty(); }

    auto find(const key_type& key) const { return this->get()->find(key); }
    size_t count(const key_type& key) const { return this->get()->count(key); }
    auto equal_range(const key_type& key) const { return this->get()->equal_range(key); }

    template<typename... _Args>
    auto insert(_Args&&... args) { return this->get()->insert(std::forward<_Args>(args)...); }

    template<typename... _Args>
    auto emplace(_Args&&... args) { return this->get()->emplace(std::forward<_Args>(args)...); }

    auto erase(const_iterator pos) { return this->get()->erase(pos); }
    auto erase(const_iterator first, const_iterator last) { return this->get()->erase(first, last); }
    size_t erase(const key_type& key) { return this->get()->erase(key); }

    mapped_type& operator[](const key_type& key) { return (*this->get())[key]; }
    mapped_type& operator[](key_type&& key) { return (*this->get())[std::move(key)]; }
    mapped_type& at(const key_type& key) { return this->get()->at(key); }
    const mapped_type& at(const key_type& key) const { return this->get()->at(key); }

    // 哈希表特有操作
    copy_func_return_auto_with_noexcept(bucket_count);
    copy_func_return_auto_with_noexcept(max_bucket_count);
    size_t bucket_size(size_t n) const { return this->get()->bucket_size(n); }
    size_t bucket(const key_type& key) const { return this->get()->bucket(key); }

    float load_factor() const noexcept { return this->get()->load_factor(); }
    float max_load_factor() const noexcept { return this->get()->max_load_factor(); }
    void max_load_factor(float ml) { this->get()->max_load_factor(ml); }
    void rehash(size_t count) { this->get()->rehash(count); }
    void reserve(size_t count) { this->get()->reserve(count); }

    auto begin(size_t n) { return this->get()->begin(n); }
    auto end(size_t n) { return this->get()->end(n); }
    auto cbegin(size_t n) const { return this->get()->cbegin(n); }
    auto cend(size_t n) const { return this->get()->cend(n); }
};

template<typename _Key, typename _Ty, typename _Hash, typename _KeyEq, typename _Alloc>
std::ostream& map_easy_json(
	std::ostream& os,
	typename const std::unordered_map<_Key, _Ty, _Hash, _KeyEq, _Alloc>::iterator& begin,
	typename const std::unordered_map<_Key, _Ty, _Hash, _KeyEq, _Alloc>::iterator& end,
	bool is_format = true
)
{
	size_t layer = 0;
    os << "{";
    for (auto iter = begin; iter != end; ++iter)
    {
        if (iter != begin)
            os << ", ";
        if (is_format)
        {
            os << std::endl << std::string(layer * 4, ' ');
        }
        if constexpr(is_string_or_string_instance_v<_Ty>)
        {
            os << "\"" << iter->first << "\": \"" << iter->second << "\"";
        }
        else
        {
            os << "\"" << iter->first << "\": " << iter->second;
        }
        if (is_format)
        {
            os << std::endl;
            layer++;
        }
    }
    if (is_format)
    {
        os.seekp(-1, std::ios_base::cur);
        os << std::endl << std::string(layer * 4, ' ');
        os << "}";
    }
    else
    {
        os << "}";
    }
    return os;
}

template<typename _Key, typename _Ty, typename _Hash, typename _KeyEq, typename _Alloc>
std::ostream& operator<<(std::ostream& os, const instance<std::unordered_map<_Key, _Ty, _Hash, _KeyEq, _Alloc>, false>& data)
{
    return map_easy_json(os, data.begin(), data.end());
}

#pragma endregion

#pragma region std::unordered_set

template<typename _Key, typename _Hash, typename _KeyEq, typename _Alloc>
class instance<std::unordered_set<_Key, _Hash, _KeyEq, _Alloc>, true>
    :public instance<std::unordered_set<_Key, _Hash, _KeyEq, _Alloc>, false>
{
public:
    using tag = std::unordered_set<_Key, _Hash, _KeyEq, _Alloc>;
    using _Mybase = instance<tag, false>;
    using key_type = _Key;
    using value_type = _Key;
    using hasher = _Hash;
    using key_equal = _KeyEq;
    using allocator_type = _Alloc;
    using iterator = typename tag::iterator;
    using const_iterator = typename tag::const_iterator;
    using local_iterator = typename tag::local_iterator;
    using const_local_iterator = typename tag::const_local_iterator;
    using view_instance = instance<view_indicator<tag>, true>;

    instance() :_Mybase(new tag()) {}
    instance(tag* data) :_Mybase(data) {}
    instance(typename _Mybase::_shared& data) :_Mybase(data) {}
    instance(typename _Mybase::_shared&& data) :_Mybase(std::move(data)) {}
    explicit instance(tag&& data) :_Mybase(new tag(std::move(data))) {}
    template<typename... _Args>
    instance(_Args&&... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		_Mybase::operator=(other);
		return *this;
	}
    instance_move_operator(public) {}

    copy_func_return_auto_with_noexcept(begin);
    copy_func_return_auto_with_noexcept(end);
    copy_func_return_auto_with_noexcept(cbegin);
    copy_func_return_auto_with_noexcept(cend);

    copy_func_return_auto_with_noexcept(size);
    copy_func_with_noexcept(clear);

    bool data_empty() const noexcept { return this->get()->empty(); }

    auto find(const key_type& key) const { return this->get()->find(key); }
    size_t count(const key_type& key) const { return this->get()->count(key); }
    auto equal_range(const key_type& key) const { return this->get()->equal_range(key); }

    template<typename... _Args>
    auto insert(_Args&&... args) { return this->get()->insert(std::forward<_Args>(args)...); }

    template<typename... _Args>
    auto emplace(_Args&&... args) { return this->get()->emplace(std::forward<_Args>(args)...); }

    auto erase(const_iterator pos) { return this->get()->erase(pos); }
    auto erase(const_iterator first, const_iterator last) { return this->get()->erase(first, last); }
    size_t erase(const key_type& key) { return this->get()->erase(key); }

    // 哈希表特有操作
    copy_func_return_auto_with_noexcept(bucket_count);
    copy_func_return_auto_with_noexcept(max_bucket_count);
    size_t bucket_size(size_t n) const { return this->get()->bucket_size(n); }
    size_t bucket(const key_type& key) const { return this->get()->bucket(key); }

    float load_factor() const noexcept { return this->get()->load_factor(); }
    float max_load_factor() const noexcept { return this->get()->max_load_factor(); }
    void max_load_factor(float ml) { this->get()->max_load_factor(ml); }
    void rehash(size_t count) { this->get()->rehash(count); }
    void reserve(size_t count) { this->get()->reserve(count); }

    auto begin(size_t n) { return this->get()->begin(n); }
    auto end(size_t n) { return this->get()->end(n); }
    auto cbegin(size_t n) const { return this->get()->cbegin(n); }
    auto cend(size_t n) const { return this->get()->cend(n); }
};

#pragma endregion

#pragma region std::deque

template<typename _Elem, typename _Alloc>
class instance<std::deque<_Elem, _Alloc>, true> :public instance<std::deque<_Elem, _Alloc>, false>
{
public:
    using tag = std::deque<_Elem, _Alloc>;
    using _Mybase = instance<tag, false>;
    using _Element = _Elem;
    using _Allocator = _Alloc;
    using iterator = typename tag::iterator;
    using const_iterator = typename tag::const_iterator;
    using view_instance = instance<view_indicator<tag>, true>;

    instance() :_Mybase(new tag()) {}
    instance(tag* data) :_Mybase(data) {}
    instance(typename _Mybase::_shared& data) :_Mybase(data) {}
    instance(typename _Mybase::_shared&& data) :_Mybase(std::move(data)) {}
    explicit instance(tag&& data) :_Mybase(new tag(std::move(data))) {}
    template<typename... _Args>
    instance(_Args&&... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		_Mybase::operator=(other);
		return *this;
	}
    instance_move_operator(public) {}

    copy_func_return_auto_with_noexcept(begin);
    copy_func_return_auto_with_noexcept(end);
    copy_func_return_auto_with_noexcept(cbegin);
    copy_func_return_auto_with_noexcept(cend);
    copy_func_return_auto_with_noexcept(rbegin);
    copy_func_return_auto_with_noexcept(rend);
    copy_func_return_auto_with_noexcept(crbegin);
    copy_func_return_auto_with_noexcept(crend);
    copy_func_return_auto_with_noexcept(size);
    copy_func_return_auto_with_noexcept(empty);
    copy_func_return_declauto_with_noexcept(front);
    copy_func_return_declauto_with_noexcept(back);
    copy_func_with_noexcept(clear);
    copy_func(shrink_to_fit);

    template<typename... _Args>
    void push_back(_Args... args) const
    {
        this->get()->push_back(std::forward<_Args>(args)...);
    }

    template<typename... _Args>
    void push_front(_Args... args) const
    {
        this->get()->push_front(std::forward<_Args>(args)...);
    }

    void pop_back() const noexcept
    {
        this->get()->pop_back();
    }

    void pop_front() const noexcept
    {
        this->get()->pop_front();
    }

    void resize(size_t count) const
    {
        this->get()->resize(count);
    }

    template<typename _Val>
    void resize(size_t count, const _Val& value) const
    {
        this->get()->resize(count, value);
    }

    decltype(auto) at(size_t pos) const
    {
        return this->get()->at(pos);
    }

    decltype(auto) operator[](size_t pos) const
    {
        return (*this->get())[pos];
    }

    auto erase(const_iterator pos)
    {
        return this->get()->erase(pos);
    }

    auto erase(const_iterator first, const_iterator last)
    {
        return this->get()->erase(first, last);
    }

    template<typename... _Args>
    auto insert(const_iterator pos, _Args&&... args)
    {
        return this->get()->insert(pos, std::forward<_Args>(args)...);
    }
};
template<typename _Elem, typename _Alloc>
class instance<view_indicator<std::deque<_Elem, _Alloc>>, true> :public instance<std::deque<_Elem, _Alloc>, false>
{
    size_t _Myhead;
    size_t _Mytail;

public:
    using tag = std::deque<_Elem, _Alloc>;
    using _Mybase = instance<tag, false>;
    using _Element = _Elem;
    using _Allocator = _Alloc;
    using iterator = typename tag::iterator;
    using const_iterator = typename tag::const_iterator;
    using shared_from_instance = instance<tag, true>;

    instance() :_Mybase(nullptr), _Myhead(0), _Mytail(0) {}
    instance(typename _Mybase::_shared& data, size_t head, size_t tail)
        :_Mybase(data),
        _Myhead(std::min(head, data->size())),
        _Mytail(std::min(tail, data->size()))
    {
        if (_Myhead > _Mytail) std::swap(_Myhead, _Mytail);
    }

    instance(const instance& data)
        :_Mybase(data), _Myhead(data._Myhead), _Mytail(data._Mytail) {}

    instance_move_operator(public)
    {
        this->_Myhead = other._Myhead;
        this->_Mytail = other._Mytail;
    }

    auto begin() const noexcept { return this->get()->begin() + _Myhead; }
    auto end() const noexcept { return this->get()->begin() + _Mytail; }
    auto cbegin() const noexcept { return this->get()->cbegin() + _Myhead; }
    auto cend() const noexcept { return this->get()->cbegin() + _Mytail; }
    auto rbegin() const noexcept { return this->get()->rbegin() + (this->get()->size() - _Mytail); }
    auto rend() const noexcept { return this->get()->rbegin() + (this->get()->size() - _Myhead); }
    auto crbegin() const noexcept { return this->get()->crbegin() + (this->get()->size() - _Mytail); }
    auto crend() const noexcept { return this->get()->crbegin() + (this->get()->size() - _Myhead); }

    size_t size() const noexcept { return _Mytail - _Myhead; }
    bool data_empty() const noexcept { return _Mytail == _Myhead; }

    decltype(auto) at(size_t pos) const
    {
        if (pos >= this->size())
            throw std::out_of_range("deque view subscript out of range");
        return this->get()->at(_Myhead + pos);
    }

    decltype(auto) operator[](size_t pos) const
    {
        return this->get()->operator[](_Myhead + pos);
    }

    decltype(auto) front() const { return *begin(); }
    decltype(auto) back() const { return *std::prev(end()); }
};

#pragma endregion

#pragma region std::forward_list

template<typename _Elem, typename _Alloc>
class instance<std::forward_list<_Elem, _Alloc>, true> :public instance<std::forward_list<_Elem, _Alloc>, false>
{
public:
    using tag = std::forward_list<_Elem, _Alloc>;
    using _Mybase = instance<tag, false>;
    using _Element = _Elem;
    using _Allocator = _Alloc;
    using iterator = typename tag::iterator;
    using const_iterator = typename tag::const_iterator;
    using view_instance = instance<view_indicator<tag>, true>;

    instance() :_Mybase(new tag()) {}
    instance(tag* data) :_Mybase(data) {}
    instance(typename _Mybase::_shared& data) :_Mybase(data) {}
    instance(typename _Mybase::_shared&& data) :_Mybase(std::move(data)) {}
    explicit instance(tag&& data) :_Mybase(new tag(std::move(data))) {}
    template<typename... _Args>
    instance(_Args&&... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		_Mybase::operator=(other);
		return *this;
	}
    instance_move_operator(public) {}

    copy_func_return_auto_with_noexcept(begin);
    copy_func_return_auto_with_noexcept(end);
    copy_func_return_auto_with_noexcept(cbegin);
    copy_func_return_auto_with_noexcept(cend);
    copy_func_return_auto_with_noexcept(before_begin);
    copy_func_return_auto_with_noexcept(cbefore_begin);
    copy_func_return_auto_with_noexcept(empty);
    copy_func_return_declauto_with_noexcept(front);
    copy_func_with_noexcept(clear);
    copy_func(sort);
    copy_func(reverse);
    copy_func(unique);

	template<typename _Pr>
	auto unique(_Pr&& pr)
	{
		return this->get()->unique(std::forward<_Pr>(pr));
	}

	auto remove(const _Element& value) const
	{
		return this->get()->remove(value);
	}
	template<typename _Pr>
	auto remove(const _Pr& pr) const
	{
		return this->get()->remove_if(pr)
	}

	template<typename _Data>
	auto merge(_Data&& data) const
	{
		this->get()->merge(std::forward<_Data>(data));
	};

    template<typename... _Args>
    void push_front(_Args... args) const
    {
        this->get()->push_front(std::forward<_Args>(args)...);
    }

    void pop_front() const noexcept
    {
        this->get()->pop_front();
    }

    auto erase_after(const_iterator pos)
    {
        return this->get()->erase_after(pos);
    }

    auto erase_after(const_iterator first, const_iterator last)
    {
        return this->get()->erase_after(first, last);
    }

    template<typename... _Args>
    auto insert_after(const_iterator pos, _Args&&... args)
    {
        return this->get()->insert_after(pos, std::forward<_Args>(args)...);
    }

    template<typename _Pred>
    void remove_if(_Pred pred)
    {
        this->get()->remove_if(pred);
    }

    template<typename _BinaryPred>
    void unique(_BinaryPred pred)
    {
        this->get()->unique(pred);
    }

    template<typename _Compare>
    void sort(_Compare comp)
    {
        this->get()->sort(comp);
    }

    template<typename _Compare>
    void merge(tag& other, _Compare comp)
    {
        this->get()->merge(other, comp);
    }
};
template<typename _Elem, typename _Alloc>
class instance<view_indicator<std::forward_list<_Elem, _Alloc>>, true>
    :public instance<std::forward_list<_Elem, _Alloc>, false>
{
public:
    using tag = std::forward_list<_Elem, _Alloc>;
    using _Mybase = instance<tag, false>;
private:
	typename tag::iterator _Myhead;
	typename tag::iterator _Mytail;
	typename tag::iterator _Mybefore_head;
	size_t _Mysize;
public:
    using _Element = _Elem;
    using _Allocator = _Alloc;
    using iterator = typename tag::iterator;
    using const_iterator = typename tag::const_iterator;
    using shared_from_instance = instance<tag, true>;

    instance() :_Mybase(nullptr), _Mysize(0) {}
    instance(typename _Mybase::_shared& data, iterator head, iterator tail)
        :_Mybase(data), _Myhead(head), _Mytail(tail)
    {
        // 找到_Myhead前面的迭代器
        _Mybefore_head = this->get()->before_begin();
        while (std::next(_Mybefore_head) != _Myhead && std::next(_Mybefore_head) != this->get()->cend())
            ++_Mybefore_head;

        _Mysize = std::distance(_Myhead, _Mytail);
    }

    instance(const instance& data)
        :_Mybase(data), _Myhead(data._Myhead), _Mytail(data._Mytail),
        _Mybefore_head(data._Mybefore_head), _Mysize(data._Mysize) {}

    instance_move_operator(public)
    {
        this->_Myhead = other._Myhead;
        this->_Mytail = other._Mytail;
        this->_Mybefore_head = other._Mybefore_head;
        this->_Mysize = other._Mysize;
    }

    auto begin() const noexcept { return _Myhead; }
    auto end() const noexcept { return _Mytail; }
    auto cbegin() const noexcept { return _Myhead; }
    auto cend() const noexcept { return _Mytail; }
    auto before_begin() const noexcept { return _Mybefore_head; }
    auto cbefore_begin() const noexcept { return _Mybefore_head; }

    size_t size() const noexcept { return _Mysize; }
    bool data_empty() const noexcept { return _Mysize == 0; }

    decltype(auto) front() const { return *_Myhead; }

    // 支持在view范围内的after操作
	auto erase_after(const_iterator pos)
    {
        if (pos == _Mytail || std::next(pos) == _Mytail)
            return _Mytail;
        --_Mysize;
        return this->get()->erase_after(pos);
    }

    template<typename... _Args>
	auto insert_after(const_iterator pos, _Args&&... args)
    {
        if (pos == _Mytail)
            return _Mytail;
        ++_Mysize;
        return this->get()->insert_after(pos, std::forward<_Args>(args)...);
    }
};

#pragma endregion

#pragma region std::stack

template<typename _Ty, typename _Container>
class instance<std::stack<_Ty, _Container>, true> :public instance<std::stack<_Ty, _Container>, false>
{
public:
    using tag = std::stack<_Ty, _Container>;
    using _Mybase = instance<tag, false>;
    using value_type = _Ty;
    using container_type = _Container;
    using size_type = typename container_type::size_type;

    instance() :_Mybase(new tag()) {}
    instance(tag* data) :_Mybase(data) {}
    instance(typename _Mybase::_shared& data) :_Mybase(data) {}
    instance(typename _Mybase::_shared&& data) :_Mybase(std::move(data)) {}
    explicit instance(tag&& data) :_Mybase(new tag(std::move(data))) {}
    template<typename... _Args>
    instance(_Args&&... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		_Mybase::operator=(other);
		return *this;
	}
    instance_move_operator(public) {}

    copy_func_return_auto_with_noexcept(empty);
    copy_func_return_auto_with_noexcept(size);
    copy_func_return_declauto_with_noexcept(top);
    copy_func(pop);

    template<typename... _Args>
    void push(_Args&&... args) { this->get()->push(std::forward<_Args>(args)...); }

    template<typename... _Args>
    void emplace(_Args&&... args) { this->get()->emplace(std::forward<_Args>(args)...); }
};

#pragma endregion

#pragma region std::queue

template<typename _Ty, typename _Container>
class instance<std::queue<_Ty, _Container>, true> :public instance<std::queue<_Ty, _Container>, false>
{
public:
    using tag = std::queue<_Ty, _Container>;
    using _Mybase = instance<tag, false>;
    using value_type = _Ty;
    using container_type = _Container;
    using size_type = typename container_type::size_type;

    instance() :_Mybase(new tag()) {}
    instance(tag* data) :_Mybase(data) {}
    instance(typename _Mybase::_shared& data) :_Mybase(data) {}
    instance(typename _Mybase::_shared&& data) :_Mybase(std::move(data)) {}
    explicit instance(tag&& data) :_Mybase(new tag(std::move(data))) {}
    template<typename... _Args>
    instance(_Args&&... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		_Mybase::operator=(other);
		return *this;
	}
    instance_move_operator(public) {}

    copy_func_return_auto_with_noexcept(empty);
    copy_func_return_auto_with_noexcept(size);
    copy_func_return_declauto_with_noexcept(front);
    copy_func_return_declauto_with_noexcept(back);
    copy_func(pop);

    template<typename... _Args>
    void push(_Args&&... args) { this->get()->push(std::forward<_Args>(args)...); }

    template<typename... _Args>
    void emplace(_Args&&... args) { this->get()->emplace(std::forward<_Args>(args)...); }
};

#pragma endregion

#pragma region std::priority_queue

template<typename _Ty, typename _Container, typename _Compare>
class instance<std::priority_queue<_Ty, _Container, _Compare>, true>
    :public instance<std::priority_queue<_Ty, _Container, _Compare>, false>
{
public:
    using tag = std::priority_queue<_Ty, _Container, _Compare>;
    using _Mybase = instance<tag, false>;
    using value_type = _Ty;
    using container_type = _Container;
    using size_type = typename container_type::size_type;
    using value_compare = _Compare;

    instance() :_Mybase(new tag()) {}
    instance(tag* data) :_Mybase(data) {}
    instance(typename _Mybase::_shared& data) :_Mybase(data) {}
    instance(typename _Mybase::_shared&& data) :_Mybase(std::move(data)) {}
    explicit instance(tag&& data) :_Mybase(new tag(std::move(data))) {}
    template<typename... _Args>
    instance(_Args&&... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
	auto& operator=(const instance& other) noexcept
	{
		_Mybase::operator=(other);
		return *this;
	}
    instance_move_operator(public) {}

    copy_func_return_auto_with_noexcept(empty);
    copy_func_return_auto_with_noexcept(size);
    copy_func_return_declauto_with_noexcept(top);
    copy_func(pop);

    template<typename... _Args>
    void push(_Args&&... args) { this->get()->push(std::forward<_Args>(args)...); }

    template<typename... _Args>
    void emplace(_Args&&... args) { this->get()->emplace(std::forward<_Args>(args)...); }
};

#pragma endregion

// 常用类型的显式实例化
#pragma region Explicit Instantiations

// std::vector 实例化和别名
template class instance<std::vector<int>, true>;
template class instance<std::vector<double>, true>;
template class instance<std::vector<std::string>, true>;
template class instance<std::vector<char>, true>;
template class instance<view_indicator<std::vector<int>>, true>;
template class instance<view_indicator<std::vector<double>>, true>;
template class instance<view_indicator<std::vector<std::string>>, true>;
template class instance<view_indicator<std::vector<char>>, true>;

// std::list 实例化
template class instance<std::list<int>, true>;
template class instance<std::list<double>, true>;
template class instance<std::list<std::string>, true>;
template class instance<view_indicator<std::list<int>>, true>;
template class instance<view_indicator<std::list<double>>, true>;
template class instance<view_indicator<std::list<std::string>>, true>;

// std::map 实例化
template class instance<std::map<std::string, int>, true>;
template class instance<std::map<std::string, std::string>, true>;
template class instance<std::map<int, std::string>, true>;
template class instance<std::map<int, int>, true>;
template class instance<std::map<std::string, instance<std::string, true>>, true>;
template class instance<std::map<int, instance<std::string, true>>, true>;
template class instance<view_indicator<std::map<std::string, int>>, true>;
template class instance<view_indicator<std::map<std::string, std::string>>, true>;
template class instance<view_indicator<std::map<int, std::string>>, true>;
template class instance<view_indicator<std::map<int, int>>, true>;
template class instance<view_indicator<std::map<std::string, instance<std::string, true>>>, true>;
template class instance<view_indicator<std::map<int, instance<std::string, true>>>, true>;

// std::basic_string 实例化
template class instance<std::string, true>;
template class instance<std::wstring, true>;
template class instance<view_indicator<std::string>, true>;
template class instance<view_indicator<std::wstring>, true>;

// std::set 实例化
template class instance<std::set<int>, true>;
template class instance<std::set<double>, true>;
template class instance<std::set<std::string>, true>;
template class instance<std::set<instance<std::string, true>>, true>;
template class instance<view_indicator<std::set<int>>, true>;
template class instance<view_indicator<std::set<double>>, true>;
template class instance<view_indicator<std::set<std::string>>, true>;
template class instance<view_indicator<std::set<instance<std::string, true>>>, true>;

// std::unordered_map 实例化
template class instance<std::unordered_map<std::string, int>, true>;
template class instance<std::unordered_map<std::string, std::string>, true>;
template class instance<std::unordered_map<int, std::string>, true>;
template class instance<std::unordered_map<int, int>, true>;
template class instance<std::unordered_map<std::string, instance<std::string, true>>, true>;
template class instance<std::unordered_map<int, instance<std::string, true>>, true>;
template class instance<view_indicator<std::unordered_map<std::string, int>>, true>;
template class instance<view_indicator<std::unordered_map<std::string, std::string>>, true>;
template class instance<view_indicator<std::unordered_map<int, std::string>>, true>;
template class instance<view_indicator<std::unordered_map<int, int>>, true>;
template class instance<view_indicator<std::unordered_map<std::string, instance<std::string, true>>>, true>;
template class instance<view_indicator<std::unordered_map<int, instance<std::string, true>>>, true>;

// std::unordered_set 实例化
template class instance<std::unordered_set<int>, true>;
template class instance<std::unordered_set<double>, true>;
template class instance<std::unordered_set<std::string>, true>;
template class instance<view_indicator<std::unordered_set<int>>, true>;
template class instance<view_indicator<std::unordered_set<double>>, true>;
template class instance<view_indicator<std::unordered_set<std::string>>, true>;

// std::deque 实例化
template class instance<std::deque<int>, true>;
template class instance<std::deque<double>, true>;
template class instance<std::deque<std::string>, true>;
template class instance<std::deque<instance<std::string, true>>, true>;
template class instance<view_indicator<std::deque<int>>, true>;
template class instance<view_indicator<std::deque<double>>, true>;
template class instance<view_indicator<std::deque<std::string>>, true>;
template class instance<view_indicator<std::deque<instance<std::string, true>>>, true>;

// std::forward_list 实例化
template class instance<std::forward_list<int>, true>;
template class instance<std::forward_list<double>, true>;
template class instance<std::forward_list<std::string>, true>;
template class instance<std::forward_list<instance<std::string, true>>, true>;
template class instance<view_indicator<std::forward_list<int>>, true>;
template class instance<view_indicator<std::forward_list<double>>, true>;
template class instance<view_indicator<std::forward_list<std::string>>, true>;
template class instance<view_indicator<std::forward_list<instance<std::string, true>>>, true>;

// std::stack 实例化
template class instance<std::stack<int>, true>;
template class instance<std::stack<double>, true>;
template class instance<std::stack<std::string>, true>;
template class instance<std::stack<instance<std::string, true>>, true>;
template class instance<std::stack<int, std::deque<int>>, true>;
template class instance<std::stack<int, std::vector<int>>, true>;

// std::queue 实例化
template class instance<std::queue<int>, true>;
template class instance<std::queue<double>, true>;
template class instance<std::queue<std::string>, true>;
template class instance<std::queue<instance<std::string, true>>, true>;
template class instance<std::queue<int, std::deque<int>>, true>;
template class instance<std::queue<int, std::list<int>>, true>;

// std::priority_queue 实例化
template class instance<std::priority_queue<int>, true>;
template class instance<std::priority_queue<double>, true>;
template class instance<std::priority_queue<std::string>, true>;
template class instance<std::priority_queue<instance<std::string, true>>, true>;
template class instance<std::priority_queue<int, std::vector<int>, std::greater<int>>, true>;

#pragma endregion

// return:
//		返回指向原对象的view(如果传入的是一个view, 那么不继承其属性)
template<typename _Inside>
instance<view_indicator<_Inside>,true> make_view(instance<_Inside,false> data, int head, int tail)
{
	return instance<view_indicator<_Inside>, true>(data, head, tail);
}

// return:
//		修改前的size-修改后的size, 指示了修改对view所容纳的范围的更改
template<typename _Inside>
int move_view(instance<view_indicator<_Inside>,true>& data, int offset)
{
	int size = data.size();
	data._Myhead = std::max<size_t>(0, data._Myhead + offset);
	data._Mytail = std::min<size_t>(data->size(), data._Mytail + offset);
	return data.size() - size;
}
// return:
//		返回修改后的view
template<typename _Inside>
auto move_view(instance<view_indicator<_Inside>, true>&& data, int offset)
{
	data._Myhead = std::max(0, data._Myhead + offset);
	data._Mytail = std::min(data->size(), data._Mytail + offset);
	return data;
}

#endif // !CONVENTION_KIT_STD_INSTANCE_H
