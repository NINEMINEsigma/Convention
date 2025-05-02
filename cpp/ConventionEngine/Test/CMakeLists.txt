add_executable(CETest cli.cpp)

# 测试可执行文件
add_executable(_KernelTest KernelTest.cpp)
add_executable(_MemoryTest MemoryTest.cpp)
add_executable(_AllocatorTest AllocatorTest.cpp)

# 链接目录
link_directories("${CMAKE_BINARY_DIR}/Convention")
link_directories("${CMAKE_BINARY_DIR}/ConventionEngine/Kernel")

# 链接库
target_link_libraries(CETest PUBLIC Convention::Shared)
target_link_libraries(CETest PUBLIC ConventionEngine::Kernel)

# 为内核测试链接库
target_link_libraries(_KernelTest PUBLIC Convention::Shared)
target_link_libraries(_KernelTest PUBLIC ConventionEngine::Kernel)

# 为内存测试链接库
target_link_libraries(_MemoryTest PUBLIC Convention::Shared)
target_link_libraries(_MemoryTest PUBLIC ConventionEngine::Kernel)

# 为分配器测试链接库
target_link_libraries(_AllocatorTest PUBLIC Convention::Shared)
target_link_libraries(_AllocatorTest PUBLIC ConventionEngine::Kernel)

# 安装可执行目标
install(TARGETS CETest _KernelTest _MemoryTest _AllocatorTest
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# 安装测试脚本
if(WIN32)
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.bat
          DESTINATION ${CMAKE_INSTALL_BINDIR}
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                     GROUP_READ GROUP_EXECUTE
                     WORLD_READ WORLD_EXECUTE)
else()
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.sh
          DESTINATION ${CMAKE_INSTALL_BINDIR}
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                     GROUP_READ GROUP_EXECUTE
                     WORLD_READ WORLD_EXECUTE)
endif()