#ifndef __FILE_CONVENTION_PROCESS_INSTANCER
#define __FILE_CONVENTION_PROCESS_INSTANCER

#include "Convention/instance/Interface.h"

#if defined(_WIN64)||defined(_WIN32)
#include<Windows.h>
#else

#endif

namespace Convention
{

	struct ProcessIndicator
	{
#if defined(_WINDOWS)
		using start_info_type = STARTUPINFO;
		using process_info_type = PROCESS_INFORMATION;
#else
		using start_info_type = int;
		using process_info_type = int;
#endif // _WINDOW_
		struct tag
		{
			start_info_type start_info = {};
			process_info_type pinfo = {};
			bool stats = false;
		};
		static constexpr bool value = true;
	};

	template<template<typename> class Allocator>
	class instance<ProcessIndicator::tag, true, Allocator, true> 
		: public instance<ProcessIndicator::tag, false, Allocator, true>
	{
	private:
		using _Mybase = instance<ProcessIndicator::tag, false, Allocator, true>;
	public:
		using config_type = int;

		using tag = void;
		constexpr static bool value = true;

		instance() :_Mybase(BuildMyPtr()) {}
		instance(const CharIndicator::tag* commandline, const config_type& config = 0) : instance()
		{
			this->exc(commandline, config);
		}
		instance(const CharIndicator::tag* executer, const CharIndicator::tag* commandline_args, const config_type& config = 0) : instance()
		{
			this->exc(executer, commandline_args, config);
		}
		instance(instance&& other) noexcept : _Mybase(std::move(other)) {}
		instance& operator=(instance&& other) noexcept
		{
			_Mybase::operator=(std::move(other));
			return *this;
		}
	public:
		virtual ~instance()
		{
			if (this->ReadValue().stats)
			{
				WaitForSingleObject(this->ReadConstValue().pinfo.hProcess, INFINITE);
				CloseHandle(this->ReadConstValue().pinfo.hProcess);
				CloseHandle(this->ReadConstValue().pinfo.hThread);
			}
		}
		bool ReadStats() const noexcept
		{
			return this->ReadConstValue().stats;
		}

		virtual bool exc(const CharIndicator::tag* commandline, const config_type& config = 0)
		{
			memset(&this->ReadValue().start_info, 0, sizeof(this->ReadConstValue().start_info));
			CharIndicator::tag commandline_c[255] = { 0 };
			StringIndicator::strcpy(commandline_c, commandline);
			this->ReadValue().start_info.cb = sizeof(this->ReadConstValue().start_info);
			if (CreateProcess(
				0,
				commandline_c,
				0,
				0,
				FALSE,
				NORMAL_PRIORITY_CLASS,
				0,
				0,
				&this->ReadValue().start_info,
				&this->ReadValue().pinfo))
			{
				this->ReadValue().stats = true;
			}
			else
				this->ReadValue().stats = false;
			return this->ReadConstValue().stats;
		}
		virtual bool exc(const CharIndicator::tag* executer, const CharIndicator::tag* commandline_args, const config_type& config = 0)
		{
			memset(&this->ReadValue().start_info, 0, sizeof(this->ReadValue().start_info));
			this->ReadValue().start_info.cb = sizeof(this->ReadConstValue().start_info);
			CharIndicator::tag cammand_c_args[255] = { 0 };
			StringIndicator::strcpy(cammand_c_args, commandline_args);
			if (CreateProcess(
				executer,
				cammand_c_args,
				0,
				0,
				FALSE,
				NORMAL_PRIORITY_CLASS,
				0,
				0,
				&this->ReadValue().start_info,
				&this->ReadValue().pinfo))
			{
				this->ReadValue().stats = true;
			}
			else
				this->ReadValue().stats = false;
			return this->ReadValue().stats;
		}

		virtual bool kill()
		{
			if (this->ReadStats())
			{
				if (this->IsStillAlive())
				{
					TerminateProcess(this->ReadConstValue().pinfo.hProcess, 0);
					CloseHandle(this->ReadConstValue().pinfo.hProcess);
					CloseHandle(this->ReadConstValue().pinfo.hThread);
				}
				this->ReadValue().stats = false;
				return true;
			}
			return false;
		}
		bool IsStillAlive()
		{
			if (this->ReadStats())
			{
				DWORD code;
				if (GetExitCodeProcess(this->ReadConstValue().pinfo.hProcess, &code) == TRUE)
				{
					bool result = code == STILL_ACTIVE;
					this->ReadValue(). stats = result;
					return result;
				}
			}
			return false;
		}

		void until(std::function<bool(void)> pr)
		{
			while (this->IsStillAlive())
				if (pr() == false)
					this->kill();
		}
		void until(std::function<bool(void)> pr, std::function<void(void)> sleeper)
		{
			while (this->IsStillAlive())
			{
				sleeper();
				if (pr() == false)
					this->kill();
			}
		}
		void until(_Inout_ std::thread& th, std::function<bool(void)> pr)
		{
			th.operator=(std::thread([this, pr]()
				{
					this->until(pr);
				}));
		}
		void until(_Inout_ std::thread& th, std::function<bool(void)> pr, std::function<void(void)> sleeper)
		{
			th.operator=(std::thread([this, pr, sleeper]()
				{
					this->until(pr, sleeper);
				}));
		}
	};

}

#endif // !__FILE_CONVENTION_PROCESS_INSTANCER
