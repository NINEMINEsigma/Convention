#ifndef __FILE_INSTANCE
#define __FILE_INSTANCE

#include "Convention/any_class_instance.h"
#include "Convention/app_instance.h"
#include "Convention/config_instance.h"
#include "Convention/console_instance.h"
#include "Convention/ffmpeg_instance.h"
#include "Convention/file_instance.h"
#include "Convention/music_instance.h"
#include "Convention/number_instance.h"
#include "Convention/process_instance.h"
#include "Convention/stream_instance.h"
#include "Convention/time_instance.h"

template<typename _Type, typename... _Args>
auto make_instance(_Args&&... args)
{
#define oper(target_type)\
if constexpr (internal::is_##target_type##_v<_Type>)\
return instance<target_type##_ex_indicator<_Type>>(std::forward<_Args>(args)...)

	oper(number);
	return instance<_Type>(std::forward<_Args>(args)...);
#undef oper
}
template<typename _Arg, typename _Type = std::decay_t<_Arg>>
auto make_instance(_Arg&& arg)
{
#define oper(target_type)\
if constexpr (internal::is_##target_type##_v<_Type>)\
return instance<target_type##_ex_indicator<_Type>>(std::forward<_Arg>(arg))

	oper(number);
	return instance<_Type>(std::forward<_Arg>(arg));
#undef oper
}

#endif // !__FILE_INSTANCE
