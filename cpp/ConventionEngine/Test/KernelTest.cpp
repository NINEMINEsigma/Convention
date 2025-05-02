#include <iostream>
#include <cassert>
#include <cstring>
#include "Convention.h"
#include "ConventionEngine/Kernel/CoreModule.h"

using namespace ConventionEngine;

// 测试框架简单实现
#define TEST_CASE(name) void name(); std::cout << "运行测试: " << #name << std::endl; name(); std::cout << #name << " 测试通过！" << std::endl;
#define ASSERT(condition) if(!(condition)) { std::cerr << "测试失败: " << __FILE__ << ":" << __LINE__ << std::endl; throw std::runtime_error("断言失败"); }

// 创建一个继承自CEObject的测试类
class TestObject : public CE CEObject {
private:
    int m_value;
    CE Types::String m_data;

protected:
    // 重写创建对象方法
    virtual CE CEObject* CreateObject() const override {
        return new TestObject();
    }

    // 重写克隆值方法
    virtual void CloneValuesTo(_In_ CE CEObject* target) const override {
        TestObject* testTarget = static_cast<TestObject*>(target);
        testTarget->m_value = this->m_value;
        testTarget->m_data = this->m_data;
    }

public:
    TestObject() : m_value(0), m_data("") {}

    void SetValue(int value) { m_value = value; }
    int GetValue() const { return m_value; }

    void SetData(const CE Types::String& data) { m_data = data; }
    const CE Types::String& GetData() const { return m_data; }
};

// 测试基本功能
void testBasicFunctionality() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024); // 初始分配1MB内存

    // 创建测试对象
    TestObject* obj = new TestObject();
    ASSERT(obj != nullptr);

    // 测试对象命名
    obj->SetName("TestObject");
    ASSERT(strcmp(obj->GetName(), "TestObject") == 0);

    // 测试对象句柄获取
    CE CEHandle handle = GetHandle(obj);
    ASSERT(handle >= 0);

    // 测试通过句柄获取对象
    void* ptrFromHandle = GetPtr(handle);
    ASSERT(ptrFromHandle == obj);

    // 测试通过句柄设置/获取名称
    SetName(handle, "RenamedObject");
    ASSERT(strcmp(GetName(handle), "RenamedObject") == 0);
    ASSERT(strcmp(obj->GetName(), "RenamedObject") == 0);

    // 清理
    delete obj;

    // 确保对象已被释放
    ASSERT(GetPtr(handle) == nullptr);

    // 清理引擎
    QuitConventionEngine();
}

// 测试内存分配和释放
void testMemoryAllocFree() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024);

    // 测试内存分配
    void* mem1 = ConventionEngineMemoryAlloc(128);
    ASSERT(mem1 != nullptr);

    // 写入一些数据
    memset(mem1, 0xAA, 128);

    // 分配第二块内存
    void* mem2 = ConventionEngineMemoryAlloc(256);
    ASSERT(mem2 != nullptr);
    ASSERT(mem2 != mem1);

    // 释放第一块
    ConventionEngineFreeMemory(mem1);

    // 再次分配，应该会重用刚释放的内存
    void* mem3 = ConventionEngineMemoryAlloc(128);

    // 释放剩余内存
    ConventionEngineFreeMemory(mem2);
    ConventionEngineFreeMemory(mem3);

    // 清理引擎
    QuitConventionEngine();
}

// 测试CE::Types容器
void testContainerTypes() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024);

    // 测试CE::Types::String
    {
        CE Types::String str("测试字符串");
        ASSERT(str.length() > 0);
        ASSERT(str == "测试字符串");

        str += " 追加内容";
        ASSERT(str == "测试字符串 追加内容");
    }

    // 测试CE::Types::Vector
    {
        CE Types::Vector<int> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);

        ASSERT(vec.size() == 3);
        ASSERT(vec[0] == 1);
        ASSERT(vec[1] == 2);
        ASSERT(vec[2] == 3);
    }

    // 测试CE::Types::Map
    {
        CE Types::Map<std::string, int> map;
        map["one"] = 1;
        map["two"] = 2;
        map["three"] = 3;

        ASSERT(map.size() == 3);
        ASSERT(map["one"] == 1);
        ASSERT(map["two"] == 2);
        ASSERT(map["three"] == 3);
    }

    // 清理引擎
    QuitConventionEngine();
}

// 测试垃圾回收
void testGarbageCollection() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024);

    // 分配多个对象和内存块，制造内存碎片
    const int allocCount = 1000;
    void* blocks[allocCount];

    // 分配
    for (int i = 0; i < allocCount; i++) {
        // 随机大小, 8到128字节
        size_t size = (rand() % 121) + 8;
        blocks[i] = ConventionEngineMemoryAlloc(size);
        ASSERT(blocks[i] != nullptr);
    }

    // 释放部分内存，制造碎片
    for (int i = 0; i < allocCount; i += 2) {
        ConventionEngineFreeMemory(blocks[i]);
        blocks[i] = nullptr;
    }

    // 执行垃圾回收
    GC_ConventionEngine();

    // 尝试再次分配
    for (int i = 0; i < allocCount; i += 2) {
        size_t size = (rand() % 121) + 8;
        blocks[i] = ConventionEngineMemoryAlloc(size);
        ASSERT(blocks[i] != nullptr);
    }

    // 释放所有内存
    for (int i = 0; i < allocCount; i++) {
        if (blocks[i] != nullptr) {
            ConventionEngineFreeMemory(blocks[i]);
        }
    }

    // 清理引擎
    QuitConventionEngine();
}

// 测试自定义CEObject类的具体功能
void testCustomCEObject() {
    // 初始化引擎
    InitConventionEngine(1024 * 1024);

    // 创建测试对象
    TestObject* obj = new TestObject();
    obj->SetName("CustomTestObject");
    obj->SetValue(42);
    obj->SetData(CE Types::String("测试数据"));

    // 验证对象值
    ASSERT(strcmp(obj->GetName(), "CustomTestObject") == 0);
    ASSERT(obj->GetValue() == 42);
    ASSERT(obj->GetData() == "测试数据");

    // 获取对象句柄
    CE CEHandle handle = GetHandle(obj);
    ASSERT(handle >= 0);

    // 删除对象并清理
    delete obj;
    ASSERT(GetPtr(handle) == nullptr);

    // 清理引擎
    QuitConventionEngine();
}

int main() {
    try {
        std::cout << "开始ConventionEngine内核单元测试...\n" << std::endl;

        TEST_CASE(testBasicFunctionality);
        TEST_CASE(testMemoryAllocFree);
        TEST_CASE(testContainerTypes);
        TEST_CASE(testGarbageCollection);
        TEST_CASE(testCustomCEObject);

        std::cout << "\n所有测试通过！" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}