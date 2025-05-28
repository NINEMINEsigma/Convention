/**
 * @file Algorithm.h
 * @brief 通用算法库，包含枚举和排序等基础算法实现
 * @details 该文件包含了一系列模板化的通用算法实现，支持自定义数据类型和比较器
 */

#ifndef __FILE_ALGORITHM
#define __FILE_ALGORITHM

#include "Convention/Config.h"

namespace convention_kit
{
	/**
	 * @namespace Enumerate
	 * @brief 提供各种枚举算法的实现
	 * @details 包含了一系列用于序列枚举和匹配的模板函数
	 */
	namespace Enumerate
	{
		/**
		 * @brief 在序列中查找满足特定生成规则的匹配对
		 * @tparam _Iter 迭代器类型，通常为std::set<_Elem>::iterator
		 * @tparam _generator 生成器函数类型，接受一个元素返回另一个元素
		 * @param begin 序列起始迭代器
		 * @param end 序列结束迭代器
		 * @param gr 生成器函数对象
		 * @return 返回匹配对的vector，每对用tuple存储
		 * @complexity O(n)，其中n为序列长度
		 */
		template<
			// std::set<_Elem>::iterator
			typename _Iter,
			// function: _Elem(_Elem&)
			typename _generator>
		auto EnumerateMonographedMatching(
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
		auto EnumerateUntil(
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
		auto EnumerateUntilNot(
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
	}

	/**
	 * @namespace Validation
	 * @brief 提供各种验证算法的实现
	 * @details 包含了一系列用于验证序列特性的模板函数，如有序性检查等
	 */
	namespace Validation
	{
		/**
		 * @brief 检查序列是否有序
		 * @tparam _Iter 迭代器类型
		 * @tparam _Comp 比较器类型，默认使用std::less<>
		 * @param begin 序列起始迭代器
		 * @param end 序列结束迭代器
		 * @param comp 比较器对象
		 * @return 如果序列按照比较器定义的顺序排序则返回true，否则返回false
		 * @complexity O(n)，其中n为序列长度
		 * @details
		 * 使用示例：
		 * @code
		 * std::vector<int> vec = {1, 2, 3, 4, 5};
		 * // 检查是否升序
		 * bool isAscending = IsSorted(vec.begin(), vec.end());
		 * // 检查是否降序
		 * bool isDescending = IsSorted(vec.begin(), vec.end(), std::greater<>());
		 * @endcode
		 */
		template<typename _Iter, typename _Comp = std::less<>>
		bool IsSorted(_Iter begin, _Iter end, _Comp comp = _Comp())
		{
			if (begin == end) return true;

			auto next = std::next(begin);
			while (next != end)
			{
				// 如果当前元素与下一个元素的顺序不符合比较器定义的顺序，则序列无序
				if (comp(*next, *begin))
					return false;
				begin = next;
				++next;
			}
			return true;
		}
	}

	/**
	 * @namespace Sorting
	 * @brief 提供各种排序算法的实现
	 * @details 包含了多种基础排序算法的模板实现，支持自定义数据类型和比较器，
	 *          包括选择排序、冒泡排序、插入排序等
	 */
	namespace Sorting
	{
		/**
		 * @brief 选择排序算法实现
		 * @tparam _Iter 迭代器类型
		 * @tparam _Comp 比较器类型，默认使用std::less<>
		 * @param begin 序列起始迭代器
		 * @param end 序列结束迭代器
		 * @param comp 比较器对象
		 * @complexity O(n²)，其中n为序列长度
		 */
		template<typename _Iter, typename _Comp = std::less<>>
		void SelectionSort(_Iter begin, _Iter end, _Comp comp = _Comp())
		{
			for (auto i = begin; i != end; ++i)
			{
				auto min_pos = i;
				for (auto j = std::next(i); j != end; ++j)
				{
					if (comp(*j, *min_pos))
					{
						min_pos = j;
					}
				}
				if (min_pos != i)
				{
					std::iter_swap(i, min_pos);
				}
			}
		}

		/**
		 * @brief 冒泡排序算法实现
		 * @tparam _Iter 迭代器类型
		 * @tparam _Comp 比较器类型，默认使用std::less<>
		 * @param begin 序列起始迭代器
		 * @param end 序列结束迭代器
		 * @param comp 比较器对象
		 * @complexity O(n²)，其中n为序列长度
		 */
		template<typename _Iter, typename _Comp = std::less<>>
		void BubbleSort(_Iter begin, _Iter end, _Comp comp = _Comp())
		{
			if (begin == end) return;
			bool swapped;
			auto last = std::prev(end);
			do
			{
				swapped = false;
				for (auto i = begin; i != last; ++i)
				{
					auto next = std::next(i);
					if (comp(*next, *i))
					{
						std::iter_swap(i, next);
						swapped = true;
					}
				}
				--last;
			} while (swapped && begin != last);
		}

		/**
		 * @brief 插入排序算法实现
		 * @tparam _Iter 迭代器类型
		 * @tparam _Comp 比较器类型，默认使用std::less<>
		 * @param begin 序列起始迭代器
		 * @param end 序列结束迭代器
		 * @param comp 比较器对象
		 * @complexity O(n²)，其中n为序列长度
		 */
		template<typename _Iter, typename _Comp = std::less<>>
		void InsertionSort(_Iter begin, _Iter end, _Comp comp = _Comp())
		{
			if (begin == end) return;
			for (auto i = std::next(begin); i != end; ++i)
			{
				auto key = std::move(*i);
				auto j = i;
				while (j != begin && comp(key, *std::prev(j)))
				{
					*j = std::move(*std::prev(j));
					--j;
				}
				*j = std::move(key);
			}
		}
	}

	/**
	 * @namespace Find
	 * @brief 提供各种查找算法的实现
	 * @details 包含了一系列用于在序列中进行元素查找的模板函数，
	 *          支持多种查找策略和匹配条件
	 */
	namespace Find
	{

	}
}

#endif // !__FILE_ALGORITHM
