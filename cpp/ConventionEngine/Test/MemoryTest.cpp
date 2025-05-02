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

// 创建用于测试的派生类
class MemTestObject : public CE CEObject {
private:
    CE Types::Vector<int> m_data;

protected:
    virtual CE CEObject* CreateObject() const override {
        return new MemTestObject();
    }

    virtual void CloneValuesTo(_In_ CE CEObject* target) const override {
        MemTestObject* testTarget = static_cast<MemTestObject*>(target);
        testTarget->m_data = this->m_data;
    }

public:
    void AddData(int value) {
        m_data.push_back(value);
    }

    int GetDataAt(size_t index) const {
        return m_data[index];
    }

    size_t GetDataSize() const {
        return m_data.size();
    }
};

// 测试大量内存分配和释放的性能
void testMemoryPerformance() {
    // 初始化引擎
    InitConventionEngine(64 * 1024 * 1024);  // 分配64MB内存

    const int ALLOC_COUNT = 100000;
    void* ptrs[ALLOC_COUNT];

    // 计时开始
    auto start = std::chrono::high_resolution_clock::now();

    // 分配大量内存块
    for (int i = 0; i < ALLOC_COUNT; i++) {
        // 8-256字节随机大小
        size_t size = (rand() % 249) + 8;
        ptrs[i] = ConventionEngineMemoryAlloc(size);
        ASSERT(ptrs[i] != nullptr);
    }

    // 释放所有内存块
    for (int i = 0; i < ALLOC_COUNT; i++) {
        ConventionEngineFreeMemory(ptrs[i]);
    }

    // 计时结束
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "内存性能测试：分配和释放 " << ALLOC_COUNT << " 个块耗时 "
              << elapsed.count() << " 毫秒" << std::endl;

    // 清理引擎
    QuitConventionEngine();
}

// 测试内存压力
void testMemoryStress() {
    // 初始化引擎，只分配较小内存，测试扩容机制
    const size_t INITIAL_SIZE = 1 * 1024 * 1024;  // 1MB初始内存
    InitConventionEngine(INITIAL_SIZE);

    std::vector<void*> allocs;
    size_t totalAllocated = 0;

    try {
        // 尝试分配比初始大小更大的内存，测试自动扩容
        for (int i = 0; i < 50; i++) {
            size_t blockSize = 100 * 1024;  // 100KB块
            void* ptr = ConventionEngineMemoryAlloc(blockSize);
            ASSERT(ptr != nullptr);
            allocs.push_back(ptr);
            totalAllocated += blockSize;

            // 写入一些数据，验证内存可用
            memset(ptr, i % 256, blockSize);
        }

        std::cout << "总共分配了 " << (totalAllocated / (1024.0 * 1024.0)) << " MB内存" << std::endl;
        ASSERT(totalAllocated > INITIAL_SIZE);  // 确保内存池已扩容

        // 释放一些内存
        for (size_t i = 0; i < allocs.size(); i += 2) {
            ConventionEngineFreeMemory(allocs[i]);
            allocs[i] = nullptr;
        }

        // 执行GC
        GC_ConventionEngine();

        // 再次分配
        for (size_t i = 0; i < allocs.size(); i += 2) {
            if (allocs[i] == nullptr) {
                size_t blockSize = 50 * 1024;  // 50KB块
                allocs[i] = ConventionEngineMemoryAlloc(blockSize);
                ASSERT(allocs[i] != nullptr);
            }
        }
    }
    catch (const std::exception&) {
        // 清理已分配的内存
        for (void* ptr : allocs) {
            if (ptr != nullptr) {
                ConventionEngineFreeMemory(ptr);
            }
        }
        QuitConventionEngine();
        throw;  // 重新抛出异常
    }

    // 释放所有内存
    for (void* ptr : allocs) {
        if (ptr != nullptr) {
            ConventionEngineFreeMemory(ptr);
        }
    }

    // 清理引擎
    QuitConventionEngine();
}

// 测试多线程内存分配
void testMultithreadedAllocation() {
    // 初始化引擎
    InitConventionEngine(32 * 1024 * 1024);  // 32MB内存

    const int THREAD_COUNT = 4;
    const int ALLOCS_PER_THREAD = 10000;

    auto threadFunc = [ALLOCS_PER_THREAD](int threadId)
    {
        std::vector<void*> threadAllocs;

        for (int i = 0; i < ALLOCS_PER_THREAD; i++) {
            // 8-128字节随机大小
            size_t size = (rand() % 121) + 8;
            void* ptr = ConventionEngineMemoryAlloc(size);
            ASSERT(ptr != nullptr);
            threadAllocs.push_back(ptr);

            // 随机释放一些内存
            if (rand() % 4 == 0 && !threadAllocs.empty()) {
                size_t index = rand() % threadAllocs.size();
                ConventionEngineFreeMemory(threadAllocs[index]);
                threadAllocs[index] = threadAllocs.back();
                threadAllocs.pop_back();
            }
        }

        // 释放该线程的所有分配
        for (void* ptr : threadAllocs) {
            ConventionEngineFreeMemory(ptr);
        }

        std::cout << "线程 " << threadId << " 完成" << std::endl;
    };

    // 启动多个线程
    std::vector<std::thread> threads;
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.emplace_back(threadFunc, i);
    }

    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }

    // 执行GC
    GC_ConventionEngine();

    // 清理引擎
    QuitConventionEngine();
}

// 测试对象生命周期和句柄管理
void testObjectLifecycle() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024);

    // 创建多个对象并存储句柄
    const int OBJ_COUNT = 100;
    CE CEHandle handles[OBJ_COUNT];

    for (int i = 0; i < OBJ_COUNT; i++) {
        MemTestObject* obj = new MemTestObject();
        char name[32];
        sprintf(name, "TestObject_%d", i);
        obj->SetName(name);
        handles[i] = GetHandle(obj);
        ASSERT(handles[i] >= 0);
    }

    // 验证所有对象都可以通过句柄访问
    for (int i = 0; i < OBJ_COUNT; i++) {
        MemTestObject* obj = static_cast<MemTestObject*>(GetPtr(handles[i]));
        ASSERT(obj != nullptr);

        char expectedName[32];
        sprintf(expectedName, "TestObject_%d", i);
        ASSERT(strcmp(obj->GetName(), expectedName) == 0);
    }

    // 删除一半对象
    for (int i = 0; i < OBJ_COUNT; i += 2) {
        MemTestObject* obj = static_cast<MemTestObject*>(GetPtr(handles[i]));
        delete obj;
    }

    // 验证已删除的对象句柄返回nullptr，保留的仍有效
    for (int i = 0; i < OBJ_COUNT; i++) {
        void* ptr = GetPtr(handles[i]);
        if (i % 2 == 0) {
            ASSERT(ptr == nullptr);  // 已删除的对象
        } else {
            ASSERT(ptr != nullptr);  // 保留的对象
        }
    }

    // 删除剩余对象
    for (int i = 1; i < OBJ_COUNT; i += 2) {
        MemTestObject* obj = static_cast<MemTestObject*>(GetPtr(handles[i]));
        delete obj;
    }

    // 清理引擎
    QuitConventionEngine();
}

// 测试高级容器类型用法
void testAdvancedContainers() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024);

    // 创建嵌套容器
    CE Types::Vector<CE Types::Vector<int>> nestedVec;

    // 添加数据
    for (int i = 0; i < 10; i++) {
        CE Types::Vector<int> innerVec;
        for (int j = 0; j < 5; j++) {
            innerVec.push_back(i * 10 + j);
        }
        nestedVec.push_back(innerVec);
    }

    // 验证数据
    ASSERT(nestedVec.size() == 10);
    for (int i = 0; i < 10; i++) {
        ASSERT(nestedVec[i].size() == 5);
        for (int j = 0; j < 5; j++) {
            ASSERT(nestedVec[i][j] == i * 10 + j);
        }
    }

    // 测试多种STL容器
    {
        // 测试列表
        CE Types::List<CE Types::String> strList;
        strList.push_back("一");
        strList.push_back("二");
        strList.push_back("三");

        ASSERT(strList.size() == 3);
        ASSERT(*strList.begin() == "一");

        // 测试集合
        CE Types::Set<int> intSet;
        intSet.insert(5);
        intSet.insert(3);
        intSet.insert(7);
        intSet.insert(5);  // 重复项

        ASSERT(intSet.size() == 3);
        ASSERT(intSet.find(5) != intSet.end());
        ASSERT(intSet.find(3) != intSet.end());
        ASSERT(intSet.find(7) != intSet.end());

        // 测试映射
        CE Types::Map<std::string, CE Types::Vector<int>> complexMap;
        complexMap["键1"].push_back(1);
        complexMap["键1"].push_back(2);
        complexMap["键2"].push_back(3);

        ASSERT(complexMap.size() == 2);
        ASSERT(complexMap["键1"].size() == 2);
        ASSERT(complexMap["键2"].size() == 1);
        ASSERT(complexMap["键1"][0] == 1);
        ASSERT(complexMap["键1"][1] == 2);
        ASSERT(complexMap["键2"][0] == 3);
    }

    // 清理引擎
    QuitConventionEngine();
}

int main() {
    try {
        std::cout << "开始ConventionEngine内存管理单元测试...\n" << std::endl;

        TEST_CASE(testMemoryPerformance);
        TEST_CASE(testMemoryStress);
        TEST_CASE(testMultithreadedAllocation);
        TEST_CASE(testObjectLifecycle);
        TEST_CASE(testAdvancedContainers);

        std::cout << "\n所有内存测试通过！" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}