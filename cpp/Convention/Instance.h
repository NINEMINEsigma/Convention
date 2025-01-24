#ifndef __FILE_INSTANCE
#define __FILE_INSTANCE

#include "Convention/number_instance.h"
#include "Convention/process_instance.h"
#include "Convention/ffmpeg_instance.h"
#include "Convention/music_instance.h"
#include "Convention/file_instance.h"
#include "Convention/iostream_instance.h"

template<typename _Type>
instance<_Type>&& make_instance(_Type&& value)
{
	return instance<_Type>(std::make_shared<_Type>(std::forward<_Type>(value)));
}

#endif // !__FILE_INSTANCE
