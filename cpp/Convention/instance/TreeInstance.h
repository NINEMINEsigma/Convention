#ifndef CONVENTION_KIT_TREEINSTANCE_H
#define CONVENTION_KIT_TREEINSTANCE_H

#include "Convention/instance/Interface.h"

namespace Convention
{

	/**
	* @brief 树节点结构体
	* @tparam Element 元素类型
	* @tparam EnableParent 是否启用父节点指针
	* @tparam EnableBranchLayer 是否启用同分支兄弟节点指针
	* @tparam Allocator 内存分配器
	*/
	template<
		typename Element,
		bool EnableParent = true,
		bool EnableBranchLayer = true,
		template<typename> class Allocator = std::allocator
	>
	struct TreeNode
	{
		using _InsidePtr = instance<TreeNode, true, Allocator, false>;
		using _PtrContainer = ElementTuple<Element, std::conditional_t<EnableParent, _InsidePtr, void>, std::conditional_t<EnableBranchLayer, _InsidePtr, void>, _InsidePtr>;
		// Forward Offset
		constexpr static size_t _ElemenetForwardOffset = std::is_same_v<Element, void> ? 0 : 1;
		constexpr static size_t _ParentForwardOffset = _ElemenetForwardOffset + (EnableParent ? 1 : 0);
		constexpr static size_t _NextForwardOffset = _ParentForwardOffset + (EnableBranchLayer ? 1 : 0);
		constexpr static size_t _ChildForwardOffset = _NextForwardOffset + 1;
		// Offset
		constexpr static size_t ElementOffset = std::is_same_v<Element, void> ? -1 : 0;
		constexpr static size_t ParentOffset = EnableParent ? _ElemenetForwardOffset : -1;
		constexpr static size_t NextOffset = EnableBranchLayer ? _ParentForwardOffset : -1;
		constexpr static size_t ChildOffset = _NextForwardOffset;
		_PtrContainer container;
	};

	/**
	* @brief 树节点迭代器
	* @tparam Element 树节点元素类型
	* @tparam EnableParent 树节点是否启用父节点指针
	* @tparam EnableBranchLayer 树节点是否启用同分支兄弟节点指针
	* @tparam Allocator 树节点内存分配器
	* @tparam _NextNodePtrOffset 下一个节点应该取的树节点偏移量, 详见TreeNode
	*/
	template<
		typename Element,
		bool EnableParent = true,
		bool EnableBranchLayer = true,
		template<typename> class Allocator = std::allocator,
		size_t _NextNodePtrOffset = 1
	>
	struct TreeNodeIterator
	{
		using _MyTreeNode = TreeNode<Element, EnableParent, EnableBranchLayer, Allocator>;
		using _MyNodeContainer = SharedPtr<_MyTreeNode>;

		_MyNodeContainer current;
		bool operator==(const TreeNodeIterator& other) const noexcept
		{
			return current == other.current;
		}
		bool operator!=(const TreeNodeIterator& other) const noexcept
		{
			return !(*this == other);
		}
		bool IsEmpty() const noexcept(noexcept(std::declval<_MyNodeContainer>()->container.template GetValue<_NextNodePtrOffset>().IsEmpty()))
		{
			return current->container.template GetValue<_NextNodePtrOffset>().IsEmpty();
		}
		operator bool() const noexcept(noexcept(std::declval<TreeNodeIterator>().IsEmpty()))
		{
			return !IsEmpty();
		}

		constexpr TreeNodeIterator(nullptr_t) noexcept : current(nullptr) {}
		TreeNodeIterator(_MyNodeContainer node) noexcept : current(std::move(node)) {}
		TreeNodeIterator(const TreeNodeIterator& other) noexcept : current(other.current) {}
		TreeNodeIterator(TreeNodeIterator&& other) noexcept : current(std::move(other.current)) {}

		TreeNodeIterator& operator++()
		{
			if (!current->container.template GetValue<_NextNodePtrOffset>().IsEmpty())
				throw std::out_of_range("TreeNodeIterator cannot be incremented past the end.");
			current = current->container.template GetValue<_NextNodePtrOffset>();
			return *this;
		}
	};

	template<typename TreeNodeIteratorType>
	struct TreeNodeEnumerater
	{
		TreeNodeIteratorType m_begin, m_end;
		TreeNodeEnumerater(TreeNodeIteratorType iter) noexcept : m_begin(std::move(iter)), m_end(nullptr) {}
		TreeNodeEnumerater(TreeNodeIteratorType head, TreeNodeIteratorType tail) noexcept : m_begin(std::move(head)), m_end(std::move(tail)) {}
		constexpr TreeNodeIteratorType begin() noexcept
		{
			return m_begin;
		}
		constexpr TreeNodeIteratorType end() noexcept
		{
			return m_end;
		}
	};

	struct TreeIndicator
	{
		template<
			typename Element,
			bool EnableParent,
			bool EnableBranchLayer,
			template<typename> class TreeAllocator,
			template<typename> class Allocator
		>
		struct BidirectionalListsTrait
		{
			using ListNode = TreeNode<Element, true, false, TreeAllocator>;
			using ListNodeIterator = TreeNodeIterator<Element, true, false, TreeAllocator, ListNode::ChildOffset>;
			using ListNodeReverseIterator = TreeNodeIterator<Element, true, false, TreeAllocator, ListNode::ParentOffset>;

			using ListInstance = typename ListNode::_InsidePtr;
			using type = ElementTuple<ListInstance, ListInstance>;

			static decltype(auto) GetNext(ListNode& node)
			{
				return node.container.template GetValue<ListNode::ChildOffset>();
			}
			static decltype(auto) GetNext(const ListNode& node)
			{
				return node.container.template GetValue<ListNode::ChildOffset>();
			}
			static void SetNext(ListNode& node, ListInstance next)
			{
				GetNext(node) = std::move(next);
			}
			static decltype(auto) GetPrev(ListNode& node)
			{
				return node.container.template GetValue<ListNode::ParentOffset>();
			}
			static decltype(auto) GetPrev(const ListNode& node)
			{
				return node.container.template GetValue<ListNode::ParentOffset>();
			}
			static void SetPrev(ListNode& node, ListInstance prev)
			{
				GetPrev(node) = std::move(prev);
			}

			static bool IsEmpty(type& list)
			{
				return list.template GetValue<0>().IsEmpty() || list.template GetValue<1>().IsEmpty();
			}

			static decltype(auto) GetHead(type& list)
			{
				return list.template GetValue<0>();
			}
			static void SetHead(type& list, ListInstance node)
			{
				list.template SetValue<0>(std::move(node));
			}
			static decltype(auto) GetTail(type& list)
			{
				return list.template GetValue<1>();
			}
			static void SetTail(type& list, ListInstance node)
			{
				list.template SetValue<1>(std::move(node));
			}
			static void AppendFront(type& list, ListInstance node)
			{
				if (IsEmpty(list))
				{
					GetHead(list) = std::move(node);
					GetTail(list) = std::move(node);
				}
				else
				{
					auto&& end = GetHead(list).ReadValue();
					SetNext(node.ReadValue(), end);
					SetPrev(end, node);
					SetHead(list, node);
				}
			}
			static void AppendBack(type& list, ListInstance node)
			{
				if (IsEmpty(list))
				{
					GetHead(list) = std::move(node);
					GetTail(list) = std::move(node);
				}
				else
				{
					auto&& end = GetTail(list).ReadValue();
					SetNext(end, node);
					SetPrev(node.ReadValue(), end);
					SetHead(list, node);
				}
			}
		};
	};

}

#endif