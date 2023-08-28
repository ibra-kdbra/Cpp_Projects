#pragma once
#include "Bitset.h"
#include "align.h"
#include "array19/SliceOf.carray.h"
#include "array19/SliceOf.max.h"
#include "meta19/Index.h"
#include "meta19/RemoveReference.h"
#include "meta19/TypeAt.h"
#include "meta19/TypePack.h"

#include <cstdint> // uint8_t
#include <new> // std::launder, operator new[], operator delete[]
#include <stddef.h> // size_t
#include <type_traits>

namespace partial19 {

using array19::sliceMaximum;
using array19::sliceOfCArray;
using meta19::_index;
using meta19::Index;
using meta19::index_of_map;
using meta19::IndexTypeMap;
using meta19::nullptr_to;
using meta19::StoredOf;
using meta19::Type;
using meta19::type;
using meta19::TypeAtMap;
using meta19::TypePack;

namespace details {

template<bool> struct ConditionalPlacementNew;
template<> struct ConditionalPlacementNew<true> {
    template<class T> static auto apply(T&& v, void* ptr) {
        using V = std::remove_const_t<std::remove_reference_t<T>>;
        new (ptr) V((T &&) v);
    }
};
template<> struct ConditionalPlacementNew<false> {
    template<class T> static auto apply(T&&, void*) {}
};

} // namespace details

/// Stores for each type of the partial wether it is initialized
/// note: semantic wrapper for Bitset
template<class... Ts> struct PartialWhich {
    using Bits = Bitset<sizeof...(Ts)>;
    using Map = IndexTypeMap<Ts...>;

    constexpr PartialWhich() = default;
    explicit constexpr PartialWhich(Bits v) : m(v) {}

    /// Sets the bit for each type given
    template<class... Ws, template<class...> class Tpl = TypePack>
    [[nodiscard]] constexpr static auto makeFor(Tpl<Ws...>* = {}) -> PartialWhich {
        auto bits = Bits{};
        (bits.setAt(index_of_map<Ws, Map>), ...);
        return PartialWhich{bits};
    }

    constexpr bool operator==(const PartialWhich& o) const = default;

    /// Extract copy of the underlying Bitset
    constexpr operator Bits() const { return m; }

    /// returns true if bit for the given index is set
    [[nodiscard]] constexpr bool operator[](size_t i) const { return m[i]; }

    /// returns true if bit for given type is set
    template<class T> [[nodiscard]] constexpr bool of(Type<T>* = {}) const { return m[index_of_map<T, Map>]; }

private:
    Bits m{};
};

/// Container that can store zero or one of each given type.
/// Storage is dynamically allocated on construction.
/// Changing stored types during live time is not supported.
///
/// Preconditions:
/// • Each type can only be placed once in this Partial implementation!
template<class... Ts> struct Partial {
    using Which = PartialWhich<Ts...>;
    using Bits = typename Which::Bits;
    using Map = typename Which::Map;
    static constexpr size_t sizeof_ts[] = {sizeof(Ts)...};
    static constexpr size_t alignof_ts[] = {alignof(Ts)...};
    enum : size_t { count = sizeof...(Ts), max_align = sliceMaximum(sliceOfCArray(alignof_ts)) };

private:
    Bits m_bits{};
    uint8_t* m_pointer{};

public:
    Partial() noexcept = default;
    ~Partial() { destructAll(); }

    // move
    Partial(Partial&& o) noexcept : m_bits(o.m_bits), m_pointer(o.m_pointer) {
        o.m_bits.reset();
        o.m_pointer = nullptr;
    }
    Partial& operator=(Partial&& o) noexcept {
        destructAll();
        m_bits = o.m_bits;
        m_pointer = o.m_pointer;
        o.m_bits.reset();
        o.m_pointer = nullptr;
        return *this;
    }

    // copy
    Partial(const Partial& o)
            : Partial(fromFactory(
                  [&](auto i) { return o.m_bits[i]; },
                  [&]<class T>(Type<T>*, void* ptr) { new (ptr) T(o.template of<T>()); })) {}
    auto operator=(const Partial& o) -> Partial& {
        auto hasValue = [&](auto i) { return o.m_bits[i]; };
        auto factory = [&]<class T>(Type<T>*, void* ptr) { new (ptr) T(o.template of<T>()); };
        return *this = fromFactory(hasValue, factory);
    }

    // Fastest way to construct Partial
    //
    // functor: hasValue(size_t index) -> bool - is called twice and has to return same decision!
    // functor: factory(Type<T>*, void* ptr) -> void - should placement new (ptr) T(…)
    template<class HasValue, class Factory>
    [[nodiscard]] static auto fromFactory(HasValue&& hasValue, Factory&& factory) -> Partial {
        auto r = Partial{};
        auto size = storageSize(hasValue);
        if (!size) return r;
        if constexpr (__STDCPP_DEFAULT_NEW_ALIGNMENT__ < max_align) {
            r.m_pointer = static_cast<uint8_t*>(::operator new[](size, std::align_val_t{max_align}));
        }
        else {
            r.m_pointer = new uint8_t[size];
        }
        r.constructAll(hasValue, factory);
        return r;
    }

    // Simple way to construct from given arguments
    // StoredOf<Vs>... has to be a subset of Ts...
    template<class... Vs> [[nodiscard]] static auto fromArgs(Vs&&... vs) -> Partial {
        auto bits = Bits{};
        (bits.setAt(index_of_map<StoredOf<Vs>, Map>), ...);
        auto hasValue = [&](size_t i) { return bits[i]; };
        auto factory_args = []<class T, class... Xs>(T*, void* ptr, Xs&&... xs) {
            (details::ConditionalPlacementNew<std::is_same_v<T, StoredOf<Xs>>>::apply((Xs &&) xs, ptr), ...);
        };
        auto factory = [&]<class T>(Type<T>*, void* ptr) { factory_args(nullptr_to<T>, ptr, (Vs &&) vs...); };
        return fromFactory(hasValue, factory);
    }

    [[nodiscard]] constexpr auto which() const -> Which { return Which{m_bits}; }

    // returns the value stored as type Ts...[I]
    // precondition: which()[I] == true
    template<size_t I> [[nodiscard]] auto at(Index<I>* = {}) const -> const TypeAtMap<I, Map>& {
        using T = TypeAtMap<I, Map>;
        return *std::launder(reinterpret_cast<const T*>(m_pointer + offsetAt(I)));
    }
    template<size_t I> [[nodiscard]] auto amendAt(Index<I>* = {}) -> TypeAtMap<I, Map>& {
        using T = TypeAtMap<I, Map>;
        return *std::launder(reinterpret_cast<T*>(m_pointer + offsetAt(I)));
    }

    // returns the value of type T
    // precondition: which().of<T>() == true
    template<class T> [[nodiscard]] auto of(Type<T>* = {}) const -> const T& {
        constexpr size_t I = index_of_map<T, Map>;
        return *std::launder(reinterpret_cast<const T*>(m_pointer + offsetAt(I)));
    }
    template<class T> [[nodiscard]] auto amendOf(Type<T>* = {}) -> T& {
        constexpr size_t I = index_of_map<T, Map>;
        return *std::launder(reinterpret_cast<T*>(m_pointer + offsetAt(I)));
    }

    // visit all initialized types stored in this Partial
    // functor: f(const auto&) - called with every actually stored type
    template<class F> void visitInitialized(F&& f) const {
        auto i = 0u;
        auto offset = size_t{};
        ((m_bits[i] ? (offset = alignOffset(alignof_ts[i], offset),
                       f(*std::launder(reinterpret_cast<const Ts*>(m_pointer + offset))),
                       offset += sizeof_ts[i],
                       ++i)
                    : ++i),
         ...);
    }
    template<class F> void amendVisitInitialized(F&& f) {
        auto i = 0u;
        auto offset = size_t{};
        ((m_bits[i] ? (offset = alignOffset(alignof_ts[i], offset),
                       f(*std::launder(reinterpret_cast<Ts*>(m_pointer + offset))),
                       offset += sizeof_ts[i],
                       ++i)
                    : ++i),
         ...);
    }

    // functor: f(Index<I>*, const auto&) - called with every actually stored type
    template<class F> void visitWithIndex(F&& f) const {
        [&f]<size_t... Is, class... Vs>(std::index_sequence<Is...>*, const Partial<Vs...>& p) {
            auto offset = size_t{};
            ((p.m_bits[Is] ? (offset = alignOffset(alignof_ts[Is], offset),
                              f(_index<Is>, *std::launder(reinterpret_cast<const Vs*>(p.m_pointer + offset))),
                              offset += sizeof_ts[Is])
                           : offset),
             ...);
        }
        (nullptr_to<std::make_index_sequence<sizeof...(Ts)>>, *this);
    }

private:
    void destructAll() {
        amendVisitInitialized([]<class T>(T& v) { v.~T(); });
        if constexpr (__STDCPP_DEFAULT_NEW_ALIGNMENT__ < max_align) {
            auto hasValue = [&](size_t i) { return m_bits[i]; };
            auto size = storageSize(hasValue);
            ::operator delete[](m_pointer, size, std::align_val_t{max_align});
        }
        else {
            delete[] m_pointer;
        }
    }

    template<class HasValue, class Factory> void constructAll(HasValue&& hasValue, Factory&& factory) {
        auto i = 0u;
        auto offset = size_t{};
        auto ptr = m_pointer;
        ((hasValue(i) ? (offset = alignOffset(alignof_ts[i], offset),
                         factory(type<Ts>, ptr + offset),
                         m_bits.setAt(i),
                         offset += sizeof_ts[i],
                         ++i)
                      : ++i),
         ...);
    }

    [[nodiscard]] auto offsetAt(size_t index) const {
        auto offset = size_t{};
        for (auto i = 0u; i < index; i++)
            if (m_bits[i]) offset = alignOffset(alignof_ts[i], offset) + sizeof_ts[i];
        return alignOffset(alignof_ts[index], offset);
    }

    template<class HasValue> [[nodiscard]] static constexpr auto storageSize(HasValue&& hasValue) {
        auto size = size_t{};
        for (auto i = 0u; i < count; i++)
            if (hasValue(i)) size = alignOffset(alignof_ts[i], size) + sizeof_ts[i];
        return size;
    }
};

} // namespace partial19
