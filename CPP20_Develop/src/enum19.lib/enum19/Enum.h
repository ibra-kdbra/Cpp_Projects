#pragma once
#include "ADL.h"
#include "array19/Array.h"
#include "meta19/nullptr_to.h"
#include "string19/StringView.h"
#include "string19/StringView.literal.h"
#include "visitEnumMemberNames.h"

#include <stddef.h> // size_t
#include <type_traits> // std::underlyng_type_t

#ifndef ENUM19_EXTRAS
#    define ENUM19_EXTRAS(NAME)
#endif

#define ENUM19(NAME, UNDERLYING, ...)                                                                                  \
    enum class NAME : UNDERLYING { __VA_ARGS__ };                                                                      \
    constexpr auto metaEnumFor(enum19::ADL*, NAME*) {                                                                  \
        using Enum19_ValueWrapper = ::enum19::details::ValueWrapper<UNDERLYING>;                                       \
        constexpr Enum19_ValueWrapper __VA_ARGS__;                                                                     \
        return ::enum19::details::buildMetaEnumFor<NAME>(#NAME, #__VA_ARGS__, __VA_ARGS__);                            \
    }                                                                                                                  \
    ENUM19_EXTRAS(NAME)

namespace enum19 {

using array19::Array;
using meta19::nullptr_to;
using string19::StringView;

template<class T> constexpr void metaEnumFor(ADL*, T*) {
    static_assert(std::is_enum_v<T>, "metaEnumFor can only work for an enum declared with ENUM19 macro!");
    static_assert(sizeof(T) && false, "Declare enum class with ENUM19 macro to get access to metaEnum!");
}

template<class T> constexpr auto meta_enum_for = metaEnumFor(adl, nullptr_to<T>);

template<class T> concept HasMetaEnum = (!std::is_same_v<void, decltype(metaEnumFor(adl, nullptr_to<T>))>);

template<class Enum> struct MetaEnumMember {
    size_t index;
    StringView name;
    Enum value;

    constexpr MetaEnumMember(size_t index, StringView name, Enum value) : index(index), name(name), value(value) {}
};

template<class Enum, size_t member_count> struct MetaEnum {
    StringView name{};
    StringView body{};
    Array<MetaEnumMember<Enum>, member_count> members{};
};

namespace details {

using string19::ConstCharArray;
using string19::viewLiteral;

template<class Underlying> struct ValueWrapper {
    bool filled{};
    Underlying value{};
    constexpr ValueWrapper() = default;
    constexpr ValueWrapper(Underlying in) : filled(true), value(in) {}
    constexpr ValueWrapper operator=(Underlying in) { return filled = true, value = in, *this; }
    constexpr ValueWrapper operator=(Underlying) const { return *this; }
    constexpr operator Underlying() const { return value; }
};

template<class... Args> constexpr auto countArgs(Args&&...) -> size_t { return sizeof...(Args); }

template<class Enum, size_t NameN, size_t BodyN, class... Args>
constexpr auto buildMetaEnumFor(
    const ConstCharArray<NameN>& name,
    const ConstCharArray<BodyN>& body,
    Args&&... members) {

    constexpr auto memberCount = sizeof...(members);
    using Underlying = std::underlying_type_t<Enum>;
    auto memberNames = extractEnumMemberNames<memberCount>(viewLiteral(body));

    auto nextValue = Underlying{};
    auto value = Underlying{};
    auto index = size_t{};
    auto nameIndex = size_t{};
    return MetaEnum<Enum, memberCount>{
        viewLiteral(name),
        viewLiteral(body),
        {MetaEnumMember<Enum>{
            index++,
            memberNames[nameIndex++],
            ((value = members.filled ? members.value : nextValue),
             nextValue = value + 1,
             static_cast<Enum>(value)) //
        }...} //
    };
}

} // namespace details

} // namespace enum19
