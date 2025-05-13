#ifndef __FILE_CONVENTION_JSON_INSTANCE
#define __FILE_CONVENTION_JSON_INSTANCE

#include "Convention/instance/nlohmann/json.hpp"
using namespace nlohmann::literals;

// 使用json_indicator<_Ty, true>继承json_indicator<_Ty, false>
// 以实现功能
template<typename _Ty, bool is_extension = false>
struct json_indicator
{
	using tag = _Ty;
	using json = nlohmann::json;
	constexpr static bool value = !std::is_same_v<_Ty, void>;

	template<typename _Input>
	static json parse(const _Input& data);

	static json parse_from_str(const std::string& str);

	static json parse_from_file(const std::istream& stream)
	{
		return json::parse(stream);
	}

	template<typename _ST>
	static json parse_from_file(const _ST& stream);

	static std::string dump(const json& data, bool isPretty);

	virtual void Serialize(json& data, _In_ const _Ty* const ptr)
	{
		data["value"] = *ptr;
	}

	virtual void Deserialize(json& data, _Out_ _Ty* ptr)
	{
		data.at("value").get_to(*ptr);
	}

	void Deserialize(const std::string& data, _Out_ _Ty* ptr)
	{
		Deserialize(parse_from_str(data), ptr);
	}
};

template<typename _Ty, bool is_extension>
template<typename _Input>
json_indicator<_Ty, is_extension>::json 
json_indicator<_Ty, is_extension>::parse(const _Input& data)
{
	return json::parse(data);
}

template<typename _Ty, bool is_extension>
json_indicator<_Ty, is_extension>::json  
json_indicator<_Ty, is_extension>::parse_from_str(const std::string& str)
{
	return json::parse(str);
}

template<typename _Ty, bool is_extension>
json_indicator<_Ty, is_extension>::json
json_indicator<_Ty, is_extension>::parse_from_file(const std::istream& stream)
{
	return json::parse(stream);
}

template<typename _Ty, bool is_extension>
template<typename _ST>
json_indicator<_Ty, is_extension>::json
json_indicator<_Ty, is_extension>::parse_from_file(const _ST& stream)
{
	return json::parse(stream);
}

template<typename _Ty, bool is_extension>
std::string json_indicator<_Ty, is_extension>::dump(const json& data, bool isPretty)
{
	if (isPretty)
		return data.dump();
	else
		return data.dump(4);
}

// 使用json_indicator<_Ty, true>继承json_indicator<_Ty, false>
// 以实现功能
template<typename _Ty>
class instance<json_indicator<_Ty, true>, true> : public instance<json_indicator<_Ty, true>::json, false>
{
public:
	using _MyIndictaor = json_indicator<_Ty, true>;
	using json = typename _MyIndictaor::json;
	using _MyInside = typename _MyIndictaor::json;
	using _Mybase = instance<json, false>;
	using tag = _Ty;

	constexpr const _MyIndictaor& GetIndicator() const
	{
		static _MyIndictaor indicator;
		return indicator;
	}

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

	template<typename _ObjTy>
	std::string Serialize(_In_opt_ const _ObjTy* const data)
	{
		GetIndicator().Serialize(**this, data);
		return _MyIndictaor::dump(**this, true);
	}

	void Deserialize(_Out_ _Ty* ptr) const
	{
		GetIndicator().Deserialize(**this, ptr);
	}
};



#endif // !__FILE_CONVENTION_JSON_INSTANCE
