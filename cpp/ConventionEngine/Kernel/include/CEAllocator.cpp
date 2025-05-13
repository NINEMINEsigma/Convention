#include "ConventionEngine/Kernel/CoreModule.h"

// 显式实例化CEAllocator的定义部分

namespace ConventionEngine::Kernel
{
    // 实现String和WString相关的显式实例化
    template class CEAllocator<char>;
    template class CEAllocator<wchar_t>;

    // 基本数据类型的显式实例化
    template class CEAllocator<int>;
    template class CEAllocator<float>;
    template class CEAllocator<double>;
    template class CEAllocator<long>;
    template class CEAllocator<unsigned int>;
    template class CEAllocator<unsigned long>;
    template class CEAllocator<bool>;

    // pair类型的显式实例化
    template class CEAllocator<std::pair<const int, int>>;
    template class CEAllocator<std::pair<const std::string, std::string>>;
    template class CEAllocator<std::pair<const int, std::string>>;
    template class CEAllocator<std::pair<const std::string, int>>;

    // 使用示例 - 创建一些容器的实例化示例，这样可以确保链接时这些实现被包含
    namespace AllocatorInstances
    {
        // 字符串类型的实例化
        template class std::basic_string<char, std::char_traits<char>, CEAllocator<char>>;
        template class std::basic_string<wchar_t, std::char_traits<wchar_t>, CEAllocator<wchar_t>>;

        // 向量类型的实例化
        template class std::vector<int, CEAllocator<int>>;
        template class std::vector<std::string, CEAllocator<std::string>>;
        template class std::vector<char, CEAllocator<char>>;

        // 列表类型的实例化
        template class std::list<int, CEAllocator<int>>;
        template class std::list<std::string, CEAllocator<std::string>>;

        // 映射类型的实例化
        template class std::map<int, int, std::less<int>, CEAllocator<std::pair<const int, int>>>;
        template class std::map<std::string, int, std::less<std::string>, CEAllocator<std::pair<const std::string, int>>>;
        template class std::map<int, std::string, std::less<int>, CEAllocator<std::pair<const int, std::string>>>;

        // 无序映射类型的实例化
        template class std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, CEAllocator<std::pair<const int, int>>>;
        template class std::unordered_map<std::string, int, std::hash<std::string>, std::equal_to<std::string>, CEAllocator<std::pair<const std::string, int>>>;

        // 集合类型的实例化
        template class std::set<int, std::less<int>, CEAllocator<int>>;
        template class std::set<std::string, std::less<std::string>, CEAllocator<std::string>>;

        // 无序集合类型的实例化
        template class std::unordered_set<int, std::hash<int>, std::equal_to<int>, CEAllocator<int>>;
        template class std::unordered_set<std::string, std::hash<std::string>, std::equal_to<std::string>, CEAllocator<std::string>>;

        // 双端队列类型的实例化
        template class std::deque<int, CEAllocator<int>>;
        template class std::deque<std::string, CEAllocator<std::string>>;
    }
}