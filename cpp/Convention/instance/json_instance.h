#ifndef __FILE_CONVENTION_JSON_INSTANCE
#define __FILE_CONVENTION_JSON_INSTANCE

#include "Convention/instance/nlohmann/json.hpp"
#include "Convention/instance/nlohmann/json_fwd.hpp"

template<typename _Ty>
struct json_indicator
{
	using tag = NLOHMANN_JSON_NAMESPACE::json;
	using target = _Ty;
	constexpr static bool value = true;
};

template<>
struct json_indicator<void>
{
	using tag = NLOHMANN_JSON_NAMESPACE::json;
	using target = void;
	constexpr static bool value = false;
};

template<typename _Ty>
class instance<json_indicator<_Ty>, true> : public instance<json_indicator<_Ty>, false>
{
public:
	using _Mybase = instance<json_indicator<_Ty>, false>;
};


#endif // !__FILE_CONVENTION_JSON_INSTANCE
