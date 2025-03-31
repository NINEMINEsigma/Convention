#ifndef __FILE_ALGORITHM
#define __FILE_ALGORITHM

#include "Config.h"

template<
	// std::set<_Elem>::iterator
	typename _Iter, 
	// function: _Elem(_Elem&)
	typename _generator>
auto enumerate_monographed_matching(
	_Iter begin,
	_Iter end,
	_generator gr
)
{
	using _Ty = std::decay_t<decltype(*begin)>;
	std::vector<std::tuple<_Ty, _Ty>> result;
	std::set<_Ty> buffer;
	for (auto head = begin, tail = end; head != end; head++)
	{
		if (buffer.count(*head))
			continue;
		buffer.insert(*head);
		auto gr_en = gr(*head);
		if (buffer.count(gr_en))
		{
			buffer.insert(gr_en);
			result.push_back(std::make_tuple(*head, gr_en));
		}
	}
	return result;
}
template<typename _Iter>
auto enumerate_until(
	_Iter begin,
	_Iter end,
	std::function<bool(_Iter)> pr
)
{
	while (begin != end)
		if (pr(begin++) == false)
			break;
	return begin;
}
template<typename _Iter>
auto enumerate_until_not(
	_Iter begin,
	_Iter end,
	std::function<bool(_Iter)> pr
)
{
	while (begin != end)
		if (pr(begin++))
			break;
	return begin;
}



#endif // !__FILE_ALGORITHM
