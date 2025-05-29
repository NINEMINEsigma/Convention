#ifndef CONVENTION_KIT_INSTANCE_H
#define CONVENTION_KIT_INSTANCE_H

#include "Convention/instance/AnyClassInstance.h"
#include "Convention/instance/AppInstance.h"
#include "Convention/instance/ConfigInstance.h"
#include "Convention/instance/ConsoleInstance.h"
#include "Convention/instance/FfmpegInstance.h"
#include "Convention/instance/FileInstance.h"
#include "Convention/instance/MusicInstance.h"
#include "Convention/instance/NumberInstance.h"
#include "Convention/instance/ProcessInstance.h"
#include "Convention/instance/StreamInstance.h"
#include "Convention/instance/TimeInstance.h"
#include "Convention/instance/StdInstance.h"
#include "Convention/instance/WebInstance.h"
#include "Convention/instance/JsonInstance.h"

template<typename TType, typename... TArgs>
auto MakeInstance(TArgs&&... args)
{
#define MAKE_INSTANCE_OPERATOR(targetType)\
if constexpr (Internal::Is##targetType##V<TType>)\
return Instance<targetType##ExIndicator<TType>>(std::forward<TArgs>(args)...)

	MAKE_INSTANCE_OPERATOR(Number);
	return Instance<TType>(std::forward<TArgs>(args)...);
#undef MAKE_INSTANCE_OPERATOR
}

template<typename TArg, typename TType = std::decay_t<TArg>>
auto MakeInstance(TArg&& arg)
{
#define MAKE_INSTANCE_OPERATOR(targetType)\
if constexpr (Internal::Is##targetType##V<TType>)\
return Instance<targetType##ExIndicator<TType>>(std::forward<TArg>(arg))

	MAKE_INSTANCE_OPERATOR(Number);
	return Instance<TType>(std::forward<TArg>(arg));
#undef MAKE_INSTANCE_OPERATOR
}

#endif // !CONVENTION_KIT_INSTANCE_H
