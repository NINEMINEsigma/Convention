#include <iostream>
#include <cassert>
#include <chrono>
#include <vector>
#include <thread>
#include "Convention.h"
#include "ConventionEngine/Kernel/CoreModule.h"

// 测试框架简单实现
#define TEST_CASE(name) void name(); std::cout << "运行测试: " << #name << std::endl; name(); std::cout << #name << " 测试通过！" << std::endl;
#define ASSERT(condition) if(!(condition)) { std::cerr << "测试失败: " << __FILE__ << ":" << __LINE__ << std::endl; throw std::runtime_error("断言失败"); }
#define StatsLog() std::cout << "当前状态: " << GetEngineAllocatedMemory() << "/" << GetEngineTotalMemory() << "\n";

// 创建用于测试的派生类
class MemTestObject : public CEKernel CEObject 
{
private:
protected:
    virtual CEKernel CEPtr<CEObject> CreateObject() const override
    {
        return CEKernel CEPtr<MemTestObject>();
    }

    virtual void CloneValuesTo(_In_ CEKernel CEPtr<CEObject> target) const override 
    {
		auto testTarget = target.cast<MemTestObject>();
        testTarget->data = this->data;
    }

public:
    int data;

};

// 测试大量内存分配和释放的性能
void testMemoryPerformance() 
{
    const int ALLOC_COUNT = 100000;
    void** ptrs = new void*[ALLOC_COUNT];

    // 计时开始
    auto start = std::chrono::high_resolution_clock::now();

    // 分配大量内存块
    for (int i = 0; i < ALLOC_COUNT; i++) 
    {
        // 8-256字节随机大小
        size_t size = (rand() % 249) + 8;
        ptrs[i] = ConventionEngineMemoryAlloc(size);
        ASSERT(ptrs[i] != nullptr);
    }

    // 释放所有内存块
    for (int i = 0; i < ALLOC_COUNT; i++) 
    {
        ConventionEngineFreeMemory(ptrs[i]);
    }

    // 计时结束
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "内存性能测试：分配和释放 " << ALLOC_COUNT << " 个块耗时 "
              << elapsed.count() << " 毫秒" << std::endl;

    // 清理引擎
    ClearConventionEngine();
}

// 测试多线程中大量内存分配和释放的性能
void testMemoryPerformance_mutli_thread()
{
    const int ALLOC_COUNT = 100000;
    void** ptrs = new void* [ALLOC_COUNT];

    // 计时开始
    auto start = std::chrono::high_resolution_clock::now();

    std::atomic_int index = 0;

    // 创建多个线程进行内存分配和释放
    srand(time(0));
    std::vector<std::thread> threads(rand() % 23 + 2);
    for (auto&& th : threads)
    {
        th = std::thread([&]() {
            // 分配大量内存块
            while (index < ALLOC_COUNT)
            {
                // 8-256字节随机大小
                size_t size = (rand() % 249) + 8;
                ptrs[index] = ConventionEngineMemoryAlloc(size);
                ASSERT(ptrs[index] != nullptr);
                index.fetch_add(1);
            }});
    }
    for (auto&& th : threads)
    {
        th.join();
    }

    // 释放所有内存块
    for (int i = 0; i < ALLOC_COUNT; i++)
    {
        ConventionEngineFreeMemory(ptrs[i]);
    }

    // 计时结束
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "内存多线程测试：线程数" << threads.size() << "分配和释放 " << ALLOC_COUNT << " 个块耗时 "
        << elapsed.count() << " 毫秒" << std::endl;

    // 清理引擎
    ClearConventionEngine();
}

int main() {
    try {
        // 初始化引擎
        InitConventionEngine((size_t)1024 * 1024);  // 分配1MB内存
        std::cout << "开始ConventionEngine内存管理单元测试...\n" << std::endl;

        TEST_CASE(testMemoryPerformance);
        TEST_CASE(testMemoryPerformance_mutli_thread);
        //TEST_CASE(testMemoryStress);
        //TEST_CASE(testMultithreadedAllocation);
        //TEST_CASE(testObjectLifecycle);
        //TEST_CASE(testAdvancedContainers);

        std::cout << "\n所有内存测试通过！" << std::endl;
        QuitConventionEngine();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}