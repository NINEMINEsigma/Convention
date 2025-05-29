#ifndef CONVENTION_KIT_INTERFACE_H
#define CONVENTION_KIT_INTERFACE_H

#include "Convention/Internal.h"

// define _shared to control real behaviour
template<typename TType>
using _shared_ptr = std::shared_ptr<TType>;

template<
    typename TType,
    bool TExtension = true
>
class instance : public any_class, public _shared_ptr<TType>
{
private:
    using _Mybase = _shared_ptr<TType>;
public:
    using _shared = _shared_ptr<TType>;
    using _MyType = TType;
    using _MyAlloc = std::allocator<TType>;
    static constexpr bool _is_extension = TExtension;
private:
    void* operator new(size_t t) { return ::operator new(t); }
public:
    constexpr instance() :_Mybase() {}
    constexpr instance(nullptr_t) : _Mybase(nullptr) {}
    explicit instance(TType* ptr) : _Mybase(ptr) {}
    explicit instance(_shared& rv) noexcept :_Mybase(rv) {}
    explicit instance(_shared&& rv) noexcept :_Mybase(std::move(rv)) {}
    explicit instance(instance&& other) noexcept :_Mybase(std::move(other)) {}
    template<typename... TArgs>
    instance(TArgs&&... args) : _Mybase(std::forward<TArgs>(args)...) {}
    virtual ~instance() {}

    operator TType& () const
    {
        return *this->get();
    }
    operator TType* () const noexcept
    {
        return this->get();
    }

    bool IsEmpty() const noexcept
    {
        return this->get() != nullptr;
    }

    instance& operator=(const instance& other) noexcept
    {
        _Mybase::operator=(other);
        return *this;
    }
    template<typename... TArgs>
    instance& operator=(TArgs&&... args)
    {
        _Mybase::operator=(std::forward<TArgs>(args)...);
        return *this;
    }

    virtual std::string ToString() const noexcept override
    {
        std::string result(64, 0);
        sprintf(result.data(), "instance<%p>", this->get());
        return result;
    }
    virtual std::string SymbolName() const noexcept override
    {
        auto front = typename2classname(this->GetType().name());
        front = front.substr(0,
            std::distance(front.begin(), std::find(front.begin(), front.end(), '<'))
        );
        return front + "<" + typename2classname(typeid(TType).name()) + ">";
    }
};

template<typename TOutputStream, typename TType>
TOutputStream& operator<<(TOutputStream& os, const instance<TType, false>& ins)
{
    os << *ins;
    return os;
}

#define instance_move_operator(internal) public:\
instance(instance&& other) noexcept:_Mybase(std::move(other))\
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
    template<typename TType>
    constexpr bool IsNumberV =
        std::is_floating_point_v<TType> ||
        std::is_integral_v<TType>;
    template<typename TType>
    constexpr bool IsStreamV = std::is_base_of_v<std::ios_base, TType>;
    template<typename TType>
    constexpr bool IsStringV =
            std::is_same_v<std::decay_t<std::remove_reference_t<std::remove_cv_t<TType>>>, std::string>||
            std::is_same_v<std::decay_t<std::remove_reference_t<std::remove_cv_t<TType>>>, char*>;
    namespace
    {
        template<typename TType>
        struct IsInstance :public std::false_type {};
        template<typename TValue>
        struct IsInstance<instance<TValue, true>> :public std::true_type {};
        template<typename TValue>
        struct IsInstance<instance<TValue, false>> :public std::true_type {};
    }
    template<typename TType>
    constexpr bool IsInstanceV = IsInstance<std::remove_reference_t<TType>>();
}

template<typename TType, class TElement, class TTraits, class TValue>
std::basic_ostream<TElement, TTraits>& operator<<(std::basic_ostream<TElement, TTraits>& out, const instance<TType, true>& tc)
{
    out << tc.ToString();
    return out;
}

#endif // !CONVENTION_KIT_INTERFACE_H
