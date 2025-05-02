#include <iostream>
#include <cassert>
#include <chrono>
#include <array>
#include <type_traits>
#include "Convention.h"
#include "ConventionEngine/Kernel/CoreModule.h"

// 测试框架简单实现
#define TEST_CASE(name) void name(); std::cout << "运行测试: " << #name << std::endl; name(); std::cout << #name << " 测试通过！" << std::endl;
#define ASSERT(condition) if(!(condition)) { std::cerr << "测试失败: " << __FILE__ << ":" << __LINE__ << std::endl; throw std::runtime_error("断言失败"); }

// 测试CEAllocator的基本接口
void testAllocatorInterface() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024);

    // 测试基本类型分配器
    {
        CE CEAllocator<int> intAlloc;

        // 分配单个值
        int* pInt = intAlloc.allocate(1);
        ASSERT(pInt != nullptr);
        *pInt = 42;
        ASSERT(*pInt == 42);

        // 分配多个值
        int* pInts = intAlloc.allocate(10);
        ASSERT(pInts != nullptr);
        for (int i = 0; i < 10; i++) {
            pInts[i] = i;
        }
        for (int i = 0; i < 10; i++) {
            ASSERT(pInts[i] == i);
        }

        // 释放内存
        intAlloc.deallocate(pInt, 1);
        intAlloc.deallocate(pInts, 10);
    }

    // 测试复杂类型分配器
    {

    }

    // 清理引擎
    QuitConventionEngine();
}

// 测试分配器状态和属性
void testAllocatorProperties() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024);

    // 测试分配器状态和属性
    {
        CE CEAllocator<int> intAlloc1;
        CE CEAllocator<int> intAlloc2;

        // 不同类型的分配器
        CE CEAllocator<double> doubleAlloc;

        // 测试相等运算符
        ASSERT(intAlloc1 == intAlloc2);
        ASSERT(!(intAlloc1 != intAlloc2));

        // 测试不同类型分配器的相等性
        ASSERT((intAlloc1 == CE CEAllocator<double>()));

        // 测试max_size()
        ASSERT(intAlloc1.max_size() > 0);
        ASSERT(doubleAlloc.max_size() > 0);

        // 类型特性
        bool is_equal = std::is_same<typename CE CEAllocator<int>::value_type, int>::value;
        ASSERT(is_equal);
    }

    // 清理引擎
    QuitConventionEngine();
}

// 测试STL容器与分配器的兼容性
void testSTLCompatibility() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024);

    // 测试向量
    {
        CE Types::Vector<int> vec;
        vec.reserve(100);
        for (int i = 0; i < 100; i++) {
            vec.push_back(i);
        }

        ASSERT(vec.size() == 100);
        for (int i = 0; i < 100; i++) {
            ASSERT(vec[i] == i);
        }

        // 测试移除
        vec.erase(vec.begin() + 50);
        ASSERT(vec.size() == 99);
        ASSERT(vec[50] == 51);

        // 测试插入
        vec.insert(vec.begin() + 25, 1000);
        ASSERT(vec.size() == 100);
        ASSERT(vec[25] == 1000);
    }

    // 测试字符串
    {
        CE Types::String str;
        for (int i = 0; i < 1000; i++) {
            str += "a";
        }

        ASSERT(str.length() == 1000);

        // 测试字符串操作
        str.replace(0, 100, "test");
        ASSERT(str.length() == 904);
        ASSERT(str.substr(0, 4) == "test");
    }

    // 测试map
    {
        CE Types::Map<int, CE Types::String> map;

        // 插入100个元素
        for (int i = 0; i < 100; i++) {
            map[i] = "Value" + std::to_string(i);
        }

        ASSERT(map.size() == 100);

        // 测试查找
        auto it = map.find(50);
        ASSERT(it != map.end());
        ASSERT(it->second == "Value50");

        // 测试删除
        map.erase(50);
        ASSERT(map.size() == 99);
        ASSERT(map.find(50) == map.end());
    }

    // 清理引擎
    QuitConventionEngine();
}

// 测试大量元素和极端情况
void testEdgeCases() {
    // 初始化引擎
    InitConventionEngine(32 * 1024 * 1024);

    // 测试大型向量
    {
        const int LARGE_SIZE = 1000000;
        CE Types::Vector<int> largeVec;

        // 分配时间
        auto start = std::chrono::high_resolution_clock::now();

        largeVec.reserve(LARGE_SIZE);
        for (int i = 0; i < LARGE_SIZE; i++) {
            largeVec.push_back(i);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        std::cout << "分配 " << LARGE_SIZE << " 个元素耗时: "
                  << elapsed.count() << " 毫秒" << std::endl;

        ASSERT(largeVec.size() == LARGE_SIZE);

        // 验证部分值
        for (int i = 0; i < LARGE_SIZE; i += 10000) {
            ASSERT(largeVec[i] == i);
        }

        // 清理前执行GC
        GC_ConventionEngine();
    }

    // 测试多个容器同时运行，检查交互
    {
        CE Types::Vector<CE Types::String> vecStr;
        CE Types::List<int> listInt;
        CE Types::Map<CE Types::String, CE Types::Vector<double>> complexMap;

        // 向容器添加数据
        for (int i = 0; i < 1000; i++) {
            // 向向量添加字符串
            vecStr.push_back("String" + std::to_string(i));

            // 向列表添加整数
            listInt.push_back(i);

            // 向映射添加复杂数据
            CE Types::String key = "Key" + std::to_string(i % 10);
            complexMap[key].push_back(i * 1.5);
        }

        ASSERT(vecStr.size() == 1000);
        ASSERT(listInt.size() == 1000);
        ASSERT(complexMap.size() <= 10); // 最多10个键

        // 验证一些数据
        ASSERT(vecStr[500] == "String500");

        // 验证列表
        int sum = 0;
        for (auto it = listInt.begin(); it != listInt.end(); ++it) {
            sum += *it;
        }
        ASSERT(sum == 499500); // 0+1+2+...+999的和

        // 验证映射
        ASSERT(complexMap["Key5"].size() == 100); // 每个键应有100个值
    }

    // 清理引擎
    QuitConventionEngine();
}

// 测试分配器自定义功能
void testCustomFeatures() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024);

    // 测试分配器复制
    {
        CE CEAllocator<int> sourceAlloc;
        int* ptr = sourceAlloc.allocate(1);
        *ptr = 42;

        // 复制分配器
        CE CEAllocator<int> destAlloc(sourceAlloc);

        // 使用新分配器释放旧内存（应该是安全的，因为它们共享同一个内存池）
        destAlloc.deallocate(ptr, 1);
    }

    // 测试allocate_at_least (C++20)
    {
#ifdef __cpp_lib_allocate_at_least
        CE CEAllocator<double> alloc;
        auto result = alloc.allocate_at_least(100);
        ASSERT(result.ptr != nullptr);
        ASSERT(result.count >= 100);

        // 使用分配的内存
        for (size_t i = 0; i < 100; i++) {
            result.ptr[i] = i * 1.5;
        }

        // 验证值
        for (size_t i = 0; i < 100; i++) {
            ASSERT(result.ptr[i] == i * 1.5);
        }

        // 释放内存
        alloc.deallocate(result.ptr, result.count);
#else
        std::cout << "C++20 allocate_at_least 特性不可用，跳过相关测试" << std::endl;
#endif
    }

    // 清理引擎
    QuitConventionEngine();
}

int main() {
    try {
        std::cout << "开始ConventionEngine CEAllocator单元测试...\n" << std::endl;

        TEST_CASE(testAllocatorInterface);
        TEST_CASE(testAllocatorProperties);
        TEST_CASE(testSTLCompatibility);
        TEST_CASE(testEdgeCases);
        TEST_CASE(testCustomFeatures);

        std::cout << "\n所有CEAllocator测试通过！" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}