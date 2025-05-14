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
	using object_reference = std::conditional_t<
		std::is_same_v<_Ty, void>, std::any, _Ty
	>&;
	using json = nlohmann::json;
	constexpr static bool value = !std::is_same_v<_Ty, void>;

	template<typename _Input>
	static json parse(const _Input& data);

	static json parse_from_str(const std::string& str);

	static json parse_from_file(const std::istream& stream);

	template<typename _ST>
	static json parse_from_file(const _ST& stream);

	static std::string dump(const json& data, bool isPretty);

	virtual void Serialize(json& data, const object_reference object)
	{
		ObjectSerialize(data, object);
	}

	void ObjectSerialize(json::reference cell, const object_reference object)
	{
		if constexpr (std::is_same_v<
			std::remove_reference_t<object_reference>, std::any
		>)
		{
			const auto& type = object.type();
			if (convention_kit::is_floating_type(type))
			{
				cell = std::any_cast<long double>(object);
			}
			else if (convention_kit::is_integral_type(type))
			{
				if (convention_kit::is_unsigned_integral_type(type))
					cell = std::any_cast<unsigned long long>(object);
				else
					cell = std::any_cast<long long>(object);
			}
			else if (convention_kit::is_string_type(type))
			{
				if (type == typeid(std::string))
					cell = std::any_cast<std::string>(object);
				else if (type == typeid(std::wstring))
					cell = std::any_cast<std::wstring>(object);
				else if (type == typeid(std::u16string))
					cell = std::any_cast<std::u16string>(object);
				else if (type == typeid(std::u32string))
					cell = std::any_cast<std::u32string>(object);
				else if (type == typeid(std::string_view))
					cell = std::any_cast<std::string_view>(object);
				else if (type == typeid(std::wstring_view))
					cell = std::any_cast<std::wstring_view>(object);
				else if (type == typeid(std::u16string_view))
					cell = std::any_cast<std::u16string_view>(object);
				else if (type == typeid(std::u32string_view))
					cell = std::any_cast<std::u32string_view>(object);
				else
				{
					throw std::runtime_error(
						std::string("Unsupported string type for serialization: ") + type.name()
					);
				}
			}
			else
			{
				throw std::runtime_error(
					std::string("Unsupported type for serialization: ") + type.name()
				);
			}
		}
		else if constexpr (!std::is_same_v<_Ty, void>)
			cell = *ptr;
	}

	virtual void Deserialize(json& data, object_reference object)
	{
		ObjectDeserialize(data, object);
	}

	template<typename _OutTy>
	void ObjectDeserialize(json::reference cell, _OutTy& object)
	{
		cell.get_to(object);
	}
};

template<typename _Ty, bool is_extension>
template<typename _Input>
typename json_indicator<_Ty, is_extension>::json 
json_indicator<_Ty, is_extension>::parse(const _Input& data)
{
	return json::parse(data);
}

template<typename _Ty, bool is_extension>
typename json_indicator<_Ty, is_extension>::json  
json_indicator<_Ty, is_extension>::parse_from_str(const std::string& str)
{
	return json::parse(str);
}

template<typename _Ty, bool is_extension>
typename json_indicator<_Ty, is_extension>::json
json_indicator<_Ty, is_extension>::parse_from_file(const std::istream& stream)
{
	return json::parse(stream);
}

template<typename _Ty, bool is_extension>
template<typename _ST>
typename json_indicator<_Ty, is_extension>::json
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
class instance<json_indicator<_Ty, true>, true> 
	: public instance<typename json_indicator<_Ty, true>::json, false>
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
	std::string Serialize(const typename _MyIndictaor::object_reference data)
	{
		GetIndicator().Serialize(**this, data);
		return _MyIndictaor::dump(**this, true);
	}

	template<typename _ObjTy>
	std::string Serialize(const std::string& key,
		const typename _MyIndictaor::object_reference data
	)
	{
		GetIndicator().Serialize(**this[key], data);
		return _MyIndictaor::dump(**this, true);
	}

	void Deserialize(typename _MyIndictaor::object_reference ptr) const
	{
		GetIndicator().Deserialize(**this, ptr);
	}
};

#endif // !__FILE_CONVENTION_JSON_INSTANCE
