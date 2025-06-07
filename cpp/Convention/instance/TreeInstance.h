#ifndef CONVENTION_KIT_TREEINSTANCE_H
#define CONVENTION_KIT_TREEINSTANCE_H

#include "Convention/instance/Interface.h"

namespace Convention
{
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
		};
	};
}

#endif