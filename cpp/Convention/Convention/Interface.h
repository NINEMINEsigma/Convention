#ifndef __FILE_INCLUDE_INTERNAL
#define __FILE_INCLUDE_INTERNAL

#include "Internal.h"

// define _shared to control real behaviour
template<typename _Type>
using _shared_ptr = std::shared_ptr<_Type>;

template<
    typename _Type,
	bool _Extension = true
>
class instance : public any_class, public _shared_ptr<_Type>
{
public:
    using _shared = _shared_ptr<_Type>;
    using _MyBase = _shared_ptr<_Type>;
    using _MyType = _Type;
    static constexpr bool _is_extension = _Extension;
private:
    void* operator new(size_t t) {}
    void operator delete(void* ptr) {}
    using _Mybase = _shared;
public:
    constexpr instance() :_MyBase() {}
    constexpr instance(nullptr_t) : _MyBase(nullptr) {}
    explicit instance(_Type* ptr) : _MyBase(ptr) {}
    template<typename... _Args>
    instance(_Args&&... args) : _MyBase(std::forward<_Args>(args)...) {}
    virtual ~instance() {}

    bool is_empty() const noexcept
    {
        return this->get() != nullptr;
    }

    virtual std::string ToString() const noexcept override
    {
        std::string result(64, 0);
        sprintf(result.data(), "%p", this->get());
        return result;
    }
    virtual std::string SymbolName() const noexcept override
    {
        return typename2classname(this->GetType().name()) + "<" +
            typename2classname(typeid(_Type).name()) + ">";
    }
};

namespace internal
{
    template<typename _Type>
    constexpr bool is_number_v = 
        std::is_floating_point_v<_Type> ||
        std::is_integral_v<_Type>;
    namespace
    {
        template<typename _Type>
        struct is_instance :public std::false_type {};
        template<typename _Ty>
        struct is_instance<instance<_Ty, true>> :public std::true_type {};
        template<typename _Ty>
        struct is_instance<instance<_Ty, false>> :public std::true_type {};
    }
    template<typename _Type>
    constexpr bool is_instance_v = is_instance<std::remove_reference_t<_Type>>();
}

template<typename _Type, class _Elem, class _Traits, class _Ty>
std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& _Out, const instance<_Type, true>& tc)
{
    _Out << tc.ToString();
    return _Out;
}

#endif // !__FILE_INCLUDE_INTERNAL
