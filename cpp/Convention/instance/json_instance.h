#ifndef CONVENTION_KIT_JSON_INSTANCE_H
#define CONVENTION_KIT_JSON_INSTANCE_H

#include "Convention/instance/nlohmann/json.hpp"
using namespace nlohmann::literals;

// 使用json_indicator<_Ty, true>继承json_indicator<_Ty, false>
// 以实现功能
template<typename TType, bool TExtension = false>
struct json_indicator
{
	using tag = TType;
	using object_reference = std::conditional_t<
		std::is_same_v<TType, void>, std::any, TType
	>&;
	using json = nlohmann::json;
	constexpr static bool value = !std::is_same_v<TType, void>;

	template<typename TInput>
	static json Parse(const TInput& data);

	static json ParseFromStr(const std::string& str);

	static json ParseFromFile(const std::istream& stream);

	static std::string Dump(const json& data, bool isPretty);

	virtual void Serialize(json& data, const object_reference object)
	{
		auto&& cell = data;
		if constexpr (std::is_same_v<
			std::remove_reference_t<object_reference>, std::any
		>)
		{
			const auto& type = object.type();
			if (ConventionKit::IsFloatingType(type))
			{
				cell = std::any_cast<long double>(object);
			}
			else if (ConventionKit::IsIntegralType(type))
			{
				if (ConventionKit::IsUnsignedIntegralType(type))
					cell = std::any_cast<unsigned long long>(object);
				else
					cell = std::any_cast<long long>(object);
			}
			else if (ConventionKit::IsStringType(type))
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
			else if (type == typeid(bool))
			{
				cell = std::any_cast<bool>(object);
			}
			else if (type == typeid(char))
			{
				cell = std::any_cast<char>(object);
			}
			else
			{
				throw std::runtime_error(
					std::string("Unsupported type for serialization: ") + type.name()
				);
			}
		}
		else if constexpr (!std::is_same_v<TType, void>)
			cell = object;
	}

	virtual void Deserialize(json& data, object_reference object)
	{
		auto&& cell = data;
		cell.get_to(object);
	}
};

template<typename TType, bool TExtension>
template<typename TInput>
typename json_indicator<TType, TExtension>::json
json_indicator<TType, TExtension>::Parse(const TInput& data)
{
	return json::parse(data);
}

template<typename TType, bool TExtension>
typename json_indicator<TType, TExtension>::json
json_indicator<TType, TExtension>::ParseFromStr(const std::string& str)
{
	return json::parse(str);
}

template<typename TType, bool TExtension>
typename json_indicator<TType, TExtension>::json
json_indicator<TType, TExtension>::ParseFromFile(const std::istream& stream)
{
	throw std::runtime_error("not support");
}

template<typename TType, bool TExtension>
std::string json_indicator<TType, TExtension>::Dump(const json& data, bool isPretty)
{
	if (isPretty)
		return data.dump();
	else
		return data.dump(4);
}

// 使用json_indicator<_Ty, true>继承json_indicator<_Ty, false>
// 以实现功能
template<typename TType>
class instance<json_indicator<TType, true>, true>
	: public instance<typename json_indicator<TType, true>::json, false>
{
public:
	using TIndicator = json_indicator<TType, true>;
	using json = typename TIndicator::json;
	using TInside = typename TIndicator::json;
	using TMybase = instance<json, false>;
	using tag = TType;

	constexpr const TIndicator& GetIndicator() const
	{
		static TIndicator indicator;
		return indicator;
	}

	instance(const std::string& data, bool isRawString = false) :
		TMybase(new json(isRawString
			? TIndicator::ParseFromStr(data)
			: TIndicator::ParseFromFile(std::ifstream(data))
		)) {}
	instance(const std::istream& stream) : TMybase(new json(json::parse(stream))) {}
	instance(const tool_file& file) : TMybase(new json(json::parse(file.GetStream(std::ios_base::in)))) {}
	instance(const json& data) noexcept : TMybase(new json(data)) {}
	instance(json&& data) noexcept : TMybase(new json(std::move(data))) {}
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

	template<typename TObjType>
	std::string Serialize(const typename TIndicator::object_reference data)
	{
		GetIndicator().Serialize(**this, data);
		return TIndicator::Dump(**this, true);
	}

	template<typename TObjType>
	std::string Serialize(const std::string& key,
		const typename TIndicator::object_reference data
	)
	{
		GetIndicator().Serialize(**this[key], data);
		return TIndicator::Dump(**this, true);
	}

	void Deserialize(typename TIndicator::object_reference ptr) const
	{
		GetIndicator().Deserialize(**this, ptr);
	}

	void Deserialize(const std::string& key, typename TIndicator::object_reference ptr) const
	{
		GetIndicator().Deserialize((**this)[key], ptr);
	}

	auto&& operator[](const std::string& key)
	{
		return (**this)[key];
	}
};

#endif // !CONVENTION_KIT_JSON_INSTANCE_H
