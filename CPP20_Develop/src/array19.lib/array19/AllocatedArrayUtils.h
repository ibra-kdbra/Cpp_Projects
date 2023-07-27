#pragma once
#include "MoveSliceOf.h"
#include "SliceOf.h"

#include <new> // ::operator new, ::operator delete
#include <stddef.h> // size_t
#include <string.h> // memcpy
#include <type_traits>
#include <utility>

namespace array19 {

/// Utility functions that implement the meat of allocated arrays
///
/// note:
/// * This replaces the algorithmns form std <memory> header.
/// * It works with Slices instead of iterators to simplify the interface
template<class T> struct AllocatedArrayUtils {
    static_assert(std::is_nothrow_move_constructible_v<T>, "Please ensure move constructior is marked noexcept!");
    static_assert(std::is_nothrow_move_assignable_v<T>, "Please ensure move assignment is marked noexcept!");
    static_assert(std::is_nothrow_destructible_v<T>, "Please ensure destructor is marked noexcept!");

    using Slice = SliceOf<T>;
    using ConstSlice = SliceOf<const T>;
    using MoveSlice = MoveSliceOf<T>;

    /// returns uninitialized storage for \param count elements with proper alignment
    /// note:
    /// * use deallocate to free the storage!
    /// * exceptions from new will terminate - without memory no recovery is possible!
    [[nodiscard]] static auto allocate(size_t count) noexcept -> T* {
        if (0 == count) return nullptr;
        if constexpr (__STDCPP_DEFAULT_NEW_ALIGNMENT__ < alignof(T)) {
            return reinterpret_cast<T*>(::operator new[](count * sizeof(T), std::align_val_t{alignof(T)}));
        }
        else {
            return reinterpret_cast<T*>(::operator new[](count * sizeof(T)));
        }
    }

    /// frees the storage pointed to by the slice
    /// note:
    /// * slice.begin() has to be the pointer returned by allocate(size.count())
    /// * exceptions form delete will terminate - we assume memory is corrupted!
    static void deallocate(Slice slice) noexcept {
#if __cpp_sized_deallocation
        if constexpr (__STDCPP_DEFAULT_NEW_ALIGNMENT__ < alignof(T)) {
            ::operator delete[](slice.begin(), slice.count() * sizeof(T), std::align_val_t{alignof(T)});
        }
        else {
            ::operator delete[](slice.begin(), slice.count() * sizeof(T));
        }
#else
        if constexpr (__STDCPP_DEFAULT_NEW_ALIGNMENT__ < alignof(T)) {
            ::operator delete[](slice.begin(), std::align_val_t{alignof(T)});
        }
        else {
            ::operator delete[](slice.begin());
        }
#endif
    }

    /// default construct every element of \param slice if necessary
    static void defaultConstruct(Slice slice) noexcept(std::is_nothrow_default_constructible_v<T>) {
        if constexpr (!std::is_trivially_default_constructible_v<T>)
            for (auto& e : slice) new (&e) T();
    }

    /// calls destructor for every element of \param slice if necessary
    /// note:
    /// * we assume destructors are always noexcept
    static void destruct(Slice slice) noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>)
            for (auto& elem : slice) elem.~T();
    }

    /// constructs a copy of \param fromSlice at \param toPointer
    /// note:
    /// * toPointer has to point to least fromSlice.count() elements
    /// * assumes toPointer and fromSlice do not overlap
    static void copyConstruct(T* toPointer, ConstSlice fromSlice) noexcept(std::is_nothrow_copy_constructible_v<T>) {
        if constexpr (std::is_trivially_copy_constructible_v<T>) {
            if (!fromSlice.isEmpty()) {
                memcpy(toPointer, fromSlice.begin(), fromSlice.count() * sizeof(T));
            }
        }
        else {
            for (auto& from : fromSlice) new (toPointer++) T(from);
        }
    }

    /// move constructs elements of \param fromSlice into \param to Pointer
    /// note:
    /// * toPointer has to point to least fromSlice.count() elements
    /// * assumes toPointer and fromSlice do not overlap
    /// * we assume move is always noexcept!
    static void moveConstruct(T* toPointer, MoveSlice fromSlice) noexcept {
        if constexpr (std::is_trivially_move_constructible_v<T>) {
            if (!fromSlice.isEmpty()) {
                memcpy(toPointer, fromSlice.begin(), fromSlice.count() * sizeof(T));
            }
        }
        else {
            for (auto& from : fromSlice) new (toPointer++) T(std::move(from));
        }
    }

    /// assigns elements of \param fromSlice into \param toPointer
    /// note:
    /// * toPointer has to point to least fromSlice.count() initialized elements
    /// * assumes toPointer and fromSlice do not overlap
    static void copyAssign(T* toPointer, ConstSlice fromSlice) noexcept(std::is_nothrow_copy_assignable_v<T>) {
        if constexpr (std::is_trivially_copy_assignable_v<T>) {
            if (!fromSlice.isEmpty()) {
                memcpy(toPointer, fromSlice.begin(), fromSlice.count() * sizeof(T));
            }
        }
        else {
            for (const auto& from : fromSlice) *toPointer++ = from;
        }
    }

    /// move assigns elements of \param fromSlice into \param to Pointer
    /// note:
    /// * toPointer has to point to least fromSlice.count() elements
    /// * assumes toPointer and fromSlice do not overlap
    /// * we assume move is always noexcept!
    static void moveAssign(T* toPointer, MoveSlice fromSlice) noexcept {
        if constexpr (std::is_trivially_move_assignable_v<T>) {
            if (!fromSlice.isEmpty()) {
                memcpy(toPointer, fromSlice.begin(), fromSlice.count() * sizeof(T));
            }
        }
        else {
            for (auto& from : fromSlice) *toPointer++ = std::move(from);
        }
    }

    /// move assigns elements of \param fromSlice into \param to Pointer
    /// note:
    /// * toPointer has to point to least fromSlice.count() elements
    /// * toPointer has to be before fromSlice.begin() if ranges overlap
    /// * we assume move is always noexcept!
    ///
    /// example:
    ///         [ e e e e e e e ]
    /// toPointer ^   ^ fromSlice.begin()
    static void moveAssignForward(T* toPointer, MoveSlice fromSlice) noexcept {
        if constexpr (std::is_trivially_move_constructible_v<T>) {
            if (!fromSlice.isEmpty()) {
                memmove(toPointer, fromSlice.begin(), fromSlice.count() * sizeof(T));
            }
        }
        else {
            for (auto& from : fromSlice) *toPointer++ = std::move(from);
        }
    }

    /// move assigns elements of \param fromSlice into \param to Pointer
    /// note:
    /// * toPointer has to point to least fromSlice.count() elements
    /// * toPointer has to be behind fromSlice.begin() if ranges overlap
    /// * we assume move is always noexcept!
    ///
    /// example:
    ///                 [ e e e e e e e ]
    /// fromSlice.begin() ^     ^ toPointer
    static void moveAssignReverse(T* toPointer, MoveSlice fromSlice) noexcept {
        if constexpr (std::is_trivially_move_assignable_v<T>) {
            if (!fromSlice.isEmpty()) {
                memmove(toPointer, fromSlice.begin(), fromSlice.count() * sizeof(T));
            }
        }
        else {
            auto rTo = toPointer + fromSlice.count();
            auto rFrom = fromSlice.end();
            auto rFromEnd = fromSlice.begin();
            while (rFrom != rFromEnd) *(--rTo) = std::move(*(--rFrom));
        }
    }
};

} // namespace array19
