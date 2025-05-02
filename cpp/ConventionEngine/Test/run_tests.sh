#!/bin/bash

echo "正在运行ConventionEngine内核单元测试..."
echo "======================================="

echo ""
echo "运行基础内核测试:"
echo "-----------------"
./_KernelTest
if [ $? -ne 0 ]; then
    echo "内核测试失败!"
    exit $?
fi

echo ""
echo "运行内存管理测试:"
echo "-----------------"
./_MemoryTest
if [ $? -ne 0 ]; then
    echo "内存管理测试失败!"
    exit $?
fi

echo ""
echo "运行分配器测试:"
echo "-----------------"
./_AllocatorTest
if [ $? -ne 0 ]; then
    echo "分配器测试失败!"
    exit $?
fi

echo ""
echo "所有测试成功完成!"
echo "======================================="