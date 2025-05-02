message("Convention Engine Kernel: --- ----- ----- ----- ----- --")

# 设置CMake版本要求和项目名称
cmake_minimum_required(VERSION 3.17)
project(ConventionEngineKernel VERSION 0.0.1)

add_compile_options(/bigobj)
message(STATUS "启用/bigobj编译选项，解决对象文件节数限制问题")

# 引入GNUInstallDirs以获取标准安装路径
include(GNUInstallDirs)

file(GLOB Root_Headers *.h)
message("Root_Headers: ${Root_Headers}")
aux_source_directory(./include cpps)

# 创建库
add_library(ConventionEngine-Kernel SHARED "${cpps}")

# 设置别名，使得在本项目中可以使用命名空间版本
add_library(ConventionEngine::Kernel ALIAS ConventionEngine-Kernel)

# 添加链接目录
link_directories("${CMAKE_BINARY_DIR}/Convention")
target_link_libraries(ConventionEngine-Kernel PUBLIC Convention::Shared)

# 定义CEObject内置名称字符数组的长度
target_compile_definitions(ConventionEngine-Kernel PUBLIC __NameMaxLength=256)
# 定义内存管理器中扩容时扩容的比例
target_compile_definitions(ConventionEngine-Kernel PUBLIC __RawT_Memory_K=1.5)
# 当被定义时内存管理器中的线程本地缓存将被禁用
#target_compile_definitions(ConventionEngineKernel PUBLIC __RawT_Disable_Thread_Cache)
# 定义线程本地缓存的最大空闲块数量
target_compile_definitions(ConventionEngine-Kernel PUBLIC __RawT_Max_Thread_Cache_Free_Blocks=64)

set_target_properties(ConventionEngine-Kernel PROPERTIES
  PUBLIC_HEADER "${Root_Headers}"
  OUTPUT_NAME "ConventionEngine-Kernel"
  EXPORT_NAME "Kernel"
  WINDOWS_EXPORT_ALL_SYMBOLS ON
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/ConventionEngine/Kernel"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/ConventionEngine/Kernel"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/ConventionEngine/Kernel"
)

# 设置包含目录
target_include_directories(ConventionEngine-Kernel PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

# 安装头文件
# 安装库文件并生成导出目标
install(TARGETS ConventionEngine-Kernel
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ConventionEngine/Kernel
        RUNTIME DESTINATION       ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION       ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION       ${CMAKE_INSTALL_LIBDIR})

message("Convention Engine Kernel: ----- ----- ----- ----- -----")