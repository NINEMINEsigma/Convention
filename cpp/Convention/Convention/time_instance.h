#ifndef __FILE_CONVENTION_TIME_INSTANCE
#define __FILE_CONVENTION_TIME_INSTANCE

#include "Convention/Interface.h"

template<>
class instance<
	std::chrono::time_point<std::chrono::high_resolution_clock>
	, true
> :public instance<
	std::chrono::time_point<std::chrono::high_resolution_clock>,
	false
>
{
public:
	using _Clock = std::chrono::high_resolution_clock;
	using _TimePoint = std::chrono::time_point<_Clock>;
	using _Mybase = instance<_TimePoint, false>;
private:
	_TimePoint m_begin;
	_TimePoint m_end;
public:
	class localized_clocker :public any_class
	{
		_TimePoint m_begin;
		instance<_TimePoint, true>& parent;
	public:
		localized_clocker(instance<_TimePoint, true>& parent)
			: m_begin(_Clock::now()),
			parent(parent) {}
		localized_clocker(localized_clocker&& other) noexcept
			:m_begin(_Clock::now()),
			parent(other.parent) {}
		virtual ~localized_clocker()
		{
			parent.m_begin = this->m_begin;
			parent.m_end = _Clock::now();
		}

		void reset() 
		{
			m_begin = _Clock::now();
		}

		template<typename _TimeUnit=std::chrono::milliseconds>
		int64_t duration() const
		{
			return std::chrono::duration_cast<_TimeUnit>(_Clock::now() - m_begin).count();
		}

		double seconds() const
		{
			return duration<std::chrono::milliseconds>() * 0.001;
		}
	};

	instance() :_Mybase(new _TimePoint(_Clock::now())) {};
	instance(const instance& other) :_Mybase(other)
	{
		this->m_begin = other.m_begin;
		this->m_end = other.m_end;
	}

	auto get_create_time() const
	{
		return **this;
	}
	decltype(auto) create_time() const
	{
		return **this;
	}
	auto get_begin_time() const
	{
		return this->m_begin;
	}
	auto get_end_time() const
	{
		return this->m_end;
	}

	template<typename _TimeUnit = std::chrono::milliseconds>
	int64_t from_create_duration() const
	{
			return std::chrono::duration_cast<_TimeUnit>(_Clock::now() - **this).count();
	}
	double from_create_seconds() const
	{
		return from_create_duration<std::chrono::milliseconds>() * 0.001;
	}

	auto make_localized_clocker()
	{
		return localized_clocker(*this);
	}
	template<typename _TimeUnit = std::chrono::milliseconds>
	int64_t last_localized_clocker_duration() const
	{
		return std::chrono::duration_cast<_TimeUnit>(m_end - m_begin).count();
	}
	double last_localized_clocker_seconds() const
	{
		return last_localized_clocker_duration<std::chrono::milliseconds>() * 0.001;
	}
};

#endif // !__FILE_CONVENTION_TIME_INSTANCE
