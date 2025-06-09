#pragma once
#ifndef CONVENTION_KIT_CONFIG_H
#define CONVENTION_KIT_CONFIG_H
#pragma warning(disable : 4267)
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)
#pragma warning(disable : 26495)
#ifndef abstract
#define abstract =0
#endif

#define DISABLE_SYMBOL
#define if_exists __if_exists
#define if_not_exists __if_not_exists

struct DecltypeAnyUnit
{
	template<typename valueType>
	constexpr operator valueType() const noexcept;
};

#pragma region bits/stdc++

// C++ includes used for precompiling -*- C++ -*-

// Copyright (C) 2003-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file stdc++.h
 *  This is an implementation file for a precompiled header.
 */

 // 17.4.1.2 Headers

 // C
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <cwchar>
#include <cwctype>

#include <stdlib.h>

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include <filesystem>

#define NOMINMAX
constexpr size_t ConstexprStrlen(const char* source)
{
	size_t length = 0;
	while (source[length] == '\0')
		length++;
	return length;
}

template<typename T>
class ICompare
{
public:
	virtual ~ICompare() {}
	virtual int Compare(T left, T right) const noexcept abstract;
};

template<>
class ICompare<void>
{
public:
	template<typename T>
	int Compare(const T& left, const T& right) const noexcept
	{
		if (left < right)
			return -1;
		else if (right < left)
			return 1;
		return 0;
	}
};

constexpr bool ConstexprStrEqual(
	const char* source,
	const char* target
)
{
	size_t length = ConstexprStrlen(source);
	size_t tlength = ConstexprStrlen(target);
	if (length == tlength)
	{
		for (auto i = 0; i != length; i++)
			if (source[i] != target[i])
				return false;
		return true;
	}
	return false;
}
constexpr int ConstexprStrCompare(
	const char* source,
	const char* target
)
{
	int length = ConstexprStrlen(source);
	int tlength = ConstexprStrlen(target);
	if (length == tlength)
	{
		for (auto i = 0; i != length; i++)
			if (source[i] != target[i])
				return source[i] - target[i];
		return 0;
	}
	else return length - tlength;
}

#ifndef CURRENT_COM_NAME
// set current-com-name to control platform paths
#define CURRENT_COM_NAME "com.default"
#endif // !CURRENT_COM_NAME

#ifndef CURRENT_APP_NAME
// set current-app-name to control platform paths
#define CURRENT_APP_NAME "unname"
#endif // CURRENT_PROJECT_NAME

#ifndef __PLATFORM_NAME
#define __PLATFORM_NAME "Unknown"
#endif // __PLATFORM_NAME

#ifndef __PLATFORM_VERSION
#define __PLATFORM_VERSION "Unknown"
#endif // __PLATFORM_VERSION

#ifndef PLATFORM_EXTENSION
#define PLATFORM_EXTENSION ""
#endif // PLATFORM_EXTENSION

struct PlatformIndicator
{
	using tag = void;
#ifdef _DEBUG
	constexpr static bool IsRelease = false;
#else
	constexpr static bool IsRelease = true;
#endif
	constexpr static bool value = IsRelease;
#if defined(_WIN64)||defined(_WIN32)
	constexpr static bool IsPlatformWindows = true;
#else
	constexpr static bool IsPlatformWindows = false;
#endif
#if defined(__linux__)
	constexpr static bool IsPlatformLinux = true;
#else
	constexpr static bool IsPlatformLinux = false;
#endif
#if defined(__unix__)
	constexpr static bool IsPlatformUnix = true;
#else
	constexpr static bool IsPlatformUnix = false;
#endif
#if defined(__APPLE__)||defined(__MACH__)
	constexpr static bool IsPlatformApple = true;
#else
	constexpr static bool IsPlatformApple = false;
#endif
#if defined(__ANDROID__)
	constexpr static bool IsPlatformAndroid = true;
#else
	constexpr static bool IsPlatformAndroid = false;
#endif
#if defined(_POSIX_VERSION)
	constexpr static bool IsPlatformPosix = true;
#else
	constexpr static bool IsPlatformPosix = false;
#endif
#if defined(_WIN64)||(__WORDSIZE==64)
	constexpr static bool IsPlatformx64 = true;
#endif
#ifdef _MSC_VER
	constexpr static bool IsMSVC = true;
#else
	constexpr static bool IsMSVC = false;
#endif
#ifdef __GNUC__
	constexpr static bool IsGNUC = true;
#else
	constexpr static bool IsGNUC = false;
#endif // __GNUC__


	constexpr static const char* PlatformInfomation = __PLATFORM_NAME "-" __PLATFORM_VERSION  "-"  __PLATFORM_EXTENSION;
	// not lock current thread, if input is exist will return it otherwise return -1
	static int KeyboardInput() noexcept;
	//
	static std::filesystem::path InjectPersistentPath();
	static const std::filesystem::path& ApplicationPath()
	{
		static auto path = std::filesystem::current_path ();
		return path;
	}
	static const std::filesystem::path& StreamingAssetsPath()
	{
		static auto path = ApplicationPath() / "StreamingAssets/";
		return path;
	}
	static const std::filesystem::path& PersistentPath()
	{
		static auto path = InjectPersistentPath();
		return path;
	}
};

#pragma endregion

using type_info = std::type_info;
using std::addressof;

#pragma region Key Check

// xkeycheck.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _XKEYCHECK_H
#define _XKEYCHECK_H

// xkeycheck.h assumes that it's being included by yvals_core.h in a specific order.
// Nothing else should include xkeycheck.h.
//*by here, is one try for check

#if _STL_COMPILER_PREPROCESSOR

#if !defined(_ALLOW_KEYWORD_MACROS) && !defined(__INTELLISENSE__)

// clang-format off
// #if defined($KEYWORD)
// #define $KEYWORD EMIT WARNING C4005
// #error The C++ Standard Library forbids macroizing the keyword "$KEYWORD". \
// Enable warning C4005 to find the forbidden define.
// #endif // $KEYWORD
// clang-format on

// *don't* check the "alternative token representations"

// keywords:
#if defined(alignas)
#define alignas EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "alignas". \
Enable warning C4005 to find the forbidden define.
#endif // alignas

#if defined(alignof)
#define alignof EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "alignof". \
Enable warning C4005 to find the forbidden define.
#endif // alignof

#if defined(asm)
#define asm EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "asm". \
Enable warning C4005 to find the forbidden define.
#endif // asm

#if defined(auto)
#define auto EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "auto". \
Enable warning C4005 to find the forbidden define.
#endif // auto

#if defined(bool)
#define bool EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "bool". \
Enable warning C4005 to find the forbidden define.
#endif // bool

#if defined(break)
#define break EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "break". \
Enable warning C4005 to find the forbidden define.
#endif // break

#if defined(case)
#define case EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "case". \
Enable warning C4005 to find the forbidden define.
#endif // case

#if defined(catch)
#define catch EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "catch". \
Enable warning C4005 to find the forbidden define.
#endif // catch

#if defined(char)
#define char EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "char". \
Enable warning C4005 to find the forbidden define.
#endif // char

#if defined(char8_t) && _HAS_CXX20
#define char8_t EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "char8_t". \
Enable warning C4005 to find the forbidden define.
#endif // char8_t

#if defined(char16_t)
#define char16_t EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "char16_t". \
Enable warning C4005 to find the forbidden define.
#endif // char16_t

#if defined(char32_t)
#define char32_t EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "char32_t". \
Enable warning C4005 to find the forbidden define.
#endif // char32_t

#if defined(class)
#define class EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "class". \
Enable warning C4005 to find the forbidden define.
#endif // class

#if defined(concept) && _HAS_CXX20
#define concept EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "concept". \
Enable warning C4005 to find the forbidden define.
#endif // concept

#if defined(const)
#define const EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "const". \
Enable warning C4005 to find the forbidden define.
#endif // const

#if defined(consteval) && _HAS_CXX20
#define consteval EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "consteval". \
Enable warning C4005 to find the forbidden define.
#endif // consteval

#if defined(constexpr)
#define constexpr EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "constexpr". \
Enable warning C4005 to find the forbidden define.
#endif // constexpr

#if defined(constinit) && _HAS_CXX20
#define constinit EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "constinit". \
Enable warning C4005 to find the forbidden define.
#endif // constinit

#if defined(const_cast)
#define const_cast EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "const_cast". \
Enable warning C4005 to find the forbidden define.
#endif // const_cast

#if defined(continue)
#define continue EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "continue". \
Enable warning C4005 to find the forbidden define.
#endif // continue

#if defined(co_await) && _HAS_CXX20
#define co_await EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "co_await". \
Enable warning C4005 to find the forbidden define.
#endif // co_await

#if defined(co_return) && _HAS_CXX20
#define co_return EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "co_return". \
Enable warning C4005 to find the forbidden define.
#endif // co_return

#if defined(co_yield) && _HAS_CXX20
#define co_yield EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "co_yield". \
Enable warning C4005 to find the forbidden define.
#endif // co_yield

#if defined(decltype)
#define decltype EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "decltype". \
Enable warning C4005 to find the forbidden define.
#endif // decltype

#if defined(default)
#define default EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "default". \
Enable warning C4005 to find the forbidden define.
#endif // default

#if defined(delete)
#define delete EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "delete". \
Enable warning C4005 to find the forbidden define.
#endif // delete

#if defined(do)
#define do EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "do". \
Enable warning C4005 to find the forbidden define.
#endif // do

#if defined(double)
#define double EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "double". \
Enable warning C4005 to find the forbidden define.
#endif // double

#if defined(dynamic_cast)
#define dynamic_cast EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "dynamic_cast". \
Enable warning C4005 to find the forbidden define.
#endif // dynamic_cast

#if defined(else)
#define else EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "else". \
Enable warning C4005 to find the forbidden define.
#endif // else

#if defined(enum)
#define enum EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "enum". \
Enable warning C4005 to find the forbidden define.
#endif // enum

#if defined(explicit)
#define explicit EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "explicit". \
Enable warning C4005 to find the forbidden define.
#endif // explicit

#if defined(export)
#define export EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "export". \
Enable warning C4005 to find the forbidden define.
#endif // export

#if defined(extern)
#define extern EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "extern". \
Enable warning C4005 to find the forbidden define.
#endif // extern

#if defined(false)
#define false EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "false". \
Enable warning C4005 to find the forbidden define.
#endif // false

#if defined(float)
#define float EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "float". \
Enable warning C4005 to find the forbidden define.
#endif // float

#if defined(for)
#define for EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "for". \
Enable warning C4005 to find the forbidden define.
#endif // for

#if defined(friend)
#define friend EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "friend". \
Enable warning C4005 to find the forbidden define.
#endif // friend

#if defined(goto)
#define goto EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "goto". \
Enable warning C4005 to find the forbidden define.
#endif // goto

#if defined(if)
#define if EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "if". \
Enable warning C4005 to find the forbidden define.
#endif // if

#if defined(inline)
#define inline EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "inline". \
Enable warning C4005 to find the forbidden define.
#endif // inline

#if defined(int)
#define int EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "int". \
Enable warning C4005 to find the forbidden define.
#endif // int

#if defined(long)
#define long EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "long". \
Enable warning C4005 to find the forbidden define.
#endif // long

#if defined(mutable)
#define mutable EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "mutable". \
Enable warning C4005 to find the forbidden define.
#endif // mutable

#if defined(namespace)
#define namespace EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "namespace". \
Enable warning C4005 to find the forbidden define.
#endif // namespace

#if defined(new) && defined(_ENFORCE_BAN_OF_MACRO_NEW)
#define new EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "new", though macroized new is supported on this \
implementation as a nonstandard extension. Enable warning C4005 to find the forbidden define, or re-enable the \
extension by removing _ENFORCE_BAN_OF_MACRO_NEW.
#endif // new

#if defined(noexcept)
#define noexcept EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "noexcept". \
Enable warning C4005 to find the forbidden define.
#endif // noexcept

#if defined(nullptr)
#define nullptr EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "nullptr". \
Enable warning C4005 to find the forbidden define.
#endif // nullptr

#if defined(operator)
#define operator EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "operator". \
Enable warning C4005 to find the forbidden define.
#endif // operator

#if defined(private)
#define private EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "private". \
Enable warning C4005 to find the forbidden define.
#endif // private

#if defined(protected)
#define protected EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "protected". \
Enable warning C4005 to find the forbidden define.
#endif // protected

#if defined(public)
#define public EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "public". \
Enable warning C4005 to find the forbidden define.
#endif // public

#if defined(register)
#define register EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "register". \
Enable warning C4005 to find the forbidden define.
#endif // register

#if defined(reinterpret_cast)
#define reinterpret_cast EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "reinterpret_cast". \
Enable warning C4005 to find the forbidden define.
#endif // reinterpret_cast

#if defined(requires) && _HAS_CXX20
#define requires EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "requires". \
Enable warning C4005 to find the forbidden define.
#endif // requires

#if defined(return)
#define return EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "return". \
Enable warning C4005 to find the forbidden define.
#endif // return

#if defined(short)
#define short EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "short". \
Enable warning C4005 to find the forbidden define.
#endif // short

#if defined(signed)
#define signed EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "signed". \
Enable warning C4005 to find the forbidden define.
#endif // signed

#if defined(sizeof)
#define sizeof EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "sizeof". \
Enable warning C4005 to find the forbidden define.
#endif // sizeof

#if defined(static)
#define static EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "static". \
Enable warning C4005 to find the forbidden define.
#endif // static

#if defined(static_assert)
#define static_assert EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "static_assert". \
Enable warning C4005 to find the forbidden define.
#endif // static_assert

#if defined(static_cast)
#define static_cast EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "static_cast". \
Enable warning C4005 to find the forbidden define.
#endif // static_cast

#if defined(struct)
#define struct EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "struct". \
Enable warning C4005 to find the forbidden define.
#endif // struct

#if defined(switch)
#define switch EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "switch". \
Enable warning C4005 to find the forbidden define.
#endif // switch

#if defined(template)
#define template EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "template". \
Enable warning C4005 to find the forbidden define.
#endif // template

#if defined(this)
#define this EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "this". \
Enable warning C4005 to find the forbidden define.
#endif // this

#if defined(thread_local)
#define thread_local EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "thread_local". \
Enable warning C4005 to find the forbidden define.
#endif // thread_local

#if defined(throw)
#define throw EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "throw". \
Enable warning C4005 to find the forbidden define.
#endif // throw

#if defined(true)
#define true EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "true". \
Enable warning C4005 to find the forbidden define.
#endif // true

#if defined(try)
#define try EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "try". \
Enable warning C4005 to find the forbidden define.
#endif // try

#if defined(typedef)
#define typedef EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "typedef". \
Enable warning C4005 to find the forbidden define.
#endif // typedef

#if defined(typeid)
#define typeid EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "typeid". \
Enable warning C4005 to find the forbidden define.
#endif // typeid

#if defined(typename)
#define typename EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "typename". \
Enable warning C4005 to find the forbidden define.
#endif // typename

#if defined(union)
#define union EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "union". \
Enable warning C4005 to find the forbidden define.
#endif // union

#if defined(unsigned)
#define unsigned EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "unsigned". \
Enable warning C4005 to find the forbidden define.
#endif // unsigned

#if defined(using)
#define using EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "using". \
Enable warning C4005 to find the forbidden define.
#endif // using

#if defined(virtual)
#define virtual EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "virtual". \
Enable warning C4005 to find the forbidden define.
#endif // virtual

#if defined(void)
#define void EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "void". \
Enable warning C4005 to find the forbidden define.
#endif // void

#if defined(volatile)
#define volatile EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "volatile". \
Enable warning C4005 to find the forbidden define.
#endif // volatile

#if defined(wchar_t)
#define wchar_t EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "wchar_t". \
Enable warning C4005 to find the forbidden define.
#endif // wchar_t

#if defined(while)
#define while EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "while". \
Enable warning C4005 to find the forbidden define.
#endif // while

// contextual keywords (a.k.a. "identifiers with special meaning"):
#if defined(final)
#define final EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the contextual keyword "final". \
Enable warning C4005 to find the forbidden define.
#endif // final

#if defined(import) && _HAS_CXX20
#define import EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the contextual keyword "import". \
Enable warning C4005 to find the forbidden define.
#endif // import

#if defined(module) && _HAS_CXX20
#define module EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the contextual keyword "module". \
Enable warning C4005 to find the forbidden define.
#endif // module

#if defined(override)
#define override EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the contextual keyword "override". \
Enable warning C4005 to find the forbidden define.
#endif // override

// attribute-tokens:
#if defined(carries_dependency)
#define carries_dependency EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the attribute-token "carries_dependency". \
Enable warning C4005 to find the forbidden define.
#endif // carries_dependency

#if defined(deprecated)
#define deprecated EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the attribute-token "deprecated". \
Enable warning C4005 to find the forbidden define.
#endif // deprecated

#if defined(fallthrough) && _HAS_CXX17
#define fallthrough EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the attribute-token "fallthrough". \
Enable warning C4005 to find the forbidden define.
#endif // fallthrough

// not checking "likely" because it is commonly defined as a function-like macro

#if defined(maybe_unused) && _HAS_CXX17
#define maybe_unused EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the attribute-token "maybe_unused". \
Enable warning C4005 to find the forbidden define.
#endif // maybe_unused

#if defined(nodiscard) // C++17 attribute-token, also enforced in C++14 mode
#define nodiscard EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the attribute-token "nodiscard". \
Enable warning C4005 to find the forbidden define.
#endif // nodiscard

#if defined(noreturn)
#define noreturn EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the attribute-token "noreturn". \
Enable warning C4005 to find the forbidden define.
#endif // noreturn

#if defined(no_unique_address) && _HAS_CXX20
#define no_unique_address EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the attribute-token "no_unique_address". \
Enable warning C4005 to find the forbidden define.
#endif // no_unique_address

// not checking "unlikely" because it is commonly defined as a function-like macro

#endif // !defined(_ALLOW_KEYWORD_MACROS) && !defined(__INTELLISENSE__)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XKEYCHECK_H

#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif // !_NODISCARD

#pragma endregion

#pragma region MSVC Features

#ifndef __Non_Portable_Features

#define __Non_Portable_Features

//Depends on the Microsoft C++ implementation

#ifdef _MSC_VER

#include <sal.h>

#define _allocator_ret_ __declspec(allocator)

#else

#define _allocator_ret_ __declspec(allocator)

#endif // _MSC_VER

#pragma region __TEST_MICROSOFT_IMPLEMENTATION

#if defined(__USE__) || (defined(_DEBUG)&&0) || defined(_USE_Non_Portable_Features)

//Variadic
//__declspec(align(#))

#endif // __TEST_MICROSOFT_IMPLEMENTATION

#pragma endregion

//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------

#pragma region __based

#if defined(__USE_BASE_PTR) || (defined(_DEBUG)&&0) || defined(_USE_Non_Portable_Features)

//class
//__based
template<typename _T, _T* _Ptr>
class BasePtr
{
public:
	typedef _T __based(_Ptr)*Offset;
	_T& operator[](_In_ const size_t index)
	{
		Offset offset = 0;
		return *(offset + index);
	}
};

//Variadic
//__based
#define DefineBasePtr(_T,_Ptr_Name,_Ptr_Org,_P)				\
	_T*	_Ptr_Name = &_Ptr_Org[0];							\
	typedef _T __based(_Ptr_Name)* _P;						\

//Variadic
//__based
#define TransfromBasePtr(_Ptr_Name,_Ptr_Org)				\
	_Ptr_Name = &_Ptr_Org[0]								\

#endif // __USE_BASE_PTR

#pragma endregion

#pragma region __inheritance

#if defined(__USE_INHERITANCE) || (defined(_DEBUG)&&0) || defined(_USE_Non_Portable_Features)

//Variadic
//__single_inheritance
#define single_inheritance __single_inheritance

#endif // __USE_INHERITANCE

#pragma endregion

#pragma region alignof

#if defined(__USE_ALIGNOF) || (defined(_DEBUG)&&0) || defined(_USE_Non_Portable_Features)

template<class _T>
class AlignInfo final
{
public:
	AlignInfo() :type(typeid(_T)), align_size(alignof(_T)), memory_size(sizeof(_T))
	{
	}

	const type_info& type;
	const size_t align_size;
	const size_t memory_size;

	template<typename _P>
	bool operator==(const AlignInfo<_P>& _Right)
	{
		return this->align_size == _Right.align_size;
	}
	template<typename _P>
	bool operator!=(const AlignInfo<_P>& _Right)
	{
		return this->align_size != _Right.align_size;
	}
	template<>
	bool operator==(const AlignInfo<_T>& _Right)
	{
		return true;
	}
	template<>
	bool operator!=(const AlignInfo<_T>& _Right)
	{
		return false;
	}
};

//Variadic
//__declspec(align(#))
#define DeclspecAlign(size_move) __declspec(align(1<<size_move))

//Variadic
//__declspec(align(#))
#define Realign(_T,size_move) typedef __declspec(align(1 << size_move)) class _T

#endif // __USE_ALIGNOF

#pragma endregion

#pragma region __assert

#if defined(__USE_ASSERT) || (defined(_DEBUG)&&1) || defined(_USE_Non_Portable_Features)

//Variadic
//__assert
#define assume_where(e) (((e) || (assert(e), (e))), __assume(e))

//Variadic
//__assert
#define assume0 __assume(0)

#endif // __USE_ASSERT

#pragma endregion

#pragma region restrict

#if defined(__USE_RESTRICT) || (defined(_DEBUG)&&0) || defined(_USE_Non_Portable_Features)

//Variadic
//__declspec(restrict)
#define Restrict __declspec(restrict)

#endif // __USE_RESTRICT

#pragma endregion

#pragma region __super

#if defined(__USE_SUPER) || (defined(_DEBUG)&&0) || defined(_USE_Non_Portable_Features)

//Variadic
//__super
#define super __super::

#endif // __super

#pragma endregion

#pragma region __vector

#if defined(__USE_VECTORCALL) || (defined(_DEBUG)&&0) || defined(_USE_Non_Portable_Features)

//Variadic
//__vectorcall
#define vectorcall __vectorcall

//Variadic
//__vectorcall
#define vector_call __vectorcall

#endif // __TEST_MICROSOFT_IMPLEMENTATION

#pragma endregion

//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------

#endif // !__Non_Portable_Features

#ifdef _MSC_VER
#define hook_allocator_ptr_return __declspec(allocator)
#else
#define hook_allocator_ptr_return
#endif // _MSC_VER

#ifndef _CONSTEXPR20
#define _CONSTEXPR20
#endif // !_CONSTEXPR20

#pragma endregion

#pragma region Kit

#define _STR_(str)		#str
#define _STR_LINE_		_STR_(__LINE__)
#define _STR_FILE_		_STR_(__FILE__)

#ifdef NULL
#undef NULL
#define NULL nullptr
#endif // NULL

#pragma endregion

#pragma region SAL

/*---------------------------------------------------------------------------*/
/* SAL ANNOTATIONS                                                           */
/*---------------------------------------------------------------------------*/
/*
 * Define SAL annotations if they aren't defined yet.
 */
#ifndef _Success_
#define _Success_( x )
#endif
#ifndef _Notref_
#define _Notref_
#endif
#ifndef _When_
#define _When_( x, y )
#endif
#ifndef _Pre_valid_
#define _Pre_valid_
#endif
#ifndef _Pre_opt_valid_
#define _Pre_opt_valid_
#endif
#ifndef _Post_invalid_
#define _Post_invalid_
#endif
#ifndef _In_
#define _In_
#endif
#ifndef _In_z_
#define _In_z_
#endif
#ifndef _In_opt_
#define _In_opt_
#endif
#ifndef _In_range_
#define _In_range_( x, y )
#endif
#ifndef _In_reads_
#define _In_reads_( x )
#endif
#ifndef _In_reads_z_
#define _In_reads_z_( x )
#endif
#ifndef _In_reads_opt_
#define _In_reads_opt_( x )
#endif
#ifndef _In_reads_bytes_
#define _In_reads_bytes_( x )
#endif
#ifndef _In_reads_bytes_opt_
#define _In_reads_bytes_opt_( x )
#endif
#ifndef _Out_
#define _Out_
#endif
#ifndef _Out_opt_
#define _Out_opt_
#endif
#ifndef _Out_writes_
#define _Out_writes_( x )
#endif
#ifndef _Out_writes_z_
#define _Out_writes_z_( x )
#endif
#ifndef _Out_writes_opt_
#define _Out_writes_opt_( x )
#endif
#ifndef _Out_writes_to_opt_
#define _Out_writes_to_opt_( x, y )
#endif
#ifndef _Out_writes_bytes_opt_
#define _Out_writes_bytes_opt_( x )
#endif
#ifndef _Inout_
#define _Inout_
#endif
#ifndef _Inout_opt_
#define _Inout_opt_
#endif
#ifndef _Inout_updates_opt_
#define _Inout_updates_opt_( x )
#endif
#ifndef _Deref_in_range_
#define _Deref_in_range_( x, y )
#endif
#ifndef _Deref_out_range_
#define _Deref_out_range_( x, y )
#endif
#ifndef _Pre_satisfies_
#define _Pre_satisfies_( x )
#endif
#ifndef _Post_satisfies_
#define _Post_satisfies_( x )
#endif
#ifndef _Post_equal_to_
#define _Post_equal_to_( x )
#endif
#ifndef _Notnull_
#define _Notnull_
#endif // !_Notnull_
#ifndef _Ret_maybenull_
#define _Ret_maybenull_
#endif // !_Ret_maybenull_


#pragma endregion

#pragma region string wstring

//#include <locale>
//#include <codecvt>

namespace std
{
	inline std::string to_string(const std::string& str)
	{
		return str;
	}
	inline std::wstring to_wstring(const std::wstring& str)
	{
		return str;
	}
	inline std::string to_string(const std::filesystem::path& path)
	{
		return path.string();
	}
	inline std::wstring to_wstring(const std::filesystem::path& path)
	{
		return path.wstring();
	}
	inline std::string to_string(const std::wstring& str)
	{
		return to_string(std::filesystem::path(str));
	}
	inline std::wstring to_wstring(const std::string& str)
	{
		return to_wstring(std::filesystem::path(str));
	}
	inline std::string to_string(const char* str)
	{
		return std::string(str);
	}
	inline std::wstring to_wstring(const wchar_t* str)
	{
		return std::wstring(str);
	}
	template<size_t size>
	inline std::string to_string(const char str[size])
	{
		return std::string(str);
	}
	template<size_t size>
	inline std::wstring to_wstring(const wchar_t str[size])
	{
		return std::wstring(str);
	}
}

struct CharIndicator
{
#if defined(UNICODE)
	using tag = wchar_t;
	static constexpr bool value = true;
#else
	using tag = char;
	static constexpr bool value = false;
#endif // _UNICODE
};
struct StringIndicator
{
	using tag = std::basic_string<CharIndicator::tag>;
	static constexpr bool value = CharIndicator::value;

	using traits = std::char_traits<CharIndicator::tag>;

	static size_t strlen(const CharIndicator::tag* str)
	{
		return traits::length(str);
	}
	static CharIndicator::tag* strcpy(
		CharIndicator::tag* dest,
		const CharIndicator::tag* source
	)
	{
		return traits::copy(dest, source, strlen(source));
	}
	static CharIndicator::tag* strcpy_s(
		CharIndicator::tag* dest,
		const CharIndicator::tag* source,
		const size_t size
	)
	{
		return traits::copy(dest, source, std::min(size, strlen(source)));
	}

	static size_t c_strlen(const char* str)
	{
		return ::strlen(str);
	}
	static char* c_strcpy(
		char* dest,
		const char* source
	)
	{
		return ::strcpy(dest, source);
	}
	static int c_strcpy_s(
		char* dest,
		const char* source,
		const size_t size
	)
	{
		return ::strcpy_s(dest, std::min(size, c_strlen(source)), source);
	}

	template<typename str, typename T>
	static str ToString(const T& value)
	{
		if_exists(T::ToString)
			return value.ToString();
		if constexpr (std::is_constructible_v<const T&, str>)
			return value;
		else if constexpr (std::is_same_v<str, std::wstring>)
			return std::to_wstring(value);
		else
			return std::to_string(value);
	}
	template<typename T,typename str>
	static T ToValue(const str& value)
	{
		if constexpr (std::is_floating_point_v<T>)
			return std::stold(value);
		else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T> == false)
			return std::stoll(value);
		else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T> == true)
			return std::stoull(value);
		else if constexpr (std::is_same_v<T, bool>)
		{
			auto temp = std::to_string(value);
			if (temp == "false")
				return false;
			else if (temp == "true")
				return true;
			else
				throw std::runtime_error("Cannot convert string to bool. Expected 'true' or 'false'.");
		}
		else if constexpr (std::is_same_v<T, char> || std::is_same_v<T, wchar_t>)
		{
			if (value.count() != 1)
				throw std::runtime_error("Cannot convert string to char. Expected a single character.");
			static_assert(sizeof(T) < sizeof(decltype(value.front())), "Cannot convert string to a smaller character");
			return value.front();
		}
		else if constexpr (std::is_convertible_v<str, T>)
			return static_cast<T>(value);
		else
			static_assert(std::is_convertible_v<str, T>, "Cannot convert string to the specified type.");
	}

	template<typename str, typename _T>
	static str Combine(const _T& first)
	{
		return ToString<str>(first);
	}
	template<typename str, typename _First, typename _LeftT>
	static str Combine(const _First& first, const _LeftT& arg)
	{
		return ToString<str>(first) + ToString<str>(arg);
	}
	template<typename str, typename _First, typename... Args>
	static str Combine(const _First& first, const Args&...args)
	{
		return ToString<str>(first) + Combine<str>(args...);
	}

	// trim whitespace from the beginning and end of a string
	template<typename str>
	str Trim(const str& input, const str& chs, bool isLeft = true, bool isRight = true)
	{
		size_t start = 0;
		size_t end = input.size();

		if (isLeft)
		{
			bool stats = true;
			while (start < end && stats)
			{
				stats = false;
				for (auto&& ch : chs)
				{
					if (ch == input[start])
					{
						start++;
						stats = true;
						break;
					}
				}
			}
		}
		if (isRight)
		{
			bool stats = true;
			while (start < end && stats)
			{
				stats = false;
				for (auto&& ch : chs)
				{
					if (ch == input[end-1])
					{
						end--;
						stats = true;
						break;
					}
				}
			}
		}
		return input.substr(start, end - start);
	}

	/**
	* brief Formats a string using the provided format and arguments. like use snprintf
	*/
	template<typename str, typename... Args>
	static str Format(size_t size, const std::string& format, const Args&... args)
	{
		str result;
		result.reserve(size);
		snprintf(result.data(), size, format.c_str(), args...);
		return result;
	}
};

#ifdef UNICODE
#define COUT std::wcout
#define __CNTEXT(str) L##str
#define CNTEXT(str) __CNTEXT(str)
#else
#define COUT std::cout
#define __CNTEXT(str) u8##str
#define CNTEXT(str) __CNTEXT(str)
#endif
#define make_string(str) StringIndicator::tag(CNTEXT(str))

#pragma endregion

#pragma region Kit

#ifndef __init
#define __init(member) member(member)
#endif // !__init

#pragma endregion

#pragma region Kit

#if _HAS_CXX20
#define is_constant_env() std::is_constant_evaluated()
#else
constexpr bool is_constant_env()
{
	return false;
}
#endif

constexpr bool is_clang_env()
{
#ifdef __clang__
	return true;
#else
	return false;
#endif // __clang__
}

#pragma endregion

#pragma region Kit

namespace Convention
{
	template<typename T, typename... Args>
	void Construct(_In_ T* ptr, Args&&... args)
	{
		new(ptr) T(std::forward<Args>(args)...);
	}
	template<typename T>
	void Destruct(_In_ T* ptr)
	{
		ptr->~T();
	}
}

#pragma endregion

template<typename _T>
_Notnull_ _T* no_warning_6387(_In_opt_ _T* from)
{
	if (from == nullptr)
	{
		throw std::bad_alloc();
	}
	return from;
}

namespace Convention
{
	// first module name will in pair: "execute":path
	// other key will remove front '-' charactor
	// if a string that is not prefixed with the character '-' does not follow a key, it becomes a key
	class CommandLineReader
	{
	public:
		std::map<std::string, std::string> KeyValuePair;
		std::vector<std::pair<std::string, std::string>> KeyVector;
		CommandLineReader(int argc, char** argv)
		{
			std::map<std::string, std::string>& first = KeyValuePair;
			std::vector<std::pair<std::string, std::string>>& second = KeyVector;
			std::string key;
			std::string value;
			bool isKey = true;
			if (argc > 0)
			{
				first["execute"] = argv[0];
				second.push_back({ argv[0],"" });
			}
			for (int i = 1; i < argc; i++)
			{
				if (second.size() != 0 &&
					second.back().first.front() == '-' &&
					second.back().second.size() == 0 &&
					argv[i][0] != '-'
					)
					second.back().second = argv[i];
				else
					second.push_back({ argv[i],"" });

				if (argv[i][0] == '-')
				{
					if (isKey)
						key = argv[i];
					else
						first[key] = value;
					isKey = false;
					key = argv[i];
					while (key.front() == '-')
					{
						key.erase(key.begin());
						if (key.size() == 0)
						{
							isKey = true;
							break;
						}
					}
				}
				else if (isKey == false)
				{
					first[key] = argv[i];
					isKey = true;
				}
				else
				{
					first[argv[i]] = "";
					isKey = true;
				}
			}
			if (isKey == false)
			{
				first[key] = "";
				second.push_back({ key,"" });
			}
		}
	};

	template<typename _Type>
	struct DescriptiveIndicator
	{
		using tag = _Type;
		constexpr static bool value = true;
		const char* description;
		tag target;
		DescriptiveIndicator(const char* description, tag target) noexcept :
			__init(description), __init(target) {
		}
	};
	template<>
	struct DescriptiveIndicator<void>
	{
		using tag = void;
		constexpr static bool value = false;
		const char* description;
		DescriptiveIndicator(const char* description) noexcept :
			__init(description) {
		}
	};

#pragma region is_specialization

	// 基础模板
	template<typename T, template<typename...> class Template>
	struct is_specialization : std::false_type {};

	// 特化模板
	template<template<typename...> class Template, typename... Args>
	struct is_specialization<Template<Args...>, Template> : std::true_type
	{
		using tags = std::tuple<Args...>;
	};

#pragma endregion

}

#pragma region __PRETTY_FUNCTION__

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define PrettyFunctionName() __PRETTY_FUNCTION__

#pragma endregion

#pragma region Tree

namespace Convention
{
	template<typename Element, typename... Elements>
	class ElementTuple
	{
	private:
		using _MySelf = ElementTuple;
		using _MyNext = ElementTuple<Elements...>;
	public:
		constexpr static size_t size = sizeof(Element) + _MyNext::size;
		constexpr static size_t _MySize = 1 + _MyNext::_MySize;
	private:
		char elements[size];
	public:
		template<size_t index>
		using ElementType = std::conditional_t<index == 0, Element, typename _MyNext::template ElementType<index - 1>>;
		template<size_t index>
		constexpr static size_t ElementOffset()
		{
			static_assert(index < _MySize, "Index out of bounds for ElementTuple.");
			if constexpr (index == 0)
				return sizeof(Element);
			else
				return sizeof(Element) + _MyNext::ElementOffset<index - 1>();
		}
		template<size_t index>
		decltype(auto) GetValue() const noexcept
		{
			static_assert(index < _MySize, "Index out of bounds for ElementTuple.");
			return *reinterpret_cast<const ElementType<index>*>(&elements[ElementOffset<index>()]);
		}
		template<size_t index>
		decltype(auto) GetValue() noexcept
		{
			static_assert(index < _MySize, "Index out of bounds for ElementTuple.");
			return *reinterpret_cast<ElementType<index>*>(&elements[ElementOffset<index>()]);
		}
		template<size_t index, typename Arg,
			std::enable_if_t<std::is_convertible_v<Arg, std::remove_reference_t<decltype(GetValue<index>())>>, size_t> = 0>
		void SetValue(Arg&& value) noexcept
		{
			GetValue<index>() = std::forward<Arg>(value);
		}
		template<size_t index, typename Arg,
			std::enable_if_t<std::is_convertible_v<const Arg&, std::remove_reference_t<decltype(GetValue<index>())>>, size_t> = 0>
		void SetValue(const Arg& value) noexcept
		{
			GetValue<index>() = value;
		}
	};
	template<typename Element>
	class ElementTuple<Element>
	{
	public:
		constexpr static size_t size = sizeof(Element);
		constexpr static size_t _MySize = 1;
	private:
		Element elements;
	public:
		template<size_t index>
		using ElementType = std::enable_if_t<index == 0, Element>;
		template<size_t index>
		constexpr static size_t ElementOffset()
		{
			static_assert(index == 0, "Index out of bounds for ElementTuple.");
			return sizeof(Element);
		}
		template<size_t index = 0>
		constexpr const Element& GetValue() const noexcept
		{
			static_assert(index == 0, "Index out of bounds for ElementTuple.");
			return elements;
		}
		template<size_t index = 0>
		constexpr Element& GetValue() noexcept
		{
			static_assert(index == 0, "Index out of bounds for ElementTuple.");
			return elements;
		}
		template<size_t index, typename Arg, std::enable_if_t<std::is_convertible_v<Arg, Element>, size_t> = 0>
		void SetValue(Arg&& value) noexcept
		{
			static_assert(index == 0, "Index out of bounds for ElementTuple.");
			elements = std::forward<Arg>(value);
		}
		template<size_t index, typename Arg,
			std::enable_if_t<std::is_convertible_v<const Arg&, Element>>, size_t> = 0 >
			void SetValue(const Arg & value) noexcept
		{
			static_assert(index == 0, "Index out of bounds for ElementTuple.");
			GetValue<index>() = value;
		}
	};
	template<typename... Elements>
	class ElementTuple<void, Elements...> : public ElementTuple<Elements...> {};
	template<>
	class ElementTuple<void>
	{
	public:
		constexpr static size_t size = 0;
		constexpr static size_t _MySize = 0;
	};
}

#pragma endregion

#endif // !CONVENTION_KIT_CONFIG_H
