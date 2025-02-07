#ifndef __FILE_CONVENTION_STD_INSTANCE
#define __FILE_CONVENTION_STD_INSTANCE

#include "Convention/Interface.h"


template<typename _Sequence>
struct sequence_indicator
{
	using tag = _Sequence;
	using iterator = typename tag::iterator;
	constexpr static bool value = void_t<iterator>;
};
template<typename _Ty>
struct view_indicator
{
	using tag = _Ty;
	constexpr static bool value = true;
};

namespace convention_kit
{
	template<typename _Ty>
	using view = view_indicator<_Ty>;
}

template<typename _Elem, typename _Alloc>
class instance<std::vector<_Elem, _Alloc>, true>;
template<typename _Elem, typename _Alloc>
class instance<view_indicator<std::vector<_Elem, _Alloc>>, true>;

template<typename _T_Ins,typename... _Args>
auto make_view(instance<_T_Ins> data, _Args&&...args)
{
	return instance<view_indicator<_T_Ins>>(data, std::forward<_Args>(args)...);
}
template<typename _T_Ins, typename... _Args>
auto make_view(instance<view_indicator<_T_Ins>> data, _Args&&...args)
{
	return instance<view_indicator<_T_Ins>>(data, std::forward<_Args>(args)...);
}

#define copy_func(name) auto name() const {this->get()->name();}
#define copy_func_with_noexcept(name) auto name() const noexcept {this->get()->name();}
#define copy_func_return_auto(name) auto name() const {return this->get()->name();}
#define copy_func_return_auto_with_noexcept(name) auto name() const noexcept {return this->get()->name();}
#define copy_func_return_declauto(name) decltype(auto) name() const {return this->get()->name();}
#define copy_func_return_declauto_with_noexcept(name) decltype(auto) name() const noexcept {return this->get()->name();}

#pragma region std::vector

template<typename _Elem,typename _Alloc>
class instance<std::vector<_Elem, _Alloc>, true> :public instance<std::vector<_Elem, _Alloc>, false>
{
public:
	using _Mybase = instance<std::vector<_Elem, _Alloc>,false>;
	using _Element = _Elem;
	using _Allocator = _Alloc;
	using tag = std::vector<_Elem, _Alloc>;
	using iterator = typename tag::iterator;
	using const_iterator = typename tag::const_iterator;
	using view_instance = instance<view_indicator<tag>, true>;
	instance() :_Mybase(new tag()) {}
	instance(nullptr_t) :_Mybase(nullptr) {}
	instance(tag* data) :_Mybase(data) {}
	instance(typename _Mybase::_shared& data) :_Mybase(data) {}
	instance(typename _Mybase::_shared&& data) :_Mybase(std::move(data)) {}
	explicit instance(tag&& data) :_Mybase(new tag(std::move(data))) {}
	explicit instance(std::initializer_list<int> data) :_Mybase(new tag(data)) {}
	template<typename... _Args>
	instance(_Args... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
	instance(instance& data) :_Mybase(data) {}
	instance_move_operator(public) {}

	copy_func_return_auto_with_noexcept(begin);
	copy_func_return_auto_with_noexcept(end);
	copy_func_return_auto_with_noexcept(cbegin);
	copy_func_return_auto_with_noexcept(cend);
	copy_func_return_auto_with_noexcept(rbegin);
	copy_func_return_auto_with_noexcept(rend);
	copy_func_return_auto_with_noexcept(crbegin);
	copy_func_return_auto_with_noexcept(crend);
	void reserve(size_t size) const
	{
		this->get()->reserve(size);
	}
	void resize(size_t size) const
	{
		this->get()->resize(size);
	}
	template<typename... _Args>
	void push_back(_Args... args) const
	{
		this->get()->push_back(std::forward<_Args>(args)...);
	}
	void pop_back() const noexcept
	{
		return this->get()->pop_back();
	}
	copy_func_return_auto_with_noexcept(size);
	copy_func_with_noexcept(clear);
	decltype(auto) at(int index) const
	{
		if (index < 0)
			index = this->size() + index;
		return this->get()->at(index);
	}
	copy_func_return_auto_with_noexcept(front);
	copy_func_return_auto_with_noexcept(back);
	copy_func_return_auto_with_noexcept(capacity);
	auto data() const
	{
		return this->get()->data();
	}
	const auto cdata() const
	{
		return this->get()->data();
	}
	bool data_empty() const noexcept
	{
		return this->get()->empty();
	}
	auto erase(const_iterator iter) const
	{
		return this->get()->erase(iter);
	}
	auto erase(const_iterator head, const_iterator end) const
	{
		return this->get()->erase(head, end);
	}
	size_t erase(const _Elem& value, size_t ignore = 0, size_t countdown = static_cast<size_t>(-1)) const
	{
		int counter = ignore;
		counter = -counter;
		auto start = this->begin();
		while (counter < countdown)
		{
			auto iter = std::find(start, this->end(), value);
			if (iter != this->end())
			{
				if (counter >= 0)
				{
					this->erase(iter);
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
	template<typename... _Args>
	auto insert(const_iterator front_iter, _Args&&... args)
	{
		return this->get()->insert(front_iter, std::forward<_Args>(args)...);
	}
	template<typename... _Args>
	auto insert(size_t front_pos, _Args&&... args)
	{
		return this->get()->insert(this->begin() + front_pos, std::forward<_Args>(args)...);
	}
	decltype(auto) operator[](int index) const
	{
		return this->at(index);
	}
};
template<typename _Elem, typename _Alloc>
class instance<view_indicator<std::vector<_Elem, _Alloc>>, true> :public instance<std::vector<_Elem, _Alloc>, false>
{
private:
	size_t _Myhead, _Mytail;
public:
	using _Mybase = instance<std::vector<_Elem, _Alloc>, false>;
	using _Element = _Elem;
	using _Allocator = _Alloc;
	using tag = std::vector<_Elem, _Alloc>;
	using iterator = typename tag::iterator;
	using const_iterator = typename tag::const_iterator;
	using shared_from_instance = instance<std::vector<_Elem, _Alloc>, true>;
	instance() :_Mybase(nullptr) {}
	instance(typename _Mybase::_shared& data, int head, int tail)
		:_Mybase(data),
		_Myhead(head < 0 ? std::max<size_t>(0, data->size() - head) : std::max<size_t>(0, head)),
		_Mytail(tail < 0 ? std::max<size_t>(0, data->size() - tail) : std::min<size_t>(data->size(), tail))
	{
		_Myhead = std::min(_Myhead, data->size());
		_Mytail = std::min(_Mytail, data->size());
		if (_Myhead > _Mytail)
			std::swap(_Myhead, _Mytail);
	}
	instance(const instance& data, int head, int tail)
		:instance(static_cast<typename _Mybase::_shared&>(data), head + data._Myhead, tail + data._Mytail) {}
	instance_move_operator(public)
	{
		this->_Myhead = other._Myhead;
		this->_Mytail = other._Mytail;
	}

	void rebind(typename _Mybase::_shared& data, int head, int tail)
	{
		_Mybase::operator=(data);
		_Myhead = (head < 0 ? std::max<size_t>(0, data->size() - head) : std::max<size_t>(0, head));
		_Mytail = (tail < 0 ? std::max<size_t>(0, data->size() - tail) : std::min<size_t>(data->size(), tail));
		_Myhead = std::min(_Myhead, data->size());
		_Mytail = std::min(_Mytail, data->size());
		if (_Myhead > _Mytail)
			std::swap(_Myhead, _Mytail);
	}
	
	auto begin() const noexcept
	{
		return this->get()->begin() + _Myhead;
	}
	auto end() const noexcept
	{
		return this->get()->begin() + _Mytail;
	}
	auto cbegin() const noexcept
	{
		return this->get()->cbegin() + _Myhead;
	}
	auto cend() const noexcept
	{
		return this->get()->cend() + _Mytail;
	}
	auto rbegin() const noexcept
	{
		return this->get()->rbegin() + (this->get()->size() - _Mytail);
	}
	auto rend() const noexcept
	{
		return this->get()->rbegin() + (this->get()->size() - _Myhead);
	}
	auto crbegin() const noexcept
	{
		return this->get()->crbegin() + (this->get()->size() - _Mytail);
	}
	auto crend() const noexcept
	{
		return this->get()->crbegin() + (this->get()->size() - _Myhead);
	}
	size_t size() const noexcept
	{
		return std::max<size_t>(0, _Mytail - _Myhead);
	}
	void clear() const noexcept
	{
		_Mytail = _Myhead;
		this->get()->erase(this->begin(), this->end());
	}
	decltype(auto) at(int index) const
	{
		if (index < 0)
			index = this->size() + index;
		index += _Myhead;
		if (index < _Mytail)
			return this->get()->at(_Myhead + index);
		throw std::overflow_error();
	}
	const auto& front() const
	{
		return *(this->begin())
	}
	const auto& back() const
	{
		return *(--this->end());
	}
	auto data() const
	{
		return this->get()->data() + _Myhead;
	}
	const auto cdata() const
	{
		return this->get()->data() + _Myhead;
	}
	bool data_empty() const noexcept
	{
		return _Mytail - _Myhead == 0;
	}
	auto erase(const_iterator iter) const
	{
		_Mytail--;
		return this->get()->erase(iter);
	}
	auto erase(const_iterator head, const_iterator end) const
	{
		_Mytail -= std::distance(head, end);
		return this->get()->erase(head, end);
	}
	size_t erase(const _Elem& value, size_t ignore = 0, size_t countdown = static_cast<size_t>(-1)) const
	{
		int counter = ignore;
		counter = -counter;
		auto start = this->begin();
		while (counter < countdown)
		{
			auto iter = std::find(start, this->end(), value);
			if (iter != this->end())
			{
				if (counter >= 0)
				{
					this->erase(iter);
				}
				else
				{
					start = ++iter;
				}
				counter++;
			}
			else break;
		}
		_Mytail = std::max(_Myhead, _Mytail - std::max(0, counter));
		return counter;
	}
	template<typename... _Args>
	auto insert(const_iterator front_iter, _Args&&... args)
	{
		_Mytail++;
		return this->get()->insert(front_iter, std::forward<_Args>(args)...);
	}
	template<typename... _Args>
	auto insert(size_t front_pos, _Args&&... args)
	{
		_Mytail++;
		return this->get()->insert(this->begin() + front_pos, std::forward<_Args>(args)...);
	}
	decltype(auto) operator[](int index) const
	{
		return this->at(index);
	}
};

template<typename _Elem, typename _Alloc>
std::ostream& operator<<(std::ostream& os,const std::vector<_Elem, _Alloc>& data)
{
	for (auto&& i : data)
	{
		os << i << ",";
	}
	return os;
}
template<typename _Elem, typename _Alloc>
std::ostream& operator<<(std::ostream& os, const typename instance<std::vector<_Elem, _Alloc>, false>& data)
{
	for (auto&& i : *data)
	{
		os << i << ",";
	}
	return os;
}
template<typename _Elem, typename _Alloc>
std::ostream& operator<<(std::ostream& os, const typename instance<view_indicator<std::vector<_Elem, _Alloc>>, true>& data)
{
	for (auto&& i : data)
	{
		os << i << ",";
	}
	return os;
}

template<typename _Elem, typename _Alloc>
std::vector<instance<view_indicator<std::vector<_Elem, _Alloc>>, true>> make_matrix(
	instance<std::vector<_Elem, _Alloc>, true> data
)
{
	size_t size = data.size();
	double logsize = std::log2(size);
	size_t forlinesize = std::floor(logsize);
	size_t linesize = std::ceil(logsize);
	std::vector<instance<view_indicator<std::vector<_Elem, _Alloc>>, true>> result(linesize);
	for (size_t i = 0, e = forlinesize; i < e; i++)
	{
		result[i].rebind(data, i * forlinesize, (i + 1) * forlinesize);
	}
	if (forlinesize != linesize)
	{
		result[forlinesize].rebind(data, forlinesize * forlinesize, data.size());
	}
	return result;
}
template<typename _Elem, typename _Alloc>
std::vector<instance<view_indicator<std::vector<_Elem, _Alloc>>, true>> make_matrix(
	instance<view_indicator<std::vector<_Elem, _Alloc>>, true> data
)
{
	size_t size = data.size();
	double logsize = std::log2(size);
	size_t forlinesize = std::floor(logsize);
	size_t linesize = std::ceil(logsize);
	std::vector<instance<view_indicator<std::vector<_Elem, _Alloc>>, true>> result(linesize);
	for (size_t i = 0, e = forlinesize; i < e; i++)
	{
		result[i].rebind(data, i * forlinesize, (i + 1) * forlinesize);
	}
	if (forlinesize != linesize)
	{
		result[forlinesize].rebind(data, forlinesize * forlinesize, data.size());
	}
	return result;
}

template<typename _Elem, typename _Alloc,typename _OutsideAlloc>
std::ostream& operator<<(std::ostream& os, const std::vector<instance<view_indicator<std::vector<_Elem, _Alloc>>, true>, _OutsideAlloc>& data)
{
	os << "[\n";
	for (auto&& i : data)
	{
		os << i << "\n";
	}
	os << "]";
	return os;
}

#pragma endregion

#endif // !__FILE_CONVENTION_STD_INSTANCE
