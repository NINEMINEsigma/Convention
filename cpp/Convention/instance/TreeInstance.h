#ifndef CONVENTION_KIT_TREEINSTANCE_H
#define CONVENTION_KIT_TREEINSTANCE_H

#include "Convention/instance/Interface.h"

namespace Convention
{
	template<typename Element, template<typename> class Allocator>
	class TreeNode
	{
	private:
		UniquePtr<TreeNode, DefaultDelete<TreeNode, Allocator>> parent, next, child;
	public:
		Element data;

		bool HasParent() const noexcept
		{
			return parent != nullptr;
		}
		bool IsRoot() const noexcept
		{
			return !HasParent();
		}
		bool HasChild() const noexcept
		{
			return child != nullptr;
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

		TreeNode& GetParent() const noexcept
		{
			return *parent;
		}
		TreeNode& GetChild() const noexcept
		{
			return *child;
		}
		TreeNode& GetNext() const noexcept
		{
			return *next;
		}
	};
}

#endif