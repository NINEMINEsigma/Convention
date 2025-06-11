#ifndef CONVENTION_KIT_VIRTUALxINSTANCE_H
#define CONVENTION_KIT_VIRTUALxINSTANCE_H

#include "Convention/instance/Interface.h"

namespace Convention
{
	template<typename _Inside,typename _Key,typename _Value>
	class VirtualOperatorIndex :public _Inside
	{
	public:
		template<typename... Args>
		VirtualOperatorIndex(Args&&... args) :_Inside(std::forward<Args>(args)...) {}
		template<typename... Args>
		decltype(auto) operator=(Args&&... args)
		{
			return _Inside::operator=(std::forward<Args>(args)...);
		}
		std::unordered_map<_IndexTy, std::function<_Value&(_IndexTy&)>> virtual_index_source;
		_Value& operator[](const _IndexTy& index)
		{
			if (virtual_index_source.count(index))
				return virtual_index_source[index](index);
			return _Inside::operator[](std::forward(index));
		}
		_Value& operator[](_IndexTy&& index)
		{
			if (virtual_index_source.count(index))
				return virtual_index_source[index](index);
			return _Inside::operator[](std::forward(index));
		}
	};

#ifdef _DEBUG
	void test_voi()
	{
		VirtualOperatorIndex<vector<int>, int, int> a;
		a.virtual_index_source[-1] = [](auto& _) 
			{
				static value = 0;
				value = 0;
				return 0;
			};
		a[-1] = 1;
	}
#endif
}

#endif // !CONVENTION_KIT_VIRTUALxINSTANCE_H
