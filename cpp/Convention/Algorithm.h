/**
 * @file Algorithm.h
 * @brief 通用算法库，包含枚举和排序等基础算法实现
 * @details 该文件包含了一系列模板化的通用算法实现，支持自定义数据类型和比较器
 */

#ifndef CONVENTION_KIT_ALGORITHM_H
#define CONVENTION_KIT_ALGORITHM_H

#include "Convention/Config.h"

namespace Convention
{
	/**
	 * @brief 数据结构的类型别名
	 */
	namespace DataStructure
	{
		template<typename T, size_t size>
		using Array = std::array<T, size>;

		template<typename T, template<typename> class Allocator = std::allocator>
		using Vector = std::vector<T, Allocator<T>>;

		template<typename T, template<typename> class Allocator = std::allocator>
		using List = std::list<T, Allocator<T>>;

		template<typename Key, typename Value, template<typename> class Comparer = std::less, template<typename> class Allocator = std::allocator>
		using RedBlackTree = std::multimap<Key, Value, Comparer<Key>, Allocator<std::pair<Key, Value>>>;

		template<typename Key, typename Value, template<typename> class Comparer = std::less, template<typename> class Allocator = std::allocator>
		using Dictionary = std::map<Key, Value, Comparer<Key>, Allocator<std::pair<Key, Value>>>;

		template<typename Key, typename Value, template<typename> class Hasher = std::hash, template<typename> class Allocator = std::allocator>
		using HashMap = std::unordered_map<Key, Value, Hasher<Key>, Allocator<std::pair<Key, Value>>>;

		template<typename T, template<typename> class Hasher = std::hash, template<typename> class Allocator = std::allocator>
		using HashSet = std::unordered_set<T, Hasher<T>, Allocator<T>>;

		template<typename T, class Container = std::vector<T>, template<typename> class Comparer = std::less>
		using Heap = std::priority_queue<T, Container, Comparer<T>>;

		template<typename T, template<typename,template<typename> class> class Container, template<typename> class Allocator = std::allocator>
		using MinHeap = Heap<T, Container<T, Allocator<T>>, std::greater>;

		template<typename T, template<typename, template<typename> class> class Container, template<typename> class Allocator = std::allocator>
		using MaxHeap = Heap<T, Container<T, Allocator<T>>, std::less>;
	}

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
			typename TIter,
			// function: _Elem(_Elem&)
			typename TGenerator>
		auto EnumerateMonographedMatching(
			TIter begin,
			TIter end,
			TGenerator generator
		)
		{
			using TType = std::decay_t<decltype(*begin)>;
			std::vector<std::tuple<TType, TType>> result;
			std::set<TType> buffer;
			for (auto head = begin, tail = end; head != end; head++)
			{
				if (buffer.count(*head))
					continue;
				buffer.insert(*head);
				auto generatedElement = generator(*head);
				if (buffer.count(generatedElement))
				{
					buffer.insert(generatedElement);
					result.push_back(std::make_tuple(*head, generatedElement));
				}
			}
			return result;
		}
		template<typename TIter>
		auto EnumerateUntil(
			TIter begin,
			TIter end,
			std::function<bool(TIter)> predicate
		)
		{
			while (begin != end)
				if (predicate(begin++) == false)
					break;
			return begin;
		}
		template<typename TIter>
		auto EnumerateUntilNot(
			TIter begin,
			TIter end,
			std::function<bool(TIter)> predicate
		)
		{
			while (begin != end)
				if (predicate(begin++))
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
		template<typename TIter, typename TComp = std::less<>>
		bool IsSorted(TIter begin, TIter end, TComp comp = TComp())
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

		/**
		 * @brief 检查序列是否单调
		 * @tparam _Iter 迭代器类型
		 * @param begin 序列起始迭代器
		 * @param end 序列结束迭代器
		 * @return 如果序列单调则返回true，否则返回false
		 * @complexity O(logn)，其中n为序列长度
		 * @details
		 * 使用二分查找的思想检查序列的单调性。
		 * 如果序列中存在峰值或谷值，则序列非单调。
		 *
		 * 使用示例：
		 * @code
		 * std::vector<int> vec1 = {1, 2, 3, 4, 5}; // 单调递增
		 * std::vector<int> vec2 = {5, 4, 3, 2, 1}; // 单调递减
		 * std::vector<int> vec3 = {1, 3, 2, 4, 5}; // 非单调
		 * bool is_monotonic1 = IsMonotonically(vec1.begin(), vec1.end()); // true
		 * bool is_monotonic2 = IsMonotonically(vec2.begin(), vec2.end()); // true
		 * bool is_monotonic3 = IsMonotonically(vec3.begin(), vec3.end()); // false
		 * @endcode
		 */
		template<typename TIter>
		bool IsMonotonically(TIter begin, TIter end)
		{
			using TType = typename std::iterator_traits<TIter>::value_type;
			auto n = std::distance(begin, end);

			// 空序列或单元素序列认为是单调的
			if (n <= 1) return true;

			// 检查序列是递增还是递减
			bool is_increasing = *(begin + 1) >= *begin;

			// 使用二分查找寻找峰值或谷值
			std::function<bool(TIter, TIter)> findPeak = [&](auto head, auto tail) -> bool
            {
				if (std::distance(head, tail) <= 1)
					return true;

				auto mid = head + (std::distance(head, tail) - 1) / 2;
				bool left_valid = mid == head ||
					(is_increasing ? *mid >= *(mid - 1) : *mid <= *(mid - 1));
				bool right_valid = mid == tail - 1 ||
					(is_increasing ? *mid <= *(mid + 1) : *mid >= *(mid + 1));

				// 如果当前位置违反单调性，返回false
				if (!left_valid || !right_valid)
					return false;

				// 递归检查左右两部分
				return findPeak(head, mid) && findPeak(mid + 1, tail);
			};

			return findPeak(begin, end);
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
		template<typename TIter, typename TComp = std::less<>>
		void SelectionSort(TIter begin, TIter end, TComp comp = TComp())
		{
			for (auto i = begin; i != end; ++i)
			{
				auto minElement = i;
				for (auto j = std::next(i); j != end; ++j)
				{
					if (comp(*j, *minElement))
						minElement = j;
				}
				if (minElement != i)
					std::iter_swap(i, minElement);
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
		template<typename TIter, typename TComp = std::less<>>
		void BubbleSort(TIter begin, TIter end, TComp comp = TComp())
		{
			if (begin == end)
				return;

			auto last = std::prev(end);
			bool swapped;
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
			} while (swapped);
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
		template<typename TIter, typename TComp = std::less<>>
		void InsertionSort(TIter begin, TIter end, TComp comp = TComp())
		{
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

		/**
		 * @brief 归并排序算法实现（迭代版本）
		 * @tparam _Iter 迭代器类型
		 * @tparam _Comp 比较器类型，默认使用std::less<>
		 * @param begin 序列起始迭代器
		 * @param end 序列结束迭代器
		 * @param comp 比较器对象
		 * @complexity O(nlogn)，其中n为序列长度
		 * @details
		 * 归并排序，使用自底向上的方法。
		 * 算法首先将序列视为n个长度为1的子序列，然后逐步合并相邻的有序子序列，
		 * 直到整个序列有序。这种实现避免了递归调用的栈开销。
		 */
		template<typename TIter, typename TComp = std::less<>, typename TBuffer = std::vector<typename std::iterator_traits<TIter>::value_type>>
		void MergeSort(TIter begin, TIter end, TComp comp = TComp())
		{
			auto length = std::distance(begin, end);
			if (length <= 1)
				return;

			TBuffer buffer(length);
			for (size_t size = 1; size < length; size *= 2)
			{
				for (auto left = begin; left != end;)
				{
					auto mid = std::next(left, std::min<std::ptrdiff_t>(size, length - std::distance(begin, left)));
					auto right = std::next(mid, std::min<std::ptrdiff_t>(size, length - std::distance(begin, mid)));

					auto l = left;
					auto r = mid;
					auto dest = buffer.begin() + std::distance(begin, left);

					while (l != mid && r != right)
					{
						if (comp(*r, *l))
							*dest++ = std::move(*r++);
						else
							*dest++ = std::move(*l++);
					}

					while (l != mid)
						*dest++ = std::move(*l++);
					while (r != right)
						*dest++ = std::move(*r++);

					left = right;
				}

				std::move(buffer.begin(), buffer.begin() + length, begin);
			}
		}

		/**
		 * @brief 三路快速排序算法实现（非递归版本）
		 * @tparam _Iter 迭代器类型
		 * @tparam _Comp 比较器类型，默认使用std::less<>
		 * @param begin 序列起始迭代器
		 * @param end 序列结束迭代器
		 * @param comp 比较器对象
		 * @complexity 平均O(nlogn)，最坏O(n²)，其中n为序列长度
		 * @details
		 * 三路快速排序的非递归实现，特别适合处理有大量重复元素的序列。
		 * 算法将序列分为三部分：小于、等于和大于基准元素。
		 * 使用栈存储待处理的区间，避免递归调用。
		 */
		template<typename TIter, typename TComp = std::less<>>
		void QuickSort3Way(TIter begin, TIter end, TComp comp = TComp())
		{
			struct Range
			{
				TIter left;
				TIter right;
				Range(TIter l, TIter r) : left(l), right(r) {}
			};
			std::vector<Range> stack;
			stack.emplace_back(begin, end);

			while (!stack.empty())
			{
				auto [left, right] = stack.back();
				stack.pop_back();

				if (left == right || std::next(left) == right)
					continue;

				auto pivotPos = left + (std::distance(left, right) / 2);
				auto pivot = std::move(*pivotPos);
				std::iter_swap(pivotPos, std::prev(right));

				auto leftBoundary = left;
				auto rightBoundary = std::prev(right);
				auto i = left;

				while (i != rightBoundary)
				{
					if (comp(*i, pivot))
					{
						std::iter_swap(i, leftBoundary);
						++leftBoundary;
						++i;
					}
					else if (comp(pivot, *i))
					{
						--rightBoundary;
						std::iter_swap(i, rightBoundary);
					}
					else
						++i;
				}

				std::iter_swap(rightBoundary, std::prev(right));

				auto leftSize = std::distance(left, leftBoundary);
				auto rightSize = std::distance(rightBoundary, right);

				if (leftSize > 1)
					stack.emplace_back(left, leftBoundary);
				if (rightSize > 1)
					stack.emplace_back(rightBoundary, right);
			}
		}

		/**
		 * @brief 查找序列中第k个元素（如果序列经过排序）
		 * @tparam _Iter 迭代器类型
		 * @tparam _Comp 比较器类型，默认使用std::less<>
		 * @param begin 序列起始迭代器
		 * @param end 序列结束迭代器
		 * @param k 要查找的位置（从0开始），默认查找第k小的元素
		 * @param comp 比较器对象，默认使用less实现第k小，使用greater可查找第k大
		 * @return 返回第k个元素的值
		 * @complexity 平均O(n)，最坏O(n²)，其中n为序列长度
		 * @details
		 * 使用三路快速选择算法的非递归实现。
		 * 该算法不会对整个序列进行排序，而是快速定位到第k个元素。
		 * 使用三路分区优化，特别适合处理有大量重复元素的序列。
		 * 使用随机选择的基准元素，以提高算法在各种数据分布下的稳定性。
		 * 默认查找第k小的元素，如果要查找第k大的元素，可以：
		 * 1. 使用 std::greater<> 作为比较器
		 * 2. 将k设置为 n-1-k，其中n为序列长度
		 * 注意：该函数会修改输入序列的顺序。
		 */
		template<typename TIter, typename TComp = std::less<>>
		auto NthElement(TIter begin, TIter end, size_t k, TComp comp = TComp())
		{
			std::random_device rd;
			static std::mt19937 gen(rd());

			struct Range
			{
				TIter left;
				TIter right;
				Range(TIter l, TIter r) : left(l), right(r) {}
			};
			std::vector<Range> stack;
			stack.emplace_back(begin, end);

			while (!stack.empty())
			{
				auto [left, right] = stack.back();
				stack.pop_back();

				if (left == right)
					continue;
				if (std::next(left) == right)
				{
					if (std::distance(begin, left) == k)
						return *left;
					continue;
				}

				std::uniform_int_distribution<> dis(0, std::distance(left, right) - 1);
				auto pivotPos = std::next(left, dis(gen));
				auto pivot = std::move(*pivotPos);
				std::iter_swap(pivotPos, std::prev(right));

				auto lt = left;
				auto gt = std::prev(right);
				auto i = left;

				while (i != gt)
				{
					if (comp(*i, pivot))
					{
						std::iter_swap(i, lt);
						++lt;
						++i;
					}
					else if (comp(pivot, *i))
					{
						--gt;
						std::iter_swap(i, gt);
					}
					else
						++i;
				}

				std::iter_swap(gt, std::prev(right));

				auto leftCount = std::distance(begin, lt);
				auto midCount = std::distance(lt, gt);

				if (k < leftCount)
					stack.emplace_back(left, lt);
				else if (k >= leftCount + midCount)
					stack.emplace_back(gt, right);
				else
					return pivot;
			}

			throw std::runtime_error("NthElement: Invalid index");
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
		/**
		 * @brief 查找序列中第k个元素（如果序列经过排序）
		 * @tparam _Iter 迭代器类型
		 * @tparam _Comp 比较器类型，默认使用std::less<>
		 * @param begin 序列起始迭代器
		 * @param end 序列结束迭代器
		 * @param k 要查找的位置（从0开始），默认查找第k小的元素
		 * @param comp 比较器对象，默认使用less实现第k小，使用greater可查找第k大
		 * @return 返回第k个元素的值
		 * @complexity 平均O(n)，最坏O(n²)，其中n为序列长度
		 * @details
		 * 使用三路快速选择算法的非递归实现。
		 * 该算法不会对整个序列进行排序，而是快速定位到第k个元素。
		 * 使用三路分区优化，特别适合处理有大量重复元素的序列。
		 * 使用随机选择的基准元素，以提高算法在各种数据分布下的稳定性。
		 * 默认查找第k小的元素，如果要查找第k大的元素，可以：
		 * 1. 使用 std::greater<> 作为比较器
		 * 2. 将k设置为 n-1-k，其中n为序列长度
		 * 注意：该函数会修改输入序列的顺序。
		 */
		template<typename _Iter, typename _Comp = std::less<>>
		typename std::iterator_traits<_Iter>::value_type
		NthElement(_Iter begin, _Iter end, size_t k, _Comp comp = _Comp())
		{
			using value_type = typename std::iterator_traits<_Iter>::value_type;

			if (begin == end || k >= static_cast<size_t>(std::distance(begin, end)))
				throw std::out_of_range("k is out of range");

			// 单元素序列的特殊处理
			if (std::next(begin) == end)
				return *begin;

			// 初始化随机数生成器
			static std::random_device rd;
			static std::mt19937 gen(rd());

			// 用于存储待处理区间的栈
			struct Range
			{
				_Iter left;
				_Iter right;
				Range(_Iter l, _Iter r) : left(l), right(r) {}
			};
			std::vector<Range> stack;
			stack.emplace_back(begin, end);

			while (!stack.empty())
			{
				auto [left, right] = stack.back();
				stack.pop_back();

				// 处理区间长度小于等于1的情况
				if (left == right)
					continue;
				if (std::next(left) == right)
				{
					if (std::distance(begin, left) == k)
						return *left;
					continue;
				}

				// 随机选择基准元素
				auto dist = std::distance(left, right);
				std::uniform_int_distribution<> dis(0, dist - 1);
				auto pivot_pos = std::next(left, dis(gen));
				value_type pivot = std::move(*pivot_pos);
				std::iter_swap(pivot_pos, std::prev(right));

				// 三路分区
				auto lt = left;      // 小于区域的右边界
				auto gt = std::prev(right); // 大于区域的左边界
				auto i = left;       // 当前扫描位置

				while (i != gt)
				{
					if (comp(*i, pivot))
					{
						std::iter_swap(i, lt);
						++lt;
						++i;
					}
					else if (comp(pivot, *i))
					{
						--gt;
						std::iter_swap(i, gt);
					}
					else
					{
						++i;
					}
				}

				// 将基准元素放回正确位置
				std::iter_swap(gt, std::prev(right));
				++gt;

				// 计算区间范围
				auto left_count = std::distance(begin, lt);
				auto mid_count = std::distance(lt, gt);
				auto equal_range_end = left_count + mid_count;

				// 如果k在等于pivot的范围内，直接返回pivot
				if (k >= left_count && k < equal_range_end)
				{
					return pivot;
				}
				// 如果k在左半部分
				else if (k < left_count)
				{
					stack.emplace_back(left, lt);
				}
				// 如果k在右半部分
				else
				{
					stack.emplace_back(gt, right);
				}
			}

			// 这里实际上不会到达，因为如果k有效，一定会在循环中返回
			throw std::runtime_error("Algorithm failed to find the nth element");
		}
	}
}

#endif // !CONVENTION_KIT_ALGORITHM_H
