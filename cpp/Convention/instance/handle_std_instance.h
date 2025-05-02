#ifndef __FILE_CONVENTION_STD_INSTANCE
#define __FILE_CONVENTION_STD_INSTANCE

#include "Convention/instance/Interface.h"


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
	explicit instance(std::initializer_list<_Elem> data) :_Mybase(new tag(data)) {}
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
	void clear() noexcept
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
		throw std::overflow_error("view.at overflow");
	}
	const auto& front() const
	{
		return *(this->begin());
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
	auto erase(const_iterator iter)
	{
		_Mytail--;
		return this->get()->erase(iter);
	}
	auto erase(const_iterator head, const_iterator end)
	{
		_Mytail -= std::distance(head, end);
		return this->get()->erase(head, end);
	}
	size_t erase(const _Elem& value, size_t ignore = 0, size_t countdown = static_cast<size_t>(-1))
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
	explicit instance(std::initializer_list<_Elem> data) :_Mybase(new tag(data)) {}
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
		return compare(tag(ptr));
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
	const auto& front() const
	{
		return *(this->begin() + _Myhead);
	};
	const auto& back() const
	{
		return *std::prev(this->end());
	};
	auto data() const
	{
		return this->get()->data() + _Myhead;
	}
	const auto* cdata() const
	{
		return this->get()->data() + _Myhead;
	}
	bool data_empty() const noexcept
	{
		return _Mytail - _Myhead == 0;
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
		//return contains_compare(tag(ptr));
		return compare(tag(ptr));
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
template<typename _Type>
constexpr bool is_string_or_string_instance_v = 
	internal::is_string_v<_Type>||
	(internal::is_instance_v<_Type>&&(typename _Type::_Element == char||typename _Type::_Element == wchar_t));

#pragma endregion

#pragma region std::list

template<typename _Elem, typename _Alloc>
class instance<std::list<_Elem, _Alloc>, true> :public instance<std::list<_Elem, _Alloc>, false>
{
public:
	using tag = std::list<_Elem, _Alloc>;
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

	copy_func_return_auto_with_noexcept(size);
	copy_func_with_noexcept(clear);

	decltype(auto) front() const
	{
		return this->get()->front();
	}

	decltype(auto) back() const
	{
		return this->get()->back();
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

	template<typename... _Args>
	auto insert(const_iterator pos, _Args&&... args)
	{
		return this->get()->insert(pos, std::forward<_Args>(args)...);
	}

	auto splice(const_iterator pos, tag& other)
	{
		return this->get()->splice(pos, other);
	}

	auto splice(const_iterator pos, tag& other, const_iterator it)
	{
		return this->get()->splice(pos, other, it);
	}

	auto splice(const_iterator pos, tag& other, const_iterator first, const_iterator last)
	{
		return this->get()->splice(pos, other, first, last);
	}

	void remove(const _Elem& value)
	{
		this->get()->remove(value);
	}

	template<typename _Pred>
	void remove_if(_Pred pred)
	{
		this->get()->remove_if(pred);
	}

	void unique()
	{
		this->get()->unique();
	}

	template<typename _BinaryPred>
	void unique(_BinaryPred pred)
	{
		this->get()->unique(pred);
	}

	void sort()
	{
		this->get()->sort();
	}

	template<typename _Compare>
	void sort(_Compare comp)
	{
		this->get()->sort(comp);
	}

	void reverse() noexcept
	{
		this->get()->reverse();
	}

	void merge(tag& other)
	{
		this->get()->merge(other);
	}

	template<typename _Compare>
	void merge(tag& other, _Compare comp)
	{
		this->get()->merge(other, comp);
	}
};
template<typename _Elem, typename _Alloc>
class instance<view_indicator<std::list<_Elem, _Alloc>>, true> :public instance<std::list<_Elem, _Alloc>, false>
{
public:
	using tag = std::list<_Elem, _Alloc>;
	using _Mybase = instance<tag, false>;
private:
	typename tag::iterator _Myhead;
	typename tag::iterator _Mytail;
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
		_Mysize = std::distance(head, tail);
	}

	instance(const instance& data) 
		:_Mybase(data), _Myhead(data._Myhead), _Mytail(data._Mytail), _Mysize(data._Mysize) {}

	instance_move_operator(public)
	{
		this->_Myhead = other._Myhead;
		this->_Mytail = other._Mytail;
		this->_Mysize = other._Mysize;
	}

	auto begin() const noexcept { return _Myhead; }
	auto end() const noexcept { return _Mytail; }
	auto cbegin() const noexcept { return _Myhead; }
	auto cend() const noexcept { return _Mytail; }

	size_t size() const noexcept { return _Mysize; }

	bool data_empty() const noexcept { return _Mysize == 0; }

	decltype(auto) front() const { return *_Myhead; }
	decltype(auto) back() const { return *std::prev(_Mytail); }

	auto erase(const_iterator pos)
	{
		if (pos == _Myhead) ++_Myhead;
		if (pos == _Mytail) --_Mytail;
		--_Mysize;
		return this->get()->erase(pos);
	}

	template<typename... _Args>
	auto insert(const_iterator pos, _Args&&... args)
	{
		if (pos == _Myhead) --_Myhead;
		if (pos == _Mytail) ++_Mytail;
		++_Mysize;
		return this->get()->insert(pos, std::forward<_Args>(args)...);
	}
};

#pragma endregion

#pragma region std::map

template<typename _Key, typename _Ty, typename _Compare, typename _Alloc>
class instance<std::map<_Key, _Ty, _Compare, _Alloc>, true> :public instance<std::map<_Key, _Ty, _Compare, _Alloc>, false>
{
public:
	using tag = std::map<_Key, _Ty, _Compare, _Alloc>;
	using _Mybase = instance<tag, false>;
	using key_type = _Key;
	using mapped_type = _Ty;
	using value_type = std::pair<const _Key, _Ty>;
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

	copy_func_return_auto_with_noexcept(size);
	copy_func_with_noexcept(clear);

	bool data_empty() const noexcept
	{
		return this->get()->empty();
	}

	auto find(const key_type& key) const
	{
		return this->get()->find(key);
	}

	size_t count(const key_type& key) const
	{
		return this->get()->count(key);
	}

	auto lower_bound(const key_type& key) const
	{
		return this->get()->lower_bound(key);
	}

	auto upper_bound(const key_type& key) const
	{
		return this->get()->upper_bound(key);
	}

	auto equal_range(const key_type& key) const
	{
		return this->get()->equal_range(key);
	}

	template<typename... _Args>
	auto insert(_Args&&... args)
	{
		return this->get()->insert(std::forward<_Args>(args)...);
	}

	template<typename... _Args>
	auto emplace(_Args&&... args)
	{
		return this->get()->emplace(std::forward<_Args>(args)...);
	}

	auto erase(const_iterator pos)
	{
		return this->get()->erase(pos);
	}

	auto erase(const_iterator first, const_iterator last)
	{
		return this->get()->erase(first, last);
	}

	size_t erase(const key_type& key)
	{
		return this->get()->erase(key);
	}

	mapped_type& operator[](const key_type& key)
	{
		return (*this->get())[key];
	}

	mapped_type& operator[](key_type&& key)
	{
		return (*this->get())[std::move(key)];
	}

	mapped_type& at(const key_type& key)
	{
		return this->get()->at(key);
	}

	const mapped_type& at(const key_type& key) const
	{
		return this->get()->at(key);
	}
};

// 不能使用字符串作为返回值, 因为没有考虑宽窄字符的转换, 需要注入到字符串中需要使用stringstream
template<typename _OS, typename _Key, typename _Ty, typename _Compare, typename _Alloc>
_OS& map_easy_json(
	_OS& os, 
	typename const std::map<_Key, _Ty, _Compare, _Alloc>::iterator& begin,
	typename const std::map<_Key, _Ty, _Compare, _Alloc>::iterator& end,
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

template<typename _OS, typename _Key, typename _Ty, typename _Compare, typename _Alloc>
_OS& operator<<(_OS& os, const instance<std::map<_Key, _Ty, _Compare, _Alloc>, false>& data)
{
	map_easy_json(os, data.begin(), data.end(), true);
    return os;
}

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
    instance(_Args... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
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
    instance(_Args... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
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
    instance(_Args... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
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
    instance(_Args... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
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
    instance(_Args... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
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
    instance(_Args... args) : _Mybase(new tag(std::forward<_Args>(args)...)) {}
    instance(instance& data) :_Mybase(data) {}
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

#endif // !__FILE_CONVENTION_STD_INSTANCE
