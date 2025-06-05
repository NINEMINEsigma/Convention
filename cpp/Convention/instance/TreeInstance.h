#ifndef CONVENTION_KIT_TREEINSTANCE_H
#define CONVENTION_KIT_TREEINSTANCE_H

#include "Convention/instance/Interface.h"

namespace Convention
{
	template<typename Element, bool EnableParent = true, template<typename> class Allocator = std::allocator>
	class TreeNode
	{
	public:
		using _InsidePtr = SharedPtr<TreeNode>;//UniquePtr<TreeNode, DefaultDelete<TreeNode, Allocator>>;
		using _SharedOutsidePtr = WeakPtr<TreeNode>;
		using _PtrContainer = std::conditional_t<EnableParent, std::tuple<_InsidePtr, _InsidePtr, _InsidePtr>, std::tuple<_InsidePtr, _InsidePtr>>;
		constexpr static size_t _ParentOffset = 0;
		constexpr static size_t _NextOffset = 1 - (EnableParent ? 1 : 0);
		constexpr static size_t _ChildOffset = 2 - (EnableParent ? 1 : 0);
	private:
		_PtrContainer container;

		static SetNodeLayerParent(_InsidePtr oldParent, _InsidePtr newParent)
		{

		}

	public:
		_InsidePtr ReadParentNode() const noexcept
		{
			if constexpr (EnableParent)
				return std::get<_ParentOffset>(container);
			else
				throw std::logic_error("TreeNode does not support parent node access when EnableParent is false.");
		}
		_InsidePtr ReadNextNode() const noexcept
		{
			return std::get<_NextOffset>(container);
		}
		_InsidePtr ReadChildNode() const noexcept
		{
			return std::get<_ChildOffset>(container);
		}

		Element data;

		/**
		* @brief �Ƿ��и��ڵ�
		*/
		bool HasParent() const noexcept
		{
			if constexpr (EnableParent)
				return ReadParentNode() != nullptr;
			else
				return false;
		}
		/**
		* @brief �Ƿ��Ǹ��ڵ�(�޸��ڵ������ֵܽڵ�)
		*/
		bool IsRoot() const noexcept
		{
			return !HasParent() && !HasNext();
		}
		/**
		* @brief ��ȡ���ڵ�
		*/
		_SharedOutsidePtr GetParent() const noexcept
		{
			if constexpr (EnableParent)
				return ReadParentNode();
			else
				return nullptr;
		}
		/**
		* @brief ���ø��ڵ�
		* @return ����ԭ���ڵ�
		*/
		_InsidePtr SetParent(_InsidePtr&& newParent)
		{
			if (IsRoot())
			{
				_InsidePtr oldParent = std::move(parent);
				parent = std::move(newParent);
				return std::move(oldParent);
			}
			else if(HasParent())
			{

			}
		}
		bool HasChild() const noexcept
		{
			return child != nullptr;
		}
		TreeNode& GetChild() const noexcept
		{
			return *child;
		}
		bool IsLeaf() const noexcept
		{
			return !HasChild();
		}
		bool HasNext() const noexcept
		{
			return next != nullptr;
		}
		bool IsLast() const noexcept
		{
			return !HasNext();
		}
		TreeNode& GetNext() const noexcept
		{
			return *next;
		}

	};
}

#endif