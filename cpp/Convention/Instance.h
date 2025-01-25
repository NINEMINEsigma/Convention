#ifndef __FILE_INSTANCE
#define __FILE_INSTANCE

#include "Convention/any_class_instance.h"
#include "Convention/app_instance.h"
#include "Convention/ffmpeg_instance.h"
#include "Convention/file_instance.h"
#include "Convention/music_instance.h"
#include "Convention/number_instance.h"
#include "Convention/process_instance.h"
#include "Convention/stream_instance.h"

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
#endif // !__FILE_INSTANCE
