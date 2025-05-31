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

    template<typename T, typename Deleter = DefaultDelete<T, std::allocator>>
    using UniquePtr = std::unique_ptr<T, Deleter>;

    /**
     * @brief ֧���ڴ���Ƶ�ʵ��
     * @tparam T Ŀ������, ��֧������
     * @tparam _is_extension �Ƿ�Ϊ��չʵ��
     * @tparam Allocator �ڴ������
     * @tparam PtrType ����ָ������
     * @tparam ExtensionPtrArgs ����ָ����չģ�������
     */
    template<
        typename T,
        bool _is_extension = true,
        template<typename...> class Allocator = std::allocator,
        template<typename...> class PtrType = SharedPtr,
        typename... ExtensionPtrArgs
    >
    class instance : public AnyClass, public PtrType<T, ExtensionPtrArgs...>
    {
    private:
        using _Mybase = PtrType<T, ExtensionPtrArgs...>;
        void* operator new(size_t t) { return ::operator new(t); }
    public:
        using _MyType = T;
        using _MyAlloc = Allocator<T>;
        constexpr static bool _MyExtension = _is_extension;
        using _RootMetaBase = PtrType<T, ExtensionPtrArgs...>;
    protected:
        /**
        * @brief ��ȡ�ڴ������
        */
        static _MyAlloc& GetStaticMyAllocator()
        {
            static _MyAlloc alloc;
            return alloc;
        }
        template<typename... Args>
        static T* _BuildMyPtr(Args&&... args)
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
        * @brief ����ƥ��Ĺ��캯��
        */
        template<typename... Args>
        instance(Args&&... args) : _Mybase(std::forward<Args>(args)...) {}
        virtual ~instance() {}

        /**
        * @brief �Ƿ�Ϊ��ָ��
        */
        bool IsEmpty() const noexcept
        {
            return this->get() != nullptr;
        }

        /**
        * @brief ��ȡֵ(���÷�ʽ)
        */
        T& ReadValue()
        {
            return *(this->get());
        }
        /**
        * @brief ��ȡconstֵ(���÷�ʽ)
        */
        const T& ReadConstValue() const
        {
            return *(this->get());
        }

        /**
        * @brief ƥ�����⸳ֵ����
        */
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
