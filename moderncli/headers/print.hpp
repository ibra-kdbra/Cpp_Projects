// Copyright (C) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#ifndef PRINT_HPP_
#define PRINT_HPP_

#include <iostream>
#include <string_view>

#if __has_include(<format>) && __cplusplus >= 202002L
#include <format>
namespace tycho {
template<class... Args>
inline auto print_format(std::string_view fmt, const Args&... args) {
    return std::format(fmt, args...);
}}
#else
#include <fmt/format.h>
namespace tycho {
template<class... Args>
auto print_format(std::string_view fmt, const Args&... args) {
    return fmt::format(fmt, args...);
}}
#endif

namespace tycho {
template<class... Args>
void print(std::string_view fmt, const Args&... args) {
    std::cout << print_format(fmt, args...);
}

template<class... Args>
void println(std::string_view fmt = "", const Args&... args) {
    std::cout << print_format(fmt, args...) << std::endl;
}

template<class... Args>
void print(std::FILE *stream, std::string_view fmt, const Args&... args) {
    fputs(print_format(fmt, args...).c_str(), stream);
}

template<class... Args>
void println(std::FILE *stream, std::string_view fmt = "", const Args&... args) {
    std::printf(stream, "%s\n", print_format(fmt, args...).c_str()); // FlawFinder: ignore
}

template<class... Args>
void die(int code, std::string_view fmt, const Args&... args) {
    std::cerr << print_format(fmt, args...);
    ::exit(code);
}

template<class... Args>
void print(std::ostream& out, std::string_view fmt, const Args&... args) {
    out << print_format(fmt, args...);
}

template<class... Args>
void println(std::ostream& out, std::string_view fmt = "", const Args&... args) {
    out << print_format(fmt, args...) << std::endl;
}

template<class Out, class... Args>
void print_to(Out& out, std::string_view fmt, const Args&... args) {
    out << print_format(fmt, args...);
}

template<class Out, class... Args>
void println_to(Out& out, std::string_view fmt = "", const Args&... args) {
    out << print_format(fmt, args...) << std::endl;
}
} // end namespace

/*!
 * Generic print and control manipulators.  This makes it easy to write
 * output and fifo control messages using C++ stream operators.
 * \file print.hpp
 */
#endif
