#ifndef __FILE_CONVENTION_CONSOLE_INSTANCE
#define __FILE_CONVENTION_CONSOLE_INSTANCE

#include "Convention/Interface.h"
#ifdef _WINDOWS
#include "Windows.h"
#endif

struct console_indicator
{
#ifdef _WINDOWS
	struct tag
	{
		HANDLE hOutput = nullptr;
		HANDLE hBuffer = nullptr;
		CONSOLE_CURSOR_INFO cci = { DWORD(0),BOOL(FALSE) };
		DWORD bytes = 0;
		char* data = nullptr;
	};
#else
	using tag = std::ostream;
#endif // _WINDOWS
	static constexpr bool value = true;
};
template<>
class instance<console_indicator, true> :public instance<console_indicator::tag, false>
{
	using _Mybase = instance<console_indicator::tag, false>;
public:
	instance() noexcept;
	virtual ~instance();
	instance& set_buffer(_In_ char* data, _Out_opt_ char** erase_);

#ifdef _WINDOWS
	instance& set_cursor(DWORD size = 0, BOOL visible = TRUE);
#endif // _WINDOWS

	instance& refresh(size_t size);
};

#endif // !__FILE_CONVENTION_CONSOLE_INSTANCE
