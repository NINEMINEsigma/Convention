#ifndef CONVENTION_KIT_INTERFACE_H
#define CONVENTION_KIT_INTERFACE_H

#include "Convention/Internal.h"

namespace Convention
{
    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    // default deleter for unique_ptr
    template <class _Ty, template<typename> class _Alloc>
    struct DefaultDelete
    {
        constexpr DefaultDelete() noexcept = default;

        template <class _Ty2, std::enable_if_t<std::is_convertible_v<_Ty2*, _Ty*>, int> = 0>
        _CONSTEXPR23 DefaultDelete(const DefaultDelete<_Ty2, _Alloc>&) noexcept {}

        _CONSTEXPR23 void operator()(_Ty* _Ptr) const noexcept /* strengthened */
        {
            // delete a pointer
            static_assert(0 < sizeof(_Ty), "can't delete an incomplete type");
            static _Alloc<_Ty> alloc;
            alloc.destroy(_Ptr);
            alloc.deallocate(_Ptr, 1);
        }
    };

    template<typename T, typename Deleter = DefaultDelete<T, std::allocator>>
    using UniquePtr = std::unique_ptr<T, Deleter>;

    template<typename T>
	using WeakPtr = std::weak_ptr<T>;

    /**
     * @brief 支持内存控制的实体
     * @tparam T 目标类型, 不支持数组
     * @tparam _is_extension 是否为扩展实体
     * @tparam Allocator 内存管理器
     * @tparam _is_unique 指示智能指针类型
     */
    template<
        typename T,
        bool _is_extension = true,
        template<typename...> class Allocator = std::allocator,
        bool _is_unique = false
    >
    class instance : public AnyClass, public std::conditional_t<_is_unique, UniquePtr<T, DefaultDelete<T, Allocator>>, SharedPtr<T>>
    {
    private:
        using _Mybase = std::conditional_t<_is_unique, UniquePtr<T, DefaultDelete<T, Allocator>>, SharedPtr<T>>;
        void* operator new(size_t t) { return ::operator new(t); }
    public:
        using _MyType = T;
        using _MyAlloc = Allocator<T>;
        constexpr static bool _MyExtension = _is_extension;
        using _RootMetaBase = _Mybase;
    protected:
        /**
        * @brief 获取内存管理器
        */
        static _MyAlloc& GetStaticMyAllocator()
        {
            static _MyAlloc alloc;
            return alloc;
        }
        template<typename... Args>
        static T* BuildMyPtr(Args&&... args)
        {
            T* ptr = GetStaticMyAllocator().allocate(1);
            GetStaticMyAllocator().construct(ptr, std::forward<Args>(args)...);
            return ptr
        }
        static void _DestoryMyPtr(_In_ T* ptr)
        {
            GetStaticMyAllocator().destroy(ptr);
            GetStaticMyAllocator().deallocate(ptr, 1);
        }
    public:
        /**
        * @brief 任意匹配的构造函数
        */
        template<typename... Args>
        instance(Args&&... args) : _Mybase(UniquePtr<T, DefaultDelete<T, Allocator>>(std::forward<Args>(args)...)) {}
        virtual ~instance() {}

        /**
        * @brief 是否为空指针
        */
        bool IsEmpty() const noexcept
        {
            return this->get() != nullptr;
        }

        /**
        * @brief 读取值(引用方式)
        */
        T& ReadValue()
        {
            return *(this->get());
        }
        /**
        * @brief 设置值(引用方式)
        * @tparam Arg 传递值
        */
        template<typename Arg>
        T& WriteValue(Arg&& value)
        {
            *(this->get()) = std::forward<Arg>(value);
            return value;
        }
        /**
        * @brief 读取const值(引用方式)
        */
        const T& ReadConstValue() const
        {
            return *(this->get());
        }

        /**
        * @brief 拷贝赋值函数
        */
        virtual instance& operator=(const instance& value) noexcept
        {
            if constexpr (_is_unique)
            {
                this->WriteValue(value.ReadConstValue());
            }
            else
            {
                _Mybase::operator=(value);
            }
            return *this;
        }
        /**
        * @brief 移动赋值函数
        */
        virtual instance& operator=(instance&& value) noexcept
        {
            _Mybase::operator=(std::move(value));
            return *this;
        }
        ///**
        //* @brief 匹配任意赋值函数
        //*/
        //template<typename... Args>
        //instance& operator=(Args&&... args)
        //{
        //    _Mybase::operator=(std::forward<Args>(args)...);
        //    return *this;
        //}

        virtual std::string ToString() const noexcept override
        {
            std::string result(64, 0);
            ::sprintf(result.data(), "instance<%p>", this->get());
            return result;
        }
        virtual std::string SymbolName() const noexcept override
        {
            auto front = Typeame2Classname(this->GetType().name());
            front = front.substr(0,
                std::distance(front.begin(), std::find(front.begin(), front.end(), '<'))
            );
            return front + "<" + Typeame2Classname(typeid(T).name()) + ">";
        }
    };

    template<
        typename TOutputStream,
        typename T,
        template<typename...> class Allocator,
        bool _is_unique
    >
    TOutputStream& operator<<(
        TOutputStream& os,
        const instance<T, false, Allocator, _is_unique>& ins
        )
    {
        os << *ins;
        return os;
    }template<
        typename TOutputStream,
        typename T,
        template<typename...> class Allocator,
        bool _is_unique
    >
        TOutputStream& operator<<(
            TOutputStream& os,
            const instance<T, true, Allocator, _is_unique>& ins
            )
    {
        os << ins.ToString();
        return os;
    }

}

#endif // !CONVENTION_KIT_INTERFACE_H
