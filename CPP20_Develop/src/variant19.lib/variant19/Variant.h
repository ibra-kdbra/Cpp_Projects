#pragma once
#include "meta19/Index.h"
#include "meta19/RemoveReference.h"
#include "meta19/Type.h"
#include "meta19/TypeAt.h"
#include "meta19/TypePack.Front.h"
#include "meta19/Unreachable.h"
#include "meta19/index_pack.h"

#include <limits>
#include <memory>
#include <new>
#include <stddef.h> // size_t
#include <utility> // std::launder

namespace variant19 {

using meta19::Index;
using meta19::index_of_map;
using meta19::index_pack;
using meta19::IndexPack;
using meta19::IndexTypeMap;
using meta19::PackFront;
using meta19::StoredOf;
using meta19::Type;
using meta19::TypeAtMap;

template<class... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

namespace details {

template<size_t I, size_t... Is> constexpr auto constexprMaxOf() {
    auto r = I;
    (void)(((Is > r ? r = Is : 0), ...));
    return r;
}

template<size_t N> constexpr auto selectVariantWhichValue() {
    if constexpr (N <= std::numeric_limits<uint8_t>::max()) {
        return uint8_t{};
    }
    else if constexpr (N <= std::numeric_limits<uint16_t>::max()) {
        return uint16_t{};
    }
    else if constexpr (N <= std::numeric_limits<uint32_t>::max()) {
        return uint32_t{};
    }
    else if constexpr (N <= std::numeric_limits<uint64_t>::max()) {
        return uint64_t{};
    }
    // fail …
}

template<class T, class... Ts, class V, class F, size_t I, size_t... Is>
constexpr auto visitRecursive(V&& v, F&& f, IndexPack<I, Is...>*) -> decltype(auto) {
    if (I == v.which) {
        return f(*v.template asPtr<T>());
    }
    if constexpr (0 != sizeof...(Ts) && 0 != sizeof...(Is)) {
        return visitRecursive<Ts...>(std::forward<V>(v), std::forward<F>(f), index_pack<Is...>);
    }
    else {
        META19_UNREACHABLE();
    }
}

template<class T, class... Ts, class V, class F, size_t I, size_t... Is>
constexpr auto amendVisitRecursive(V&& v, F&& f, IndexPack<I, Is...>*) -> decltype(auto) {
    if (I == v.which) {
        return f(*v.template amendAsPtr<T>());
    }
    if constexpr (0 != sizeof...(Ts) && 0 != sizeof...(Is)) {
        return amendVisitRecursive<Ts...>(std::forward<V>(v), std::forward<F>(f), index_pack<Is...>);
    }
    else {
        META19_UNREACHABLE();
    }
}

} // namespace details

template<class... Ts> struct VariantWhich {
    using Value = decltype(details::selectVariantWhichValue<sizeof...(Ts)>());
    using Map = IndexTypeMap<Ts...>;

    constexpr VariantWhich() = default;
    explicit constexpr VariantWhich(Value v) : m(v) {}

    constexpr operator Value() const { return m; }

    constexpr bool operator==(const VariantWhich& o) const = default;

    template<class T> static constexpr auto of(Type<T>* = {}) -> VariantWhich {
        return VariantWhich{static_cast<Value>(index_of_map<T, Map>)};
    }

    template<class T> constexpr bool operator==(Type<T>*) const { return index_of_map<T, Map> == m; }
    template<class T> constexpr bool operator!=(Type<T>*) const { return index_of_map<T, Map> != m; }

private:
    Value m{sizeof...(Ts)};
};

template<class... Ts> struct Variant {
    using Which = VariantWhich<Ts...>;
    using Map = typename Which::Map;
    enum { npos = sizeof...(Ts) }; // invalid state after exception - only destruction checks!

private:
    template<class> struct IndexedVariant;

    template<size_t... Is> struct IndexedVariant<std::index_sequence<Is...>> {
        enum : size_t {
            storage_size = details::constexprMaxOf<sizeof(Ts)...>(),
        };
        using First = PackFront<Ts...>;
        using WhichValue = typename Which::Value;

        WhichValue which{npos};
        alignas(Ts...) std::byte storage[storage_size];

        constexpr IndexedVariant() {
            constructAs<First>();
            which = 0; // only initialize once constuction was successful
        }
        ~IndexedVariant() noexcept {
            if (which == npos) return;
            destruct();
        }

        constexpr IndexedVariant(const IndexedVariant& o) {
            (((Is == o.which ? (constructAs<Ts>(*o.asPtr<Ts>()), 0) : 0), ...));
            which = o.which;
        }
        constexpr auto operator=(const IndexedVariant& o) -> IndexedVariant& {
            if (which == o.which) {
                (((Is == o.which ? (*amendAsPtr<Ts>() = *o.asPtr<Ts>(), 0) : 0), ...));
            }
            else {
                destruct();
                which = npos;
                (((Is == o.which ? (constructAs<Ts>(*o.asPtr<Ts>()), 0) : 0), ...));
                which = o.which;
            }
            return *this;
        }

        constexpr IndexedVariant(IndexedVariant&& o) noexcept {
            (((Is == o.which ? (constructAs<Ts>(std::move(*o.amendAsPtr<Ts>())), 0) : 0), ...));
            which = o.which; // o.which is needed for destruction!
        }
        constexpr auto operator=(IndexedVariant&& o) noexcept -> IndexedVariant& {
            if (which == o.which) {
                (((Is == o.which ? (*amendAsPtr<Ts>() = std::move(*o.amendAsPtr<Ts>()), 0) : 0), ...));
            }
            else {
                destruct();
                which = npos;
                (((Is == o.which ? (constructAs<Ts>(std::move(*o.amendAsPtr<Ts>())), 0) : 0), ...));
                which = o.which; // o.which is needed for destruction!
            }
            return *this;
        }

        template<class T> constexpr auto asPtr() const -> const T* {
            return std::launder(reinterpret_cast<const T*>(&storage));
        }
        template<class T> constexpr auto amendAsPtr() -> T* { return std::launder(reinterpret_cast<T*>(&storage)); }

        template<class T, class... Args> constexpr auto constructAs(Args&&... args) {
            new (&storage) T(std::forward<Args>(args)...);
        }
        template<class... Args> constexpr auto constructWhich(WhichValue w, Args&&... args) {
            (((Is == w ? (constructAs<Ts>(std::forward<Args>(args)...), 0) : 0), ...));
        }

        constexpr void destruct() { (void)((Is == which ? (std::destroy_at(amendAsPtr<Ts>()), true) : false) || ...); }

        template<class F> constexpr auto visitImpl(F&& f) const -> decltype(auto) {
            if constexpr (std::is_same_v<void, decltype(f(first()))>) {
                (void)(((Is == which ? (f(*asPtr<Ts>()), true) : false) || ...));
            }
            else {
                return details::visitRecursive<Ts...>(*this, std::forward<F>(f), index_pack<Is...>);
            }
        }

        template<class F> constexpr auto amendVisitImpl(F&& f) -> decltype(auto) {
            if constexpr (std::is_same_v<void, decltype(f(first()))>) {
                (void)(((Is == which ? (f(*amendAsPtr<Ts>()), true) : false) || ...));
            }
            else {
                return details::amendVisitRecursive<Ts...>(*this, std::forward<F>(f), index_pack<Is...>);
            }
        }

        auto first() -> First&;
        auto first() const -> const First&;
    };
    using Indexed = IndexedVariant<std::make_index_sequence<sizeof...(Ts)>>;

private:
    Indexed indexed;

public:
    constexpr Variant() = default;

    /// construct from Ts move/copy
    template<class T> requires((std::is_same_v<StoredOf<T>, Ts> || ...)) Variant(T&& t) {
        indexed.template constructAs<StoredOf<T>>(std::forward<T>(t));
        indexed.which = index_of_map<StoredOf<T>, Map>;
    }

    /// construct of type inside variant
    template<class T, class... Args> Variant(Type<T>*, Args&&... args) {
        static_assert((std::is_same_v<T, Ts> || ...), "type not part of variant");
        indexed.template constructAs<T>(std::forward<Args>(args)...);
        indexed.which = index_of_map<T, Map>;
    }

    /// construct indexed type inside variant
    template<size_t I, class... Args> Variant(Index<I>*, Args&&... args) {
        static_assert(I < npos, "index not part of variant");
        using T = TypeAtMap<I, Map>;
        indexed.template constructAs<T>(std::forward<Args>(args)...);
        indexed.which = I;
    }

    /// Which describing the currently constructed type
    constexpr auto which() const -> Which { return Which{indexed.which}; }

    /// Extract specified type
    /// precondition: which() == type<T> otherwise undefined behavior
    template<class T> constexpr auto as(Type<T>* = {}) const -> const T& {
        static_assert((std::is_same_v<T, Ts> || ...), "type not part of variant");
        return *indexed.template asPtr<std::remove_reference_t<T>>();
    }
    template<class T> constexpr auto amendAs(Type<T>* = {}) -> T& {
        static_assert((std::is_same_v<T, Ts> || ...), "type not part of variant");
        return *indexed.template amendAsPtr<T>();
    }

    /// Invoke lambda with the currently instanciated type
    template<class F> constexpr auto visit(F&& f) const -> decltype(auto) {
        return indexed.visitImpl(std::forward<F>(f));
    }
    template<class F> constexpr auto amendVisit(F&& f) -> decltype(auto) {
        return indexed.amendVisitImpl(std::forward<F>(f));
    }

    /// Convinience function to overload all given lambdas
    template<class... Fs> requires(sizeof...(Fs) > 0)
    constexpr auto visitOverloaded(Fs&&... fs) const -> decltype(auto) {
        return indexed.visitImpl(Overloaded{(Fs &&) fs...});
    }

    template<class... Fs> requires(sizeof...(Fs) > 0) constexpr auto amendOverloaded(Fs&&... fs) -> decltype(auto) {
        return indexed.amendVisitImpl(Overloaded{(Fs &&) fs...});
    }

    /// inplace change of indexed type inside variant
    template<size_t I, class... Args> void emplaceAt(Index<I>*, Args&&... args) {
        static_assert(I < npos, "index not part of variant");
        indexed.destruct();
        indexed.which = npos;
        using T = TypeAtMap<I, Map>;
        indexed.template constructAs<T>(std::forward<Args>(args)...);
        indexed.which = I;
    }

    /// inplace change of type inside variant
    template<class T, class... Args> void emplaceAs(Type<T>*, Args&&... args) {
        static_assert((std::is_same_v<T, Ts> || ...), "type not part of variant");
        indexed.destruct();
        indexed.which = npos;
        indexed.template constructAs<T>(std::forward<Args>(args)...);
        indexed.which = index_of_map<T, Map>;
    }
};

} // namespace variant19
