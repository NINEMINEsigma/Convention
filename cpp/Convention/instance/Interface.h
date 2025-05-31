#ifndef CONVENTION_KIT_INTERFACE_H
#define CONVENTION_KIT_INTERFACE_H

#include "Convention/Internal.h"

namespace Convention
{
    // default deleter for unique_ptr
    template <class _Ty, template<typename> class _Alloc>
    struct DefaultDelete
    {
        constexpr DefaultDelete() noexcept = default;

        template <class _Ty2, enable_if_t<std::is_convertible_v<_Ty2*, _Ty*>, int> = 0>
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

    /**
     * @brief 支持内存控制的实体
     * @tparam T 目标类型, 不支持数组
     * @tparam _is_extension 是否为扩展实体
     * @tparam Allocator 内存管理器
     * @tparam PtrType 智能指针类型
     * @tparam ExtensionPtrArgs 智能指针扩展模板参数包
     */
    template<
        typename T,
        bool _is_extension = true,
        template<typename...> class Allocator = std::allocator,
        template<typename...> class PtrType = std::shared_ptr,
        typename... ExtensionPtrArgs
    >
    class instance : public AnyClass, public PtrType<T, ExtensionPtrArgs...>
    {
    private:
        using _Mybase = PtrType<T, ExtensionPtrArgs...>;
    public:
        using _MyType = T;
        using _MyAlloc = Allocator<T>;
        using _RootMetaBase = PtrType<T, ExtensionPtrArgs...>;

        static _MyAlloc& GetStaticMyAllocator()
        {
            static _MyAlloc alloc;
            return alloc;
        }

        static constexpr bool is_extension = _is_extension;
    private:
        void* operator new(size_t t) { return ::operator new(t); }
    public:
        template<typename... Args>
        instance(Args&&... args) : _Mybase(std::forward<Args>(args)...) {}
        virtual ~instance() {}

        bool IsEmpty() const noexcept
        {
            return this->get() != nullptr;
        }

        T& ReadValue()
        {
            return *(this->get());
        }
        const T& ReadConstValue() const
        {
            return *(this->get());
        }

        template<typename... Args>
        instance& operator=(Args&&... args)
        {
            _Mybase::operator=(std::forward<Args>(args)...);
            return *this;
        }

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
        template<typename...> class PtrType,
        typename... ExtensionPtrArgs
    >
    TOutputStream& operator<<(
        TOutputStream& os,
        const instance<T, false, Allocator, PtrType, ExtensionPtrArgs...>& ins
        )
    {
        os << *ins;
        return os;
    }template<
        typename TOutputStream,
        typename T,
        template<typename...> class Allocator,
        template<typename...> class PtrType,
        typename... ExtensionPtrArgs
    >
        TOutputStream& operator<<(
            TOutputStream& os,
            const instance<T, true, Allocator, PtrType, ExtensionPtrArgs...>& ins
            )
    {
        os << ins.ToString();
        return os;
    }

}

#endif // !CONVENTION_KIT_INTERFACE_H
