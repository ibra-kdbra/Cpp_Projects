#pragma once
#include "AllocatedArrayUtils.h"
#include "MoveSliceOf.h"
#include "SliceOf.single.h"

#include <new> // launder
#include <stddef.h> // size_t

namespace array19 {

/// replacement for std::vector with different API
/// * focus on SliceOf
/// * mutable is explicit - use amend()
template<class T> struct DynamicArrayOf final {
    using Element = T;
    using Count = size_t;
    using Index = size_t;

    using Iterator = Element*;
    using ConstIterator = const Element*;
    using Slice = SliceOf<Element>;
    using ConstSlice = SliceOf<const Element>;
    using MoveSlice = MoveSliceOf<Element>;

private:
    using Utils = AllocatedArrayUtils<T>;

    T* m_pointer{};
    Count m_count{};
    Count m_capacity{};

public:
    DynamicArrayOf() = default;
    ~DynamicArrayOf() noexcept {
        if (m_pointer) {
            Utils::destruct(amend());
            Utils::deallocate(Slice{m_pointer, m_capacity});
        }
    }

    explicit DynamicArrayOf(ConstSlice slice) : m_pointer(Utils::allocate(slice.count())), m_capacity(slice.count()) {
        Utils::copyConstruct(m_pointer, slice);
        m_count = slice.count();
    }

    explicit DynamicArrayOf(MoveSlice slice) : m_pointer(Utils::allocate(slice.count())), m_capacity(slice.count()) {
        Utils::moveConstruct(m_pointer, slice);
        m_count = slice.count();
    }

    template<class... Ts> requires(sizeof...(Ts) > 0) && requires(Ts&&... args) { (T{(Ts &&) args}, ...); }
    explicit DynamicArrayOf(Ts&&... args) : m_pointer(Utils::allocate(sizeof...(Ts)))
                                          , m_capacity(sizeof...(Ts)) {
        (new (m_pointer + m_count++) T{(Ts &&) args}, ...);
    }

    DynamicArrayOf(const DynamicArrayOf& o)
            : m_pointer(Utils::allocate(o.m_count))
            , m_count(o.m_count)
            , m_capacity(o.m_count) {
        Utils::copyConstruct(m_pointer, o);
    }
    DynamicArrayOf& operator=(const DynamicArrayOf& o) {
        if (!m_pointer || m_capacity < o.m_count) {
            *this = DynamicArrayOf(o);
        }
        else {
            if (m_count > o.m_count) {
                Utils::copyAssign(amendBegin(), o);
                Utils::destruct(Slice{amendBegin() + o.m_count, m_count - o.m_count});
            }
            else {
                Utils::copyAssign(amendBegin(), ConstSlice{o.begin(), m_count});
                Utils::copyConstruct(storageEnd(), ConstSlice{o.begin() + m_count, o.m_count - m_count});
            }
            m_count = o.m_count;
        }
        return *this;
    }

    DynamicArrayOf(DynamicArrayOf&& o) noexcept //
            : m_pointer(o.m_pointer)
            , m_count(o.m_count)
            , m_capacity(o.m_capacity) {
        o.m_pointer = nullptr;
    }
    DynamicArrayOf& operator=(DynamicArrayOf&& o) noexcept {
        if (m_pointer) {
            Utils::destruct(amend());
            Utils::deallocate(Slice{m_pointer, m_capacity});
        }
        m_pointer = o.m_pointer;
        m_count = o.m_count;
        m_capacity = o.m_capacity;
        o.m_pointer = nullptr;
        return *this;
    }

    [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool { return m_count == 0; }
    [[nodiscard]] auto count() const -> Count { return m_count; }
    [[nodiscard]] auto totalCapacity() const -> Count { return m_capacity; }
    [[nodiscard]] auto unusedCapacity() const -> Count { return m_capacity - m_count; }

    [[nodiscard]] auto front() const -> const T& { return *begin(); }
    [[nodiscard]] auto back() const -> const T& { return *(begin() + m_count - 1); }

    [[nodiscard]] auto begin() const noexcept -> ConstIterator {
        return std::launder(reinterpret_cast<const T*>(m_pointer));
    }
    [[nodiscard]] auto end() const noexcept -> ConstIterator { return begin() + m_count; }
    [[nodiscard]] auto operator[](Index index) const noexcept -> const Element& {
        return *std::launder(reinterpret_cast<const T*>(m_pointer + index));
    }
    [[nodiscard]] operator ConstSlice() const noexcept { return ConstSlice{begin(), m_count}; }

    [[nodiscard]] auto amendBegin() & noexcept -> Iterator { return std::launder(reinterpret_cast<T*>(m_pointer)); }
    [[nodiscard]] auto amendEnd() & noexcept -> Iterator { return amendBegin() + m_count; }

    [[nodiscard]] auto amend() -> Slice { return Slice{amendBegin(), m_count}; }
    [[nodiscard]] auto move() -> MoveSlice { return MoveSlice{amendBegin(), m_count}; }

    void ensureCapacity(Count count) {
        if (totalCapacity() < count) growBy(count - totalCapacity());
    }
    void ensureUnusedCapacity(Count count) {
        if (unusedCapacity() < count) growBy(count - unusedCapacity());
    }

    void clear() {
        Utils::destruct(amend());
        m_count = 0;
    }

    /// append a single element constructed in place with the given arguments
    template<class... Ts> auto emplace_back(Ts&&... args) -> T& {
        ensureUnusedCapacity(1);
        auto ptr = new (storageEnd()) Element{(Ts &&) args...};
        m_count++;
        return *ptr;
    }

    /// appends possibly multiple elements at the end
    void append(ConstSlice elems) {
        ensureUnusedCapacity(elems.count());
        Utils::copyConstruct(storageEnd(), elems);
        m_count += elems.count();
    }

    /// appends possibly multiple elements at the end
    void appendMoved(MoveSlice elems) {
        ensureUnusedCapacity(elems.count());
        Utils::moveConstruct(storageEnd(), elems);
        m_count += elems.count();
    }

    /// appends count default constructed elements
    void appendCount(size_t count) {
        ensureUnusedCapacity(count);
        Utils::defaultConstruct(Slice{storageEnd(), count});
        m_count += count;
    }

    /// removes the last element
    void pop() {
        Utils::destruct(Slice{amendBegin() + m_count - 1, 1});
        m_count--;
    }

    /// swiss army knife of mutating an dynamic array
    /// replaces:
    /// * insert - splice(it, 0, sliceOfSingle{V})
    /// * erase - splice(it, N, {})
    /// extensions:
    /// * insert of elements at once
    /// * combined remove and insert
    void splice(Iterator it, Count removeCount, ConstSlice insertSlice) {
        auto offset = it - amendBegin();
        auto insertCount = insertSlice.count();
        auto remainCount = m_count - offset - removeCount;
        auto remainSlice = MoveSlice{it + removeCount, remainCount};
        // old: [ ..(offset)..,[it] ..(removeCount).., ..(remainCount)... ]
        // new: [ ..(offset)..,[it] ..(insertCount).., ..(remainCount)... ]

        auto newCount = (m_count - removeCount) + insertCount;
        if (m_capacity < newCount) { // not enough storage => arrange everything in new storage
            auto newStorage = grownStorage(insertCount - removeCount);
            Utils::moveConstruct(newStorage.begin(), MoveSlice{amendBegin(), static_cast<size_t>(offset)});
            Utils::copyConstruct(newStorage.begin() + offset, insertSlice);
            Utils::moveConstruct(newStorage.begin() + offset + insertCount, remainSlice);
            Utils::destruct(amend());
            Utils::deallocate(Slice{m_pointer, m_capacity});
            m_pointer = newStorage.begin();
            m_capacity = newStorage.count();
        }
        else if (m_count >= newCount) { // shrinking
            auto shrinkCount = m_count - newCount;
            Utils::copyAssign(it, insertSlice);
            Utils::moveAssignForward(m_pointer + offset + insertCount, remainSlice);
            Utils::destruct(Slice{amendEnd() - shrinkCount, shrinkCount});
        }
        else if (offset + insertCount <= m_count) { // parts of remainSlice is moved beyond end()
            auto growCount = newCount - m_count;
            Utils::moveConstruct(storageEnd(), remainSlice.slice(remainCount - growCount, growCount));
            Utils::moveAssignReverse(it + insertCount, remainSlice.slice(0, remainCount - growCount));
            Utils::copyAssign(it, insertSlice);
        }
        else { // remainSlice is moved beyond end()
            Utils::moveConstruct(m_pointer + offset + insertCount, remainSlice);
            auto assignElems = m_count - offset;
            Utils::copyAssign(it, insertSlice.slice(0, assignElems));
            Utils::copyConstruct(storageEnd(), insertSlice.slice(assignElems, insertCount - assignElems));
        }
        m_count = newCount;
    }

    /// Same as splice but moves inserted elements into the array
    void spliceMoved(Iterator it, Count removeCount, MoveSlice insertSlice) {
        auto offset = it - amendBegin();
        auto insertCount = insertSlice.count();
        auto remainCount = m_count - offset - removeCount;
        auto remainSlice = MoveSlice{it + removeCount, remainCount};
        // old: [ ..(offset)..,[it] ..(removeCount).., ..(remainCount)... ]
        // new: [ ..(offset)..,[it] ..(insertCount).., ..(remainCount)... ]

        auto newCount = (m_count - removeCount) + insertCount;
        if (m_capacity < newCount) { // not enough storage => arrange everything in new storage
            auto newStorage = grownStorage(insertCount - removeCount);
            Utils::moveConstruct(newStorage.begin(), MoveSlice{amendBegin(), static_cast<size_t>(offset)});
            Utils::moveConstruct(newStorage.begin() + offset, insertSlice);
            Utils::moveConstruct(newStorage.begin() + offset + insertCount, remainSlice);
            Utils::destruct(amend());
            Utils::deallocate(Slice{m_pointer, m_capacity});
            m_pointer = newStorage.begin();
            m_capacity = newStorage.count();
        }
        else if (m_count >= newCount) { // shrinking
            auto shrinkCount = m_count - newCount;
            Utils::moveAssign(it, insertSlice);
            Utils::moveAssignForward(m_pointer + offset + insertCount, remainSlice);
            Utils::destruct(Slice{amendEnd() - shrinkCount, shrinkCount});
        }
        else if (offset + insertCount <= m_count) { // parts of remainSlice is moved beyond end()
            auto growCount = newCount - m_count;
            Utils::moveConstruct(storageEnd(), remainSlice.slice(remainCount - growCount, growCount));
            Utils::moveAssignReverse(it + insertCount, remainSlice.slice(0, remainCount - growCount));
            Utils::moveAssign(it, insertSlice);
        }
        else { // remainSlice is moved beyond end()
            Utils::moveConstruct(m_pointer + offset + insertCount, remainSlice);
            auto assignElems = m_count - offset;
            Utils::moveAssign(it, insertSlice.slice(0, assignElems));
            Utils::moveConstruct(storageEnd(), insertSlice.slice(assignElems, insertCount - assignElems));
        }
        m_count = newCount;
    }

    /// Same as splice but moves inserted elements into the array
    void remove(Iterator it, Count removeCount) {
        auto offset = it - amendBegin();
        auto remainCount = m_count - offset - removeCount;
        auto remainSlice = MoveSlice{it + removeCount, remainCount};

        Utils::moveAssignForward(m_pointer + offset, remainSlice);
        Utils::destruct(Slice{amendEnd() - removeCount, removeCount});
        m_count = m_count - removeCount;
    }

private:
    [[nodiscard]] auto byteSize() const -> size_t { return m_count * sizeof(T); }
    [[nodiscard]] auto storageEnd() -> Iterator { return m_pointer + m_count; }

    [[nodiscard]] auto grownStorage(size_t growBy) const -> Slice {
        auto cur = m_capacity;
        auto res = (cur << 1) - (cur >> 1) + (cur >> 4); // * 1.563
        if (res < 5) res = 5;
        if (res < m_capacity + growBy) res = m_capacity + growBy;
        auto ptr = Utils::allocate(res);
        return Slice{ptr, res};
    }
    void growBy(size_t by) {
        auto newStorage = grownStorage(by);
        Utils::moveConstruct(newStorage.begin(), move());
        Utils::destruct(amend());
        Utils::deallocate(Slice{m_pointer, m_capacity});
        m_pointer = newStorage.begin();
        m_capacity = newStorage.count();
    }
};

/// simplified deduction guide
/// usage:
///     DynamicArrayOf{1,2,3}
template<class T, class... Ts> DynamicArrayOf(T&&, Ts&&...) -> DynamicArrayOf<T>;

/// deduce T from slices
template<class T> DynamicArrayOf(SliceOf<const T>) -> DynamicArrayOf<T>;
template<class T> DynamicArrayOf(MoveSliceOf<T>) -> DynamicArrayOf<T>;

/// deduce SliceOf from DynamicArray
/// usage:
///     auto a = DynamicArrayOf{1,2,3;
///     auto slice = SliceOf{a};
template<class T> SliceOf(const DynamicArrayOf<T>&) -> SliceOf<const T>;

} // namespace array19
