//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.12.0
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013 - 2025 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#pragma once

// restore GCC/clang diagnostic settings
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif

// clean up
#undef JSON_ASSERT
#undef JSON_INTERNAL_CATCH
#undef JSON_THROW
#undef JSON_PRIVATE_UNLESS_TESTED
#undef NLOHMANN_BASIC_JSON_TPL_DECLARATION
#undef NLOHMANN_BASIC_JSON_TPL
#undef JSON_EXPLICIT
#undef NLOHMANN_CAN_CALL_STD_FUNC_IMPL
#undef JSON_INLINE_VARIABLE
#undef JSON_NO_UNIQUE_ADDRESS
#undef JSON_DISABLE_ENUM_SERIALIZATION
#undef JSON_USE_GLOBAL_UDLS

#if defined(JSON_HAS_CPP_20)
    #undef JSON_HAS_CPP_20
#endif
#if defined(JSON_HAS_CPP_17)
    #undef JSON_HAS_CPP_17
#endif
#if defined(JSON_HAS_CPP_14)
    #undef JSON_HAS_CPP_14
#endif
#if defined(JSON_HAS_CPP_11)
    #undef JSON_HAS_CPP_11
#endif

#if defined(JSON_HAS_FILESYSTEM)
    #undef JSON_HAS_FILESYSTEM
#endif
#if defined(JSON_HAS_EXPERIMENTAL_FILESYSTEM)
    #undef JSON_HAS_EXPERIMENTAL_FILESYSTEM
#endif

#if defined(JSON_HAS_THREE_WAY_COMPARISON)
    #undef JSON_HAS_THREE_WAY_COMPARISON
#endif

#ifndef JSON_TEST_KEEP_MACROS
    #undef JSON_CATCH
    #undef JSON_TRY
    #undef JSON_HAS_CPP_11
    #undef JSON_HAS_CPP_14
    #undef JSON_HAS_CPP_17
    #undef JSON_HAS_CPP_20
    #undef JSON_HAS_CPP_23
    #undef JSON_HAS_FILESYSTEM
    #undef JSON_HAS_EXPERIMENTAL_FILESYSTEM
    #undef JSON_HAS_THREE_WAY_COMPARISON
    #undef JSON_HAS_RANGES
    #undef JSON_HAS_STATIC_RTTI
    #undef JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
#endif

#include <Convention/instance/nlohmann/thirdparty/hedley/hedley_undef.hpp>
