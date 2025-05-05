#ifndef __FILE_INSTANCE
#define __FILE_INSTANCE

#include "Convention/instance/any_class_instance.h"
#include "Convention/instance/app_instance.h"
#include "Convention/instance/config_instance.h"
#include "Convention/instance/console_instance.h"
#include "Convention/instance/ffmpeg_instance.h"
#include "Convention/instance/file_instance.h"
#include "Convention/instance/music_instance.h"
#include "Convention/instance/number_instance.h"
#include "Convention/instance/process_instance.h"
#include "Convention/instance/stream_instance.h"
#include "Convention/instance/time_instance.h"
#include "Convention/instance/std_instance.h"
#include "Convention/instance/web_instance.h"
#include "Convention/instance/json_instance.h"

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
