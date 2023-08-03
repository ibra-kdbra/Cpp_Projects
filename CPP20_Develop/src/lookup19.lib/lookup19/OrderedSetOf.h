#pragma once
#include "OrderedSliceOf.h"
#include "array19/AllocatedArrayUtils.h"

#include <stddef.h> // size_t

namespace lookup19 {

using array19::AllocatedArrayUtils;

struct DefaultLess {
    template<class A, class B> constexpr auto operator()(const A& a, const B& b) const { return a < b; }
};

/// Useful for trivial sortable types
template<class T, class Less = DefaultLess> struct OrderedSetOf {
    using Element = T;
    using Count = size_t;
    using Index = size_t;

    using Iterator = Element*;
    using ConstIterator = const Element*;
    using Slice = OrderedSliceOf<const T, Less>;
    using AmendableSlice = SliceOf<T>;
    using UnorderedSlice = SliceOf<const T>;

    static_assert(std::is_trivial_v<T>, "Only works for trivial types!");

private:
    using Utils = AllocatedArrayUtils<T>;

    T* m_pointer{};
    Count m_count{};
    Count m_capacity{};

public:
    OrderedSetOf() = default;
    ~OrderedSetOf() noexcept {
        if (m_pointer) Utils::deallocate(SliceOf{m_pointer, m_capacity});
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
    [[nodiscard]] operator Slice() const noexcept { return Slice{begin(), m_count}; }

    void ensureCapacity(Count count) {
        if (totalCapacity() < count) growBy(count - totalCapacity());
    }
    void ensureUnusedCapacity(Count count) {
        if (unusedCapacity() < count) growBy(count);
    }

    /// inserts a single value to the set if it is not yet present
    /// note:
    /// * if you want to insert muliple values use merge
    bool insert(T v) {
        auto it = const_cast<Iterator>(static_cast<Slice>(*this).lowerBound(v));
        if (it != end() && *it == v) {
            return false;
        }
        if (m_count == m_capacity) {
            auto newStorage = grownStorage(1);
            auto nPtr = newStorage.begin();
            auto fCount = static_cast<size_t>(it - m_pointer);
            if (0 != fCount) {
                memcpy(nPtr, m_pointer, fCount);
                nPtr += fCount;
            }
            *nPtr++ = v;
            if (fCount != m_count) {
                memcpy(nPtr, m_pointer + fCount, m_count - fCount);
            }
            Utils::deallocate(SliceOf{m_pointer, m_capacity});
            m_pointer = newStorage.begin();
            m_capacity = newStorage.count();
            m_count++;
            return true;
        }
        if (it != end()) {
            memmove(it + 1, it, end() - it);
        }
        *it = v;
        m_count++;
        return true;
    }

    /// removes a single element from the set
    /// preconditions:
    /// * cIt has to point between begin() and before end()
    void remove(ConstIterator cIt) {
        auto it = const_cast<Iterator>(cIt);
        if (it + 1 != end()) {
            memmove(it, it + 1, end() - it - 1);
        }
        m_count--;
    }

    /// adds multiple elements to the set if they are not already present
    /// note:
    /// * assumes that enough capacity for inserting all elements is required
    void merge(Slice elems) {
        if (elems.isEmpty()) return;
        auto less = Less{};
        auto nCount = elems.count();
        if (unusedCapacity() < nCount) { // merge into new storage
            auto newStorage = grownStorage(nCount);
            auto dPtr = newStorage.begin();
            auto nPtr = elems.begin();
            auto nEnd = elems.end();
            if (isEmpty()) {
                memcpy(dPtr, nPtr, nCount);
                Utils::deallocate(SliceOf{m_pointer, m_capacity});
                m_pointer = newStorage.begin();
                m_capacity = newStorage.count();
                m_count = nCount;
                return;
            }
            auto n = *nPtr;
            auto oPtr = begin();
            auto o = *oPtr;
            auto oEnd = end();
            while (true) {
                if (less(n, o)) {
                    *dPtr++ = n;
                    nPtr++;
                    if (nPtr == nEnd) {
                        memcpy(dPtr, oPtr, oEnd - oPtr);
                        break;
                    }
                    n = *nPtr;
                }
                else {
                    *dPtr++ = o;
                    oPtr++;
                    if (!less(o, n)) {
                        nPtr++;
                        if (nPtr == nEnd) {
                            if (oPtr != oEnd) memcpy(dPtr, oPtr, oEnd - oPtr);
                            break;
                        }
                        n = *nPtr;
                    }
                    if (oPtr == oEnd) {
                        memcpy(dPtr, nPtr, nEnd - nPtr);
                        break;
                    }
                    o = *oPtr;
                }
            }
            Utils::deallocate(SliceOf{m_pointer, m_capacity});
            m_pointer = newStorage.begin();
            m_capacity = newStorage.count();
            m_count += nCount;
            return;
        }
        if (isEmpty()) {
            memcpy(m_pointer, elems.begin(), nCount);
            m_count = nCount;
            return;
        }
        auto oBegin = m_pointer;
        auto oIt = oBegin + m_count - 1;
        auto o = *oIt;
        auto nBegin = elems.begin();
        auto nIt = nBegin + nCount - 1;
        auto n = *nIt;
        auto dIt = oBegin + m_count + nCount - 1;

        while (true) {
            if (less(o, n)) {
                *dIt-- = n;
                nIt--;
                if (nIt == nBegin) {
                    // remaining old values are in place
                    break;
                }
            }
            else {
                *dIt-- = o;
                oIt--;
                if (!less(n, o)) {
                    nIt--;
                    if (nIt == nBegin) {
                        // remaining old values are in place
                        break;
                    }
                    n = *nIt;
                }
                if (oIt == oBegin) {
                    memcpy(oBegin, nBegin, nIt - nBegin);
                    break;
                }
                o = *oIt;
            }
        }
        m_count += nCount;
    }

private:
    [[nodiscard]] auto grownStorage(int growBy) const -> AmendableSlice {
        auto cur = m_capacity;
        auto res = (cur << 1) - (cur >> 1) + (cur >> 4); // * 1.563
        if (res < 5) res = 5;
        if (res < m_capacity + growBy) res = m_capacity + growBy;
        auto ptr = Utils::allocate(res);
        return AmendableSlice{ptr, res};
    }
    void growBy(int by) {
        auto newStorage = grownStorage(by);
        memcpy(newStorage.begin(), m_pointer, m_count);
        Utils::deallocate(SliceOf{m_pointer, m_capacity});
        m_pointer = newStorage.begin();
        m_capacity = newStorage.count();
    }
};

/// deduce OrderedSliceOf from OrderedSetOf
/// usage:
///     auto a = OrderedSetOf{1,2,3;
///     auto slice = OrderedSliceOf{a};
template<class T, class L> OrderedSliceOf(const OrderedSetOf<T, L>&) -> OrderedSliceOf<const T, L>;

} // namespace lookup19
