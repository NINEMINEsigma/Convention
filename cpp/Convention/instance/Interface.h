#ifndef CONVENTION_KIT_INTERFACE_H
#define CONVENTION_KIT_INTERFACE_H

#include "Convention/Internal.h"

namespace Convention
{

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
        template<typename...> class PtrType = std::shared_ptr,
        typename... ExtensionPtrArgs
    >
    TOutputStream& operator<<(
        TOutputStream& os,
        const instance<T, false, PtrType, ExtensionPtrArgs...>& ins
        )
    {
        os << *ins;
        return os;
    }template<
        typename TOutputStream,
        typename T,
        template<typename...> class PtrType = std::shared_ptr,
        typename... ExtensionPtrArgs
    >
        TOutputStream& operator<<(
            TOutputStream& os,
            const instance<T, true, PtrType, ExtensionPtrArgs...>& ins
            )
    {
        os << ins.ToString();
        return os;
    }

}

#endif // !CONVENTION_KIT_INTERFACE_H
