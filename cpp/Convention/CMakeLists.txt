message("Convention: --- ----- ----- ----- ----- --")

BoostRefer()

# 设置CMake版本要求和项目名称
cmake_minimum_required(VERSION 3.17)
project(ConventionLibrary VERSION 0.0.2)

# 引入GNUInstallDirs以获取标准安装路径
include(GNUInstallDirs)

file(GLOB Root_Headers *.h)
message("Root_Headers: ${Root_Headers}")
aux_source_directory(./include  cpps)
file(GLOB Instance_Headers instance/*.h)
message("Instance_Headers: ${Instance_Headers}")

# 创建库
add_library(Convention-Static STATIC ${cpps})
add_library(Convention-Shared SHARED ${cpps})

# 设置别名，使得在本项目中可以使用命名空间版本
add_library(Convention::Static ALIAS Convention-Static)
add_library(Convention::Shared ALIAS Convention-Shared)

# 为库设置属性
set_target_properties(Convention-Static PROPERTIES
  OUTPUT_NAME "Convention-Static"
  EXPORT_NAME "Static"
  POSITION_INDEPENDENT_CODE ON
)

set_target_properties(Convention-Shared PROPERTIES
  OUTPUT_NAME "Convention-Shared"
  EXPORT_NAME "Shared"
  POSITION_INDEPENDENT_CODE ON
  WINDOWS_EXPORT_ALL_SYMBOLS ON 
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Convention"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Convention"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Convention"
)

# 设置包含目录
target_include_directories(Convention-Static PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(Convention-Shared PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

# 安装头文件
install(FILES ${Root_Headers}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Convention)
install(FILES ${Instance_Headers}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Convention/instance)

# 安装库文件并生成导出目标
install(TARGETS Convention-Static
        EXPORT ConventionTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS Convention-Shared
        EXPORT ConventionTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# 安装导出目标文件
install(EXPORT ConventionTargets
        FILE ConventionTargets.cmake
        NAMESPACE Convention::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Convention)

# 生成配置文件
include(CMakePackageConfigHelpers)

# 生成版本文件
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ConventionConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# 配置并安装Config文件
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ConventionConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ConventionConfig.cmake
  @ONLY
)

# 安装配置文件
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/ConventionConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/ConventionConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Convention
)

message("Convention: ----- ----- ----- ----- -----")