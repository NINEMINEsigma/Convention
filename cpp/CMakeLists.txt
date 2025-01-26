# Make Setting
option(disable_FFMPEG "disable ffmpeg_instance" OFF)

function(disable_instance instance_name)
	if(disable_${instance_name})
		add_compile_definitions(__FILE_CONVENTION_${instance_name}_INSTANCE)
	endif()
endfunction()

message("Root: --- ----- ----- ----- ----- --")

disable_instance(FFMPEG)

add_compile_definitions(__PLATFORM_NAME="${PLATFORM_NAME}")
add_compile_definitions(__PLATFORM_VERSION="${PLATFORM_VERSION}")
add_compile_definitions(__PLATFORM_EXTENSION="${PLATFORM_EXTENSION}")


# All Configs
cmake_minimum_required (VERSION 3.14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ENABLE_TEST ON)
# Enable C++17
set(CMAKE_CXX_STANDARD 17)
# Enable C11
set(C_STANDARD 11)
if(DEFINED _WINDOWS OR DEFINED _LINUX_ON_WINDOWS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive- /std:c++17 /Zc:__cplusplus") 
elseif(DEFINED _LINUX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") 
endif()
add_compile_definitions(UNICODE)
message("Root: PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")


# MSVC Policy
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
# Set CMP0076 Policy NEW
cmake_policy(SET CMP0076 NEW) 


# Message Platfrom
message("Root: ${PLATFORM_NAME}-${PLATFORM_VERSION}-${PLATFORM_EXTENSION}")

# Threads Enable
add_compile_definitions(_PTHREADS)

# Project
project("Convention")

# Sub Project(buf from third-party)

# Project
add_subdirectory("Convention")
# Test Demo
add_subdirectory("Test")

message("Root: CMAKE_CXX_STANDARD = ${CMAKE_CXX_STANDARD}" )
message("Root: CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message("Root: CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")


message("Root: ----- ----- ----- ----- -----")