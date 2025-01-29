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
private:
    using _Mybase = _shared_ptr<_Type>;
public:
    using _shared = _shared_ptr<_Type>;
    using _MyType = _Type;
    static constexpr bool _is_extension = _Extension;
private:
    void* operator new(size_t t) {}
    void operator delete(void* ptr) {}
public:
    constexpr instance() :_Mybase() {}
    constexpr instance(nullptr_t) : _Mybase(nullptr) {}
    explicit instance(_Type* ptr) : _Mybase(ptr) {}
    explicit instance(_shared& rv) noexcept :_Mybase(rv) {}
    explicit instance(_shared&& rv) noexcept :_Mybase(std::move(rv)) {}
    explicit instance(instance&& other) noexcept :_Mybase(std::move(other)) {}
    template<typename... _Args>
    instance(_Args&&... args) : _Mybase(std::forward<_Args>(args)...) {}
    virtual ~instance() {}

    bool is_empty() const noexcept
    {
        return this->get() != nullptr;
    }

    instance& operator=(const instance& other) noexcept
    {
        _Mybase::operator=(other);
        return *this;
    }
    template<typename... _Args>
    instance& operator=(_Args&&... args)
    {
        _Mybase::operator=(std::forward<_Args>(args)...);
        return *this;
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

#define instance_move_operator(internal) public:\
explicit instance(instance&& other) noexcept:_Mybase(std::move(other))\
{\
    this->move(std::move(other));\
}\
instance& operator=(instance&& other) noexcept\
{\
    _Mybase::operator=(std::move(other));\
    this->move(std::move(other));\
    return *this;\
}\
internal:\
void move(instance&& other) noexcept

namespace internal
{
    template<typename _Type>
    constexpr bool is_number_v = 
        std::is_floating_point_v<_Type> ||
        std::is_integral_v<_Type>;
    template<typename _Type>
    constexpr bool is_stream_v = std::is_base_of_v<std::ios_base, _Type>;
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
