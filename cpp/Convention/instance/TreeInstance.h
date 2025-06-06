#ifndef CONVENTION_KIT_TREEINSTANCE_H
#define CONVENTION_KIT_TREEINSTANCE_H

#include "Convention/instance/Interface.h"

namespace Convention
{
	struct TreeIndictor
	{
		template<
			typename Element,
			bool Enable2EndOperator,
			template<typename> class Allocator = std::allocator
		>
		struct ListTrait
		{


			using tag = Element;
			using Tree = TreeNode <Element, Enable2EndOperator, false, Allocator>;
			using Iterator = TreeNodeIterator<Element, Enable2EndOperator, false, Allocator, Tree::ChildOffset>;
			using ReverseIterator = TreeNodeIterator<Element, Enable2EndOperator, false, Allocator, Tree::ParentOffset>;
		};
	};

	template<typename Element, template<typename> class TreeAllocator, template<typename> class Allocator>
	class instance<TreeNode<Element, false, false, TreeAllocator>, true, Allocator, false>
		: public instance<TreeNode<Element, false, false, TreeAllocator>, false, Allocator, false>
	{
	public:
		using Trait = TreeIndictor::ListTrait<Element, false, Allocator>;
		using Tree = Trait::Tree;
		using Iterator = Trait::Iterator;
	private:
		using _Mybase = instance<Tree, false, Allocator, false>;
		SharedPtr<Tree> tail;
	public:
		Iterator begin() noexcept
		{
			return Iterator(*this);
		}
		constexpr Iterator end() noexcept
		{
			return Iterator(nullptr);
		}
		Iterator cbegin() noexcept
		{
			return Iterator(*this);
		}
		constexpr Iterator cend() noexcept
		{
			return Iterator(nullptr);
		}

		template<typename Arg, std::enable_if_t<std::is_convertible_v<Arg, Element>, size_t> = 0>
		void push(Arg&& value)
		{
			SharedPtr<Tree> temp(BuildMyPtr());
			temp->container.SetValue<Tree::ElementOffset>(std::forward<Arg>(value));
			if (this->IsEmpty())
			{
				this->WriteValue(std::move(temp));
				tail = *this;
			}
			else
			{
				tail->container.SetValue
			}
		}
	};
}

#endif