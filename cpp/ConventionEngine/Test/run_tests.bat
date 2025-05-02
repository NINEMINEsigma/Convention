@echo off
echo 正在运行ConventionEngine内核单元测试...
echo =======================================

echo.
echo 运行基础内核测试:
echo -----------------
_KernelTest.exe
if %ERRORLEVEL% NEQ 0 (
    echo 内核测试失败!
    exit /b %ERRORLEVEL%
)

echo.
echo 运行内存管理测试:
echo -----------------
_MemoryTest.exe
if %ERRORLEVEL% NEQ 0 (
    echo 内存管理测试失败!
    exit /b %ERRORLEVEL%
)

echo.
echo 运行分配器测试:
echo -----------------
_AllocatorTest.exe
if %ERRORLEVEL% NEQ 0 (
    echo 分配器测试失败!
    exit /b %ERRORLEVEL%
)

echo.
echo 所有测试成功完成!
echo =======================================