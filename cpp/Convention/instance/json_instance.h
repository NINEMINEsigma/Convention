#ifndef __FILE_CONVENTION_JSON_INSTANCE
#define __FILE_CONVENTION_JSON_INSTANCE

#include "Convention/instance/nlohmann/json.hpp"
//#include "Convention/instance/nlohmann/json_fwd.hpp"
using namespace nlohmann::literals;

#include "Convention/instance/file_instance.h"

template<typename _Ty, bool is_extension = false>
struct json_indicator
{
	using tag = _Ty;
	using json = nlohmann::json;
	constexpr static bool value = !std::is_same_v<_Ty, void>;

	template<typename _Input>
	static auto parse(const _Input& data);

	static auto parse_from_str(const std::string& str);

	static auto parse_from_file(const std::istream& stream)
	{
		return json::parse(stream);
	}

	template<typename _ST>
	static auto parse_from_file(const _ST& stream);
};

template<typename _Ty, bool is_extension>
template<typename _Input>
auto json_indicator<_Ty, is_extension>::parse(const _Input& data)
{
	return json::parse(data);
}

template<typename _Ty, bool is_extension>
auto json_indicator<_Ty, is_extension>::parse_from_str(const std::string& str)
{
	return json::parse(str);
}

template<typename _Ty, bool is_extension>
auto json_indicator<_Ty, is_extension>::parse_from_file(const istream& stream)
{
	return json::parse(stream);
}

template<typename _Ty, bool is_extension>
template<typename _ST>
auto json_indicator<_Ty, is_extension>::parse_from_file(const _ST& stream)
{
	return json::parse(stream);
}

template<typename _Ty>
class instance<json_indicator<_Ty, true>, true> : public instance<json_indicator<_Ty, true>::json, false>
{
public:
	using _MyInside = json_indicator<_Ty, true>;
	using json = typename _MyInside::json;
	using _Mybase = instance<json, false>;
	using tag = _Ty;

	instance(const std::string& data, bool is_raw_string = false) :
		_Mybase(new json(is_raw_string
			? _MyInside::parse_from_str(data)
			: _MyInside::parse_from_file(std::ifstream(data))
		)) {}
	instance(const std::istream& stream) : _Mybase(new json(json::parse(stream))) {}
	instance(const tool_file& file) : _Mybase(new json(json::parse(file.get_stream(std::ios_base::in)))) {}
	instance(const json& data) noexcept : _Mybase(new json(data)) {}
	instance(json&& data) noexcept : _Mybase(new json(std::move(data))) {}
	instance& operator=(const json& data) noexcept
	{
		**this = data;
		return *this;
	}
	instance& operator=(json&& data) noexcept
	{
		**this = std::move(data);	
		return *this;
	}
	instance_move_operator(public) {}
	virtual ~instance() {}

	using object_ret_type = typename std::conditional<std::is_same_v<void, _Ty>, std::any, _Ty>::type;

	std::string Serialize(const object_ret_type& data)
	{
		if constexpr (std::is_same_v<void, _Ty>)
		{

		}
		else
		{

		}
		return *this;
	}

	object_ret_type Deserialize() const
	{
		if constexpr (std::is_same_v<void, _Ty>)
		{
			return std::any();
		}
		else
		{
			return _Mybase::get();
		}
	}
};



#endif // !__FILE_CONVENTION_JSON_INSTANCE
