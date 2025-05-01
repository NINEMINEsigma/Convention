message("Convention Engine Kernel: --- ----- ----- ----- ----- --")

aux_source_directory(./include cpps)
add_library(ConventionEngineKernel SHARED "${cpps}")
target_link_libraries(ConventionEngineKernel PUBLIC Convention-Shared)

# 定义CEObject内置名称字符数组的长度
target_compile_definitions(ConventionEngineKernel PUBLIC __NameMaxLength=256)
# 定义内存管理器中扩容时扩容的比例
target_compile_definitions(ConventionEngineKernel PUBLIC __RawT_Memory_K=1.5)
# 当被定义时内存管理器中的线程本地缓存将被禁用
#target_compile_definitions(ConventionEngineKernel PUBLIC __RawT_Disable_Thread_Cache)
# 定义线程本地缓存的最大空闲块数量
target_compile_definitions(ConventionEngineKernel PUBLIC __RawT_Max_Thread_Cache_Free_Blocks=64)

set_target_properties(ConventionEngineKernel PROPERTIES PUBLIC_HEADER ./CoreModule.h)

install(TARGETS ConventionEngineKernel
        PUBLIC_HEADER DESTINATION           ${CMAKE_INSTALL_PREFIX}/include/ConventionEngine/Kernel
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)
        
message("Convention Engine Kernel: ----- ----- ----- ----- -----")