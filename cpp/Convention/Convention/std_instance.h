#ifndef __FILE_CONVENTION_STD_INSTANCE
#define __FILE_CONVENTION_STD_INSTANCE

#include "Convention/Interface.h"


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
	copy_func_return_declauto_with_noexcept(front);
	copy_func_return_declauto_with_noexcept(back);
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
	copy_func(shrink_to_fit);
};
template<typename _Elem, typename _Alloc>
class instance<view_indicator<std::vector<_Elem, _Alloc>>, true> :public instance<std::vector<_Elem, _Alloc>, false>
{
private:
	size_t _Myhead, _Mytail;
public:
	template<typename _Inside>
	friend int move_view(instance<view_indicator<_Inside>, true>& data, int offset);
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
		_Mytail(tail < 0 ? std::max<size_t>(0, data->size() + tail) : std::min<size_t>(data->size(), tail))
	{
		_Myhead = std::min(_Myhead, data->size());
		_Mytail = std::min(_Mytail, data->size());
		if (_Myhead > _Mytail)
			std::swap(_Myhead, _Mytail);
	}
	instance(const instance& data) :_Mybase(data), _Myhead(data._Myhead), _Mytail(data._Mytail) {}
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
	constexpr auto get_head() const noexcept
	{
		return _Myhead;
	}
	constexpr auto get_tail() const noexcept
	{
		return _Mytail;
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
	for (auto i=data.begin(),e=data.end();i!=e;i++)
	{
		if (i != data.begin())
		{
			if constexpr (std::is_same_v<char, _Elem> || std::is_same_v<wchar_t, _Elem>)
				os << ',';
			else if constexpr (std::is_same_v<bool, _Elem>)
				os << ' ';
			else
				os << "\t, ";
		}
		os << *i;
	}
	return os;
}
template<typename _Elem, typename _Alloc>
std::ostream& operator<<(std::ostream& os, const typename instance<std::vector<_Elem, _Alloc>, false>& data)
{
	for (auto i = data.begin(), e = data.end(); i != e; i++)
	{
		if (i != data.begin())
		{
			if constexpr (std::is_same_v<char, _Elem> || std::is_same_v<wchar_t, _Elem>)
				os << ',';
			else if constexpr (std::is_same_v<bool, _Elem>)
				os << ' ';
			else
				os << "\t, ";
		}
		os << *i;
	}
	return os;
}
template<typename _Elem, typename _Alloc>
std::ostream& operator<<(std::ostream& os, const typename instance<view_indicator<std::vector<_Elem, _Alloc>>, true>& data)
{
	for (auto i = data.begin(), e = data.end(); i != e; i++)
	{
		if (i != data.begin())
		{
			if constexpr (std::is_same_v<char, _Elem> || std::is_same_v<wchar_t, _Elem>)
				os << ',';
			else if constexpr (std::is_same_v<bool, _Elem>)
				os << ' ';
			else
				os << "\t, ";
		}
		os << *i;
	}
	return os;
}

template<typename _Elem, typename _Alloc>
std::vector<instance<view_indicator<std::vector<_Elem, _Alloc>>, true>> make_matrix(
	instance<std::vector<_Elem, _Alloc>, true> data,
	size_t row_size,
	size_t col_size,
	size_t maxend
)
{
	using _Inside = instance<view_indicator<std::vector<_Elem, _Alloc>>, true>;
	std::vector<_Inside> result(row_size);
	for (size_t i = 0, e = row_size; i < e; i++)
	{
		result[i].rebind(data, i * col_size, std::min(maxend, (i + 1) * col_size));
	}
	if (row_size * col_size < maxend)
	{
		result.push_back(_Inside(data, row_size * col_size, maxend));
	}
	return result;
}
template<typename _Elem, typename _Alloc>
std::vector<instance<view_indicator<std::vector<_Elem, _Alloc>>, true>> make_matrix(
	instance<view_indicator<std::vector<_Elem, _Alloc>>, true> data,
	size_t row_size,
	size_t col_size,
	size_t maxend
)
{
	using _Inside = instance<view_indicator<std::vector<_Elem, _Alloc>>, true>;
	std::vector<_Inside> result(row_size);
	for (size_t i = 0, e = row_size; i < e; i++)
	{
		result[i].rebind(data, i * col_size, std::min(maxend, (i + 1) * col_size));
	}
	if (row_size * col_size < maxend)
	{
		result.push_back(_Inside(data, row_size * col_size, maxend));
	}
	return result;
}
template<typename _Elem, typename _Alloc>
std::vector<instance<view_indicator<std::vector<_Elem, _Alloc>>, true>> make_matrix(
	instance<std::vector<_Elem, _Alloc>, true> data
)
{
	size_t size = data.size();
	double logsize = std::log2(size);
	return make_matrix(data, logsize, logsize, size);
}
template<typename _Elem, typename _Alloc>
std::vector<instance<view_indicator<std::vector<_Elem, _Alloc>>, true>> make_matrix(
	instance<view_indicator<std::vector<_Elem, _Alloc>>, true> data
)
{
	size_t size = data.size();
	double logsize = std::log2(size);
	return make_matrix(data, logsize, logsize, size);
}

template<typename _Elem, typename _Alloc,typename _OutsideAlloc>
std::ostream& operator<<(std::ostream& os, const std::vector<instance<view_indicator<std::vector<_Elem, _Alloc>>, true>, _OutsideAlloc>& data)
{
	os << "[\n";
	for (auto&& i : data)
	{
		if (i.data_empty())
			os << "<empty>,\n";
		else
			os << i << ",\n";
	}
	os << "]";
	return os;
}

#pragma endregion

#pragma region std::string

template<typename _Iter1, typename _Iter2,typename _VecMatrix>
size_t levenshtein_distance_with_custom_buffer(
	_Iter1 first_begin, _Iter1 first_end,
	_Iter2 second_begin, _Iter2 second_end,
	_VecMatrix& buffer_matrix, bool matrix_init
)
{
	size_t first_length = std::distance(first_begin, first_end);
	size_t second_length = std::distance(second_begin, second_end);
	if (matrix_init)
	{
		buffer_matrix.resize(first_length + 1);
		for (auto&& i : buffer_matrix)
			i.resize(second_length + 1, 0);
	}
	for (int k = 0; k <= first_length; k++) buffer_matrix[k][0] = k;
	for (int k = 0; k <= second_length; k++) buffer_matrix[0][k] = k;
	for (int i = 1; i <= first_length; i++)
	{
		for (int j = 1; j <= second_length; j++)
		{
			int cost = (*(first_begin + i - 1) == *(second_begin + j - 1)) ? 1 : 0;
			buffer_matrix[i][j] = std::min({
				buffer_matrix[i - 1][j - 1] + cost,
				buffer_matrix[i][j - 1] + 1,
				buffer_matrix[i - 1][j] + 1
				});
		}
	}
	return buffer_matrix[first_length][second_length];
}
template<typename _Iter1,typename _Iter2>
size_t levenshtein_distance(
	_Iter1 first_begin, _Iter1 first_end,
	_Iter2 second_begin, _Iter2 second_end
)
{
	std::vector<std::vector<int>> matrix;
	return levenshtein_distance_with_custom_buffer(first_begin, first_end, second_begin, second_end, matrix, true);
}
template<typename _Pr, typename _Iter1, typename _Iter2 >
int fuzzy_distance(
	_Iter1 first_begin, _Iter1 first_end,
	_Iter2 second_begin, _Iter2 second_end,
	_Pr _DisCounter
)
{
	return _DisCounter(first_begin, first_end, second_begin, second_end);
}
template< typename _Pr, typename... _DistanceCounters, typename _Iter1, typename _Iter2>
int fuzzy_distance(
	_Iter1 first_begin, _Iter1 first_end,
	_Iter2 second_begin, _Iter2 second_end,
	_Pr _DisCounter,
	_DistanceCounters... _DisCounters
)
{
	return
		_DisCounter(first_begin, first_end, second_begin, second_end) +
		fuzzy_distance(first_begin, first_end, second_begin, second_end, _DisCounters...);
}
template<typename _Elem, typename _Traits, typename _Alloc>
class instance<std::basic_string<_Elem, _Traits, _Alloc>, true> :public instance<std::basic_string<_Elem, _Traits, _Alloc>, false>
{
public:
	using tag = std::basic_string<_Elem, _Traits, _Alloc>;
	using _Mybase = instance<tag, false>;
	using _Element = _Elem;
	using _My_Traits = _Traits;
	using _Allocator = _Alloc;
	using iterator = typename tag::iterator;
	using const_iterator = typename tag::const_iterator;
	using view_instance = instance<view_indicator<tag>, true>;
	instance() :_Mybase(new tag()) {}
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
	template<typename... _Args>
	instance& append(_Args&&... args)
	{
		this->get()->append(std::forward<_Args>(args)...);
		return *this;
	}
	instance& append(const typename _Mybase::_shared& data)
	{
		this->get()->append(*data);
		return *this;
	}
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
	copy_func_return_declauto_with_noexcept(front);
	copy_func_return_declauto_with_noexcept(back);
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
	auto deep_copy() const
	{
		return instance(**this);
	}
	auto shallow_copy() const noexcept
	{
		return instance(*this);
	}
	copy_func_return_auto_with_noexcept(c_str);
	auto substr(int offset = 0, int Count = static_cast<size_t>(-1))
	{
		if (offset < 0)
			offset = this->size() - offset;
		if (Count < 0)
			Count = this->size() - Count;
		return this->get()->substr(offset, Count);
	}
	template<typename _Right>
	bool start_with(const _Right& str) const noexcept
	{
		return this->size() >= str.size() && _Traits::compare(this->data(), str.data(), str.size()) == 0;
	}
	bool start_with(const _Elem* ptr) const noexcept
	{
		return start_with(tag(ptr));
	}
	template<typename _Right>
	bool end_with(const _Right& str) const noexcept
	{
		return this->size() >= str.size() && _Traits::compare(this->data() + this->size() - str.size(), str.data(), str.size()) == 0;
	}
	bool end_with(const _Elem* ptr) const noexcept
	{
		return end_with(tag(ptr));
	}
	template<typename _Right>
	bool contains(const _Right& str) const noexcept
	{
		if (this->size() < str.size())
			return false;
		for (int offset = 0, end = this->size() - str.size(); offset < end; offset++)
		{
			if (_Traits::compare(this->data() + offset, str.data(), str.size()) == 0)
				return true;
		}
		return false;
	}
	bool contains(const _Elem* ptr) const noexcept
	{
		return contains(tag(ptr));
	}
	template<typename _Right>
	int compare(const _Right& str) const noexcept
	{
		return _Traits::compare(this->data(), str.data(), std::min(this->size(), str.size()));
	}
	auto compare(const _Elem* ptr) const noexcept
	{
		return contains_compare(tag(ptr));
	}
	int compare(const typename _Mybase::_shared& right)
	{
		return this->get()->compare(*right);
	}
	copy_func_return_auto_with_noexcept(length);
	copy_func(shrink_to_fit);
	template<typename _Target>
	size_t edit_distance(const _Target& target) const
	{
		return levenshtein_distance(this->begin(), this->end(), target.cbegin(), target.cend());
	}
	template<size_t length>
	size_t edit_distance(const _Elem target[length]) const
	{
		return levenshtein_distance(this->begin(), this->end(), &target[0], &target[length - 1]);
	}
	size_t edit_distance(_In_ const _Elem* target) const
	{
		return edit_distance(tag(target));
	}
	template<typename _Right>
	int first(const _Right& str) const noexcept
	{
		if (this->size() < str.size())
			return -1;
		for (int offset = 0, end = this->size() - str.size(); offset < end; offset++)
		{
			if (_Traits::compare(this->data() + offset, str.data(), str.size()) == 0)
				return offset;
		}
		return -1;
	}
	int first(const _Elem& ch) const noexcept
	{
		if (this->size() != 0)
			for (int offset = 0, end = this->size(); offset < end; offset++)
				if (this->at(offset) == ch)
					return offset;
		return -1;
	}
	int first(_In_ const _Elem* ptr) const noexcept
	{
		return first(tag(ptr));
	}
	template<typename _Right>
	int last(const _Right& str) const noexcept
	{
		if (this->size() < str.size())
			return -1;
		for (int offset = this->size() - str.size() - 1, end = 0; offset >= end; offset--)
		{
			if (_Traits::compare(this->data() + offset, str.data(), str.size()) == 0)
				return offset;
		}
		return -1;
	}
	int last(const _Elem& ch) const noexcept
	{
		if (this->size() != 0)
			for (int offset = this->size() - 1, end = 0; offset >= end; offset--)
				if (this->at(offset) == ch)
					return offset;
		return -1;
	}
	int last(_In_ const _Elem* ptr) const noexcept
	{
		return last(tag(ptr));
	}
	template<typename _Param>
	tag join(_Param&& arg)
	{
		return tag(std::forward(arg));
	}
	template<typename _First, typename... _Params>
	tag join(_First&& first, _Params&&... args)
	{
		return tag(std::forward<_First>(first)) + join(std::forward<_Params>(args)...);
	}
	template<typename... Args>
	auto format_s(void* buffer, size_t buffer_size, Args&&... args)
	{
		if constexpr (std::is_same_v<_Elem, char>)
		{
			sprintf_s((char*)buffer, buffer_size, **this, std::forward<Args>(args)...);
		}
		else if constexpr (std::is_same_v<_Elem, wchar_t>)
		{
			swprintf_s((wchar_t*)buffer, buffer_size, **this, std::forward<Args>(args)...);
		}
	}
	template<typename... Args>
	auto format(Args&&... args)
	{
		if constexpr (std::is_same_v<_Elem, char>)
		{
			std::string result(1024, 0);
			sprintf_s(result.data(), 1024, **this, std::forward<Args>(args)...);
			return result;
		}
		else if constexpr (std::is_same_v<_Elem, wchar_t>)
		{
			std::wstring result(1024, 0);
			swprintf_s(result.data(), 1024, **this, std::forward<Args>(args)...);
			return result;
		}
	}


	virtual std::string ToString() const noexcept override
	{
		std::string str(sizeof(char) * this->size(), 0);
		::memmove(str.data(), this->data(), this->size());
		return str;
	}

};
template<typename _Elem, typename _Traits, typename _Alloc>
class instance<view_indicator<std::basic_string<_Elem, _Traits, _Alloc>>, true> :public instance<std::basic_string<_Elem, _Traits, _Alloc>, false>
{
	size_t _Myhead, _Mytail;
public:
	using tag = std::basic_string<_Elem, _Traits, _Alloc>;
	using _Mybase = instance<tag, false>;
	using _Element = _Elem;
	using _My_Traits = _Traits;
	using _Allocator = _Alloc;
	using iterator = typename tag::iterator;
	using const_iterator = typename tag::const_iterator;
	using shared_from_instance = instance<tag, true>;
	instance() :_Mybase(nullptr) {}
	instance(typename _Mybase::_shared& data, int head, int tail)
		:_Mybase(data),
		_Myhead(head < 0 ? std::max<size_t>(0, data->size() - head) : std::max<size_t>(0, head)),
		_Mytail(tail < 0 ? std::max<size_t>(0, data->size() + tail) : std::min<size_t>(data->size(), tail))
	{
		_Myhead = std::min(_Myhead, data->size());
		_Mytail = std::min(_Mytail, data->size());
		if (_Myhead > _Mytail)
			std::swap(_Myhead, _Mytail);
	}
	instance(const instance& data) :_Mybase(data), _Myhead(data._Myhead), _Mytail(data._Mytail) {}
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
	constexpr auto get_head() const noexcept
	{
		return _Myhead;
	}
	constexpr auto get_tail() const noexcept
	{
		return _Mytail;
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
		return this->get()->cbegin() + _Mytail;
	}
	auto rbegin() const noexcept
	{
		return this->get()->rbegin() + _Myhead;
	}
	auto rend() const noexcept
	{
		return this->get()->rbegin() + _Myhead;
	}
	auto crbegin() const noexcept
	{
		return this->get()->crbegin() + _Myhead;
	}
	auto crend() const noexcept
	{
		return this->get()->crbegin() + _Myhead;
	}
	auto size() const noexcept 
	{
		return _Mytail - _Myhead;
	};
	decltype(auto) at(int index) const
	{
		if (index < 0)
			index = this->size() + index;
		return this->get()->at(index + _Myhead);
	}
	const auto& front() const noexcept 
	{
		return *(this->begin() + _Myhead);
	};
	const auto& back() const noexcept 
	{
		return *(this->begin() + _Mytail);
	};
	auto data() const noexcept
	{
		return this->get()->data() + _Myhead;
	}
	const auto* cdata() const noexcept
	{
		return this->get()->data() + _Myhead;
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
	auto deep_copy() const
	{
		return instance(**this);
	}
	auto shallow_copy() const noexcept
	{
		return instance(*this);
	}
	copy_func_return_auto_with_noexcept(c_str);
	auto substr(int offset = 0, int Count = static_cast<size_t>(-1))
	{
		if (offset < 0)
			offset = this->size() - offset;
		if (Count < 0)
			Count = this->size() - Count;
		return this->get()->substr(offset + _Myhead, Count);
	}
	template<typename _Right>
	bool start_with(const _Right& str) const noexcept
	{
		return this->size() >= str.size() && _Traits::compare(this->data(), str.data(), str.size()) == 0;
	}
	bool start_with(const _Elem* ptr) const noexcept
	{
		return start_with(tag(ptr));
	}
	template<typename _Right>
	bool end_with(const _Right& str) const noexcept
	{
		return this->size() >= str.size() && _Traits::compare(this->data() + this->size() - str.size(), str.data(), str.size()) == 0;
	}
	bool end_with(const _Elem* ptr) const noexcept
	{
		return end_with(tag(ptr));
	}
	template<typename _Right>
	bool contains(const _Right& str) const noexcept
	{
		if (this->size() < str.size())
			return false;
		for (int offset = 0, end = this->size() - str.size(); offset < end; offset++)
		{
			if (_Traits::compare(this->data() + offset, str.data(), str.size()) == 0)
				return true;
		}
		return false;
	}
	bool contains(const _Elem* ptr) const noexcept
	{
		return contains(tag(ptr));
	}
	template<typename _Right>
	int compare(const _Right& str) const noexcept
	{
		return _Traits::compare(this->data() + _Myhead, str.data(), std::min(this->size(), str.size()));
	}
	auto compare(const _Elem* ptr) const noexcept
	{
		return contains_compare(tag(ptr));
	}
	int compare(const typename _Mybase::_shared& right)
	{
		return this->get()->compare(*right);
	}
	auto length() const noexcept 
	{
		return _Mytail - _Myhead;
	}
	template<typename _Target>
	size_t edit_distance(const _Target& target) const
	{
		return levenshtein_distance(this->begin(), this->end(), target.cbegin(), target.cend());
	}
	template<size_t length>
	size_t edit_distance(const _Elem target[length]) const
	{
		return levenshtein_distance(this->begin(), this->end(), &target[0], &target[length - 1]);
	}
	size_t edit_distance(_In_ const _Elem* target) const
	{
		return edit_distance(tag(target));
	}
	template<typename _Right>
	int first(const _Right& str) const noexcept
	{
		if (this->size() < str.size())
			return -1;
		for (int offset = 0, end = this->size() - str.size(); offset < end; offset++)
		{
			if (_Traits::compare(this->data() + offset, str.data(), str.size()) == 0)
				return offset;
		}
		return -1;
	}
	int first(const _Elem& ch) const noexcept
	{
		if (this->size() != 0)
			for (int offset = 0, end = this->size(); offset < end; offset++)
				if (this->at(offset) == ch)
					return offset;
		return -1;
	}
	int first(_In_ const _Elem* ptr) const noexcept
	{
		return first(tag(ptr));
	}
	template<typename _Right>
	int last(const _Right& str) const noexcept
	{
		if (this->size() < str.size())
			return -1;
		for (int offset = this->size() - str.size() - 1, end = 0; offset >= end; offset--)
		{
			if (_Traits::compare(this->data() + offset, str.data(), str.size()) == 0)
				return offset;
		}
		return -1;
	}
	int last(const _Elem& ch) const noexcept
	{
		if (this->size() != 0)
			for (int offset = this->size() - 1, end = 0; offset >= end; offset--)
				if (this->at(offset) == ch)
					return offset;
		return -1;
	}
	int last(_In_ const _Elem* ptr) const noexcept
	{
		return last(tag(ptr));
	}
	template<typename _Param>
	tag join(_Param&& arg)
	{
		return tag(std::forward(arg));
	}
	template<typename _First, typename... _Params>
	tag join(_First&& first, _Params&&... args)
	{
		return tag(std::forward<_First>(first)) + join(std::forward<_Params>(args)...);
	}
	template<typename... Args>
	auto format_s(void* buffer, size_t buffer_size, Args&&... args)
	{
		if constexpr (std::is_same_v<_Elem, char>)
		{
			sprintf_s((char*)buffer, buffer_size, **this, std::forward<Args>(args)...);
		}
		else if constexpr (std::is_same_v<_Elem, wchar_t>)
		{
			swprintf_s((wchar_t*)buffer, buffer_size, **this, std::forward<Args>(args)...);
		}
	}
	template<typename... Args>
	auto format(Args&&... args)
	{
		if constexpr (std::is_same_v<_Elem, char>)
		{
			std::string result(1024, 0);
			sprintf_s(result.data(), 1024, **this, std::forward<Args>(args)...);
			return result;
		}
		else if constexpr (std::is_same_v<_Elem, wchar_t>)
		{
			std::wstring result(1024, 0);
			swprintf_s(result.data(), 1024, **this, std::forward<Args>(args)...);
			return result;
		}
	}
	auto get_view() const
	{
		return std::basic_string_view(this->get()->data() + _Myhead, this->size());
	}

	virtual std::string ToString() const noexcept override
	{
		std::string str(sizeof(char) * this->size(), 0);
		::memmove(str.data(), this->data(), this->size());
		return str;
	}

};
template<typename _Elem, typename _Traits, typename _Alloc, typename _OS>
decltype(auto) operator<<(_OS& os, const instance<view_indicator<std::basic_string<_Elem, _Traits, _Alloc>>, true>& str)
{
	os << str.get_view();
	return os;
}

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

#endif // !__FILE_CONVENTION_STD_INSTANCE
