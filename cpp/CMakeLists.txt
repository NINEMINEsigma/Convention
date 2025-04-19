# Make Setting
option(__FILE_CONVENTION_FFMPEG_INSTANCE "disable ffmpeg_instance" OFF)
option(__IS_REF_BOOST "is import boost" OFF)

message("Root: --- ----- ----- ----- ----- --")

add_compile_definitions(__PLATFORM_NAME="${PLATFORM_NAME}")
add_compile_definitions(__PLATFORM_VERSION="${PLATFORM_VERSION}")
add_compile_definitions(__PLATFORM_EXTENSION="${PLATFORM_EXTENSION}")

function(BoostRefer)
    
    # 检查是否启用Boost
    if(__IS_REF_BOOST)
        message("Convention: Boost support enabled")
        add_compile_definitions(__REF_BOOST)
        
        # 查找Boost库
        find_package(Boost)
        if(Boost_FOUND)
            include_directories(${Boost_INCLUDE_DIRS})
            link_directories(${Boost_LIBRARY_DIRS})
            message("Convention: Boost found at ${Boost_INCLUDE_DIRS}")
        else()
            #message(FATAL_ERROR "Convention: Boost not found, but __IS_REF_BOOST is enabled")
            message("Convention: Boost not found, but __IS_REF_BOOST is enabled")
        endif(Boost_FOUND)
    endif(__IS_REF_BOOST)
    
endfunction(BoostRefer)

# All Configs
cmake_minimum_required (VERSION 3.14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ENABLE_TEST ON)
# Enable C++17
set(CMAKE_CXX_STANDARD 17)
# Enable C11
set(C_STANDARD 11)
# Enable UNICODE
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
# EasySave with Convention-Build
add_subdirectory("EasySave")
# Test Demo
add_subdirectory("Test")
# Visual Algorithm with Convention-Build
add_subdirectory("Visual-Algorithm")

message("Root: CMAKE_CXX_STANDARD = ${CMAKE_CXX_STANDARD}" )
message("Root: CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message("Root: CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")


message("Root: ----- ----- ----- ----- -----")