﻿#ifndef CONVENTION_KIT_FILE_INSTANCE_H
#define CONVENTION_KIT_FILE_INSTANCE_H

#include "Convention/instance/Interface.h"
#include <filesystem>
#include <optional>
#include <any>
#include "Convention/instance/StreamInstance.h"

#include "Convention/instance/nlohmann/json.hpp"

namespace Convention
{

	extern bool IsBinaryFile(const std::filesystem::path& path);
	extern std::filesystem::path GetExtensionName(const std::filesystem::path& path);
	extern std::filesystem::path GetBaseFilename(const std::filesystem::path& path);

	template<template<typename> class Allocator>
	class instance<std::filesystem::path, true, Allocator, false>
		: public instance<std::filesystem::path, false, Allocator, false>
	{
	private:
		using _Mybase = instance<std::filesystem::path, false, Allocator, false>;
		using _MyStream = instance<std::ios_base, true, Allocator, >;
	public:
		using buffer_type = std::basic_string<size_t>;

		using path = std::filesystem::path;

		using DataType = std::optional<std::any>;

		_MyStream stream = nullptr;

		/**
		* @brief 构造函数
		* @param data 路径
		*/
		instance(path data) :_Mybase(BuildMyPtr(data)) {}
		/**
		* @brief 使用当前目录构造
		*/
		instance() :instance("./") {}
		/**
		* @brief 拷贝构造函数, 只拷贝路径
		*/
		instance(const instance& data) noexcept :_Mybase(data) {}
		/**
		* @brief 拷贝赋值函数, 只拷贝路径
		*/
		instance& operator=(const instance& data) noexcept
		{
			_Mybase::operator=(data);
			return *this;
		}
		/**
		* @brief 移动构造函数, 只移动路径
		*/
		instance(instance&& data) noexcept : _Mybase(std::move(data)) {}
		/**
		* @brief 移动赋值函数, 只移动路径
		*/
		instance& operator=(instance&& other) noexcept
		{
			_Mybase::operator=(std::move(other));
			return *this;
		}
		virtual ~instance() {}

		/**
		* @brief 获取文件名
		* @param 是否忽略文件扩展名
		*/
		path GetFilename(bool isWithoutExtension = false) const
		{
			std::string cur = this->ReadConstValue().filename().string();
			if (isWithoutExtension && this->ReadConstValue().has_extension())
			{
				return cur.substr(0, cur.find_last_of('.'));
			}
			else if (cur.back() == '\\' || cur.back() == '/')
				return cur.substr(0, cur.size() - 1);
			return cur;
		}
		virtual std::string ToString() const noexcept override
		{
			return this->GetFilename().string();
		}
		virtual std::string SymbolName() const noexcept override
		{
			return StringIndicator::Combine<std::string>(
				"file<",
				this->Exist() ? "e" : "-",
				this->IsDir() ? "d" : "-",
				">"
			);
		}

		// get target path's stats

		/**
		* @brief 是否是目录(文件夹), 若不存在, 则根据路径最后一个字符判断
		*/
		bool IsDir() const noexcept
		{
			auto endchar = this->ReadConstValue().string().back();
			if (endchar == '/' || endchar == '\\')
				return true;
			return std::filesystem::is_directory(**this);
		}
		/**
		* @brief 是否是文件(实际判断是否为非目录)
		*/
		bool IsFile() const noexcept
		{
			return !this->IsDir();
		}
		/**
		* @brief 是否是块特殊文件
		*/
		bool IsBlockFile() const noexcept
		{
			return std::filesystem::is_block_file(**this);
		}
		/**
		* @brief 是否是字符特殊文件
		*/
		bool IsCharacterFile() const noexcept
		{
			return std::filesystem::is_character_file(**this);
		}
		/**
		* @brief 是否是空目录或空文件
		*/
		bool IsEmpty() const 
		{
			return std::filesystem::is_empty(**this);
		}
		/**
		* @brief 是否是空目录
		*/
		bool IsDirEmpty() const
		{
			return IsDir() && std::filesystem::is_empty(**this);
		}
		/**
		* @brief 是否是空文件
		*/
		bool IsFileEmpty() const
		{
			return IsFile() && std::filesystem::is_empty(**this);
		}
		/**
		* @brief 猜测是否为二进制文件
		*/
		bool IsBinaryFile() const
		{
			return Convention::IsBinaryFile(**this);
		}

		// operators

		/**
		* @brief 重新打开
		* @param data 新的路径
		*/
		instance& Open(path data)
		{
			this->WriteValue(std::move(data));
			return *this;
		}
		/**
		* 
		*/
		instance& Open(std::ios::openmode mode)
		{
			auto* ptr = new std::fstream(**this, mode);
			this->stream = ptr;
			return *this;
		}
		instance& Close()
		{
			if (this->stream.IsEmpty() == false)
			{
				this->stream = nullptr;
			}
			return *this;
		}
		bool Exist() const noexcept
		{
			return std::filesystem::exists(**this);
		}
		void Create() const noexcept
		{
			if (this->Exist())
				return;
			if (this->IsDir())
				std::filesystem::create_directory(**this);
			else
			{
				std::ofstream of(**this, std::ios::out);
			}
		}
		bool IsPathEmpty() const noexcept
		{
			return this->get()->empty();
		}
		instance& Rename(path newName)
		{
			std::filesystem::rename(**this, newName);
			**this = newName;
			return *this;
		}
		void CopyC(const path& target) const
		{
			if (target.has_root_directory())
			{
				std::filesystem::copy(**this, target);
			}
			else
			{
				path cur = this->get()->parent_path() / target;
				std::filesystem::copy(**this, cur);
			}
		}
		instance& Copy(const path& target)
		{
			this->CopyC(target);
			return *this;
		}
		instance& Move(const path& target)
		{
			if (target.has_root_directory())
			{
				std::filesystem::copy(**this, target);
				std::filesystem::remove(**this);
				**this = target;
			}
			else
			{
				path cur = this->get()->parent_path() / target;
				std::filesystem::copy(**this, cur);
				std::filesystem::remove(**this);
				**this = cur;
			}
			return *this;
		}
		void DeleteC() const
		{
			if (this->IsDir())
				std::filesystem::remove_all(**this);
			else
				std::filesystem::remove(**this);
		}
		instance& Remove()
		{
			std::filesystem::remove(**this);
			return *this;
		}
		instance& RemoveAll()
		{
			std::filesystem::remove_all(**this);
			return *this;
		}
		instance& MustExistPath() noexcept
		{
			this->stream = nullptr;
			this->TryCreateParentPathC();
			this->Create();
			return *this;
		}
		auto GetStream(std::ios::openmode mode) const
		{
			return std::fstream(**this, mode);
		}
		auto GetWStream(std::ios::openmode mode) const
		{
			return std::wfstream(**this, mode);
		}

		void TryCreateParentPathC() const
		{
			path dir = **this;
			if (dir.has_parent_path())
			{
				dir = dir.parent_path();
				std::filesystem::create_directories(dir);
			}
		}
		instance& TryCreateParentPath()
		{
			this->TryCreateParentPathC();
			return *this;
		}
		auto DirIter() const
		{
			return std::filesystem::directory_iterator(**this);
		}
		std::vector<instance> DirInstanceIter() const
		{
			std::vector<instance> result;
			for (auto&& index : this->DirIter())
			{
				result.push_back(instance(index.path()));
			}
			return result;
		}
		instance& BackToParentDir()
		{
			**this = this->get()->parent_path();
			return *this;
		}
		instance GetParentDir()
		{
			return this->get()->parent_path();
		}
		size_t DirCount() const
		{
			size_t result = 0;
			for (auto&& _ : this->DirIter())
				result++;
			return result;
		}
		void DirClearC() const
		{
			for (auto&& index : this->DirIter())
				std::filesystem::remove(index);
		}
		instance& DirClear()
		{
			this->DirClearC();
			return *this;
		}
		// if found return it, otherwise return myself
		instance FirstFileWithExtension(const std::string& extension) const
		{
			for (auto&& index : this->DirIter())
				if (index.path().extension() == extension)
					return instance(index.path());
			return instance(static_cast<_shared>(*this));
		}
		// if found return it, otherwise return myself
		instance FirstFileWithName(const std::string& name) const
		{
			for (auto&& index : this->DirIter())
			{
				auto cur = index.path();
				if (cur.has_filename() == false)continue;
				auto sv = cur.filename().string();
				if (name.size() <= sv.size() && 0 == decltype(sv)::traits_type::compare(sv.data(), name.data(), name.size()))
					return instance(cur);
			}
			return instance(static_cast<_shared>(*this));
		}
		// if found return it, otherwise return myself
		instance FirstFile(std::function<bool(const path&)> pr) const
		{
			for (auto&& index : this->DirIter())
				if (pr(index))
					return instance(index.path());
			return instance(static_cast<_shared>(*this));
		}
		// if found return it, otherwise return myself
		instance FirstFile(std::function<bool(const instance&)> pr) const
		{
			for (auto&& index : this->DirInstanceIter())
				if (pr(index))
					return instance(std::move(index));
			return instance(static_cast<_shared>(*this));
		}
		instance& MakeFileInside(instance& data, bool isDeleteSource = false)
		{
			if (this->IsDir() == false)
				throw std::filesystem::filesystem_error(
					"Cannot make file inside a file, because this object target is not a directory",
					**this,
					std::make_error_code(std::errc::is_a_directory)
				);
			auto result = *this | data.GetFilename();
			if (isDeleteSource)
				data.Move(result.GetFilename());
			else
				data.Copy(result.GetFilename());
			return *this;
		}

		// operators

		instance operator|(const path& next) const noexcept
		{
			return instance(**this / next);
		}
		instance operator|(nullptr_t) const
		{
			if (this->IsDir())
				return instance(static_cast<_shared>(*this));
			return instance(this->get()->string() + "/");
		}
		instance operator|(_shared ptr) const
		{
			return instance(**this / (*ptr));
		}

		// stream operators

		template<typename TType>
		decltype(auto) operator<<(const TType& value)
		{
			this->stream << value;
			return this->stream;
		}
		template<typename TType>
		decltype(auto) operator>>(TType& value)
		{
			this->stream >> value;
			return this->stream;
		}
		template<typename TStreamType>
		decltype(auto) GetStream() const
		{
			if constexpr (std::is_pointer_v<TStreamType>)
				return dynamic_cast<TStreamType>(this->stream.get());
			else if constexpr (std::is_lvalue_reference_v<TStreamType>)
				return dynamic_cast<TStreamType>(*this->stream);
			else if constexpr (std::is_rvalue_reference_v<TStreamType>)
				return dynamic_cast<TStreamType>(std::move(*this->stream));
			else
				return dynamic_cast<TStreamType&>(*this->stream);
		}
		template<typename... TArgs>
		decltype(auto) SetStream(TArgs... args)
		{
			this->stream = _MyStream(std::forward<TArgs>(args)...);
			return *this;
		}

		void Write(const std::string& data)
		{
			this->GetStream<std::ofstream&>() << data;
		}
		void Write(_In_ const char* data, size_t size)
		{
			this->GetStream<std::ofstream&>().write(data, size);
		}

		using MonitorCallback = std::function<void(const std::string&, const path&)>;
		instance& Compress(const path& outputPath = "", const std::string& format = "cab");
		instance& Decompress(const path& outputPath = "");
		instance& Encrypt(const std::string& key, const std::string& algorithm = "AES");
		instance& Decrypt(const std::string& key, const std::string& algorithm = "AES");
		std::string CalculateHash(const std::string& algorithm = "md5");
		void StartMonitoring(
			MonitorCallback callback,
			bool recursive = false,
			const std::vector<std::string>& ignorePatterns = {},
			bool ignoreDirectories = false
		);
		instance CreateBackup(
			const path& backupDir = "",
			size_t maxBackups = 5,
			const std::string& backupFormat = "bak",
			bool includeMetadata = true
		);
		std::map<std::string, bool> GetPermissions();
		instance& SetPermissions(
			std::optional<bool> read = std::nullopt,
			std::optional<bool> write = std::nullopt,
			std::optional<bool> execute = std::nullopt,
			bool recursive = false
		);
	private:

	public:
		DataType data;
		using json = nlohmann::json;

		static std::vector<std::string> textReadableFileType;
		static std::vector<std::string> audioFileType;
		static std::vector<std::string> imageFileType;
		static std::string tempToolFilePathName;

#pragma region Load

		DataType& Load()
		{
			return this->data;
		}

		auto LoadAsJson()
		{
			this->data = json::parse(this->get()->string());
			return std::any_cast<json>(this->data);
		}

#pragma endregion

#pragma region Save

		template<typename TDataType = void>
		void Save() const
		{

		}
		template<typename TDataType = void>
		void SaveAsJson() const
		{
			SaveAsJson<TDataType>(**this);
		}
		template<typename TDataType, typename TPath>
		void SaveAsJson(const TPath& newPath) const
		{
			const std::filesystem::path path = newPath;
			Open(std::ios::out);
			json jsonData;
			if (data.has_value())
			{
				auto& realData = data.value();
				if constexpr (std::is_same_v<TDataType, json>)
				{
					jsonData = std::any_cast<json>(realData);
				}
				else
				{
					if (realData.type() == typeid(json))
					{
						jsonData = std::any_cast<json>(realData);
					}
					else
					{
						jsonData = json(std::any_cast<TDataType>(realData));
					}
				}
				auto str = jsonData.dump(4);
				this->Write(str);
			}
		}


#pragma endregion


	};

	template<template<typename> class Allocator = std::allocator>
	using ToolFile = instance<std::filesystem::path, true, Allocator, false>;

}

#endif // !CONVENTION_KIT_FILE_INSTANCE_H
