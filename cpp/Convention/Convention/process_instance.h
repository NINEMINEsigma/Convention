#ifndef __FILE_CONVENTION_PROCESS_INSTANCER
#define __FILE_CONVENTION_PROCESS_INSTANCER

#include "Convention/Interface.h"

struct process_indicator
{
	using tag = void;
	static constexpr bool value = true;
};
#if defined(_WINDOWS)
#include<Windows.h>
#else

#endif
template<>
class instance<process_indicator, true> :public any_class
{
#if defined(_WINDOWS)
	using start_info_type = STARTUPINFO;
	using process_info_type = PROCESS_INFORMATION;
#else
	using start_info_type = int;
	using process_info_type = int;
#endif // _WINDOW_
private:
	start_info_type start_info;
	process_info_type pinfo;
	bool stats;
public:
	using config_type = int;

	using tag = void;
	constexpr static bool value = true;

	instance();
	instance(const char_indicator::tag* commandline, const config_type& config = 0);
	instance(const char_indicator::tag* executer, const char_indicator::tag* commandline_args, const config_type& config = 0);
public:
	explicit instance(instance&& other) noexcept
	{
		this->move(std::move(other));
	}
	instance& operator=(instance&& other) noexcept
	{
		this->move(std::move(other));
		return *this;
	}
private:
	void move(instance&& other) noexcept
	{
		other.stats = false;
		this->start_info = std::move(other.start_info);
		this->pinfo = std::move(other.pinfo);
	}
public:
	virtual ~instance();
	bool get_stats() const noexcept
	{
		return stats;
	}

	virtual bool exc(const char_indicator::tag* commandline, const config_type& config = 0);
	virtual bool exc(const char_indicator::tag* executer, const char_indicator::tag* commandline_args, const config_type& config = 0);

	virtual bool kill();
	bool is_still_alive();

	void until(std::function<bool(void)> pr)
	{
		while (this->is_still_alive())
			if (pr() == false)
				this->kill();
	}
	void until(std::function<bool(void)> pr, std::function<void(void)> sleeper)
	{
		while (this->is_still_alive())
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

#endif // !__FILE_CONVENTION_PROCESS_INSTANCER
