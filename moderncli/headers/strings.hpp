// Copyright (C) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#ifndef STRINGS_HPP_
#define STRINGS_HPP_

#include <string>
#include <string_view>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <set>
#include <sstream>

namespace tycho {
template<typename S = std::string>
auto begins_with(const S& s, const std::string_view &b) -> bool {
    return s.find(b) == 0;
}

template<typename S = std::string>
auto ends_with(const S& s, const std::string_view &e) -> bool {
    if(s.size() < e.size())
        return false;
    auto pos = s.rfind(e);
    if(pos == S::npos)
        return false;
    return s.substr(pos) == e;
}

template<typename S = std::string>
auto upper_case(const S& s) -> S {
    S out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::toupper);
    return out;
}

template<typename S = std::string>
auto lower_case(const S& s) -> S {
    S out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

template<typename S = std::string>
auto trim(const S& str) -> S {
    auto last = str.find_last_not_of(" \t\f\v\n\r");
    if(last == S::npos)
        return "";
    return str.substr(0, ++last);
}

template<typename S = std::string>
auto strip(const S& str) -> S {
    const size_t first = str.find_first_not_of(" \t\f\v\n\r");
    const size_t last = str.find_last_not_of(" \t\f\v\n\r");
    if(last == S::npos)
        return "";
    return str.substr(first, (last-first+1));
}

template<typename S = std::string>
auto unquote(const S& str, std::string_view pairs = R"(""''{})") -> S {
    if(str.empty())
        return "";
    auto pos = pairs.find_first_of(str[0]);
    if(pos == S::npos || (pos & 0x01))
        return str;
    auto len = str.length();
    if(--len < 1)
        return str;
    if(str[len] == pairs[++pos])
        return str.substr(1, --len);
    return str;
}

template<typename S = std::string>
auto join(const std::vector<S>& list, const std::string_view& delim = ",") -> S {
    S separator, result;
    for(const auto& str : list) {
        result = result + separator + str;
        separator = delim;
    }
    return result;
}

template<typename S = std::string>
auto split(const S& str, std::string_view delim = " ", unsigned max = 0)
{
    std::vector<S> result;
    std::size_t current{}, prev{};
    unsigned count = 0;
    current = str.find_first_of(delim);
    while((!max || ++count < max) && current != S::npos) {
        result.emplace_back(str.substr(prev, current - prev));
        prev = current + 1;
        current = str.find_first_of(delim, prev);
    }
    result.emplace_back(str.substr(prev));
    return result;
}

template<typename T>
auto join(const std::set<T>& list, const std::string_view& delim = ",") {
    std::string sep;
    std::stringstream buf;
    for(auto const& value : list) {
        buf << sep << value;
        sep = delim;
    }
    return buf.str();
}

template<typename S=std::string>
auto tokenize(const S& str, std::string_view delim = " ", std::string_view quotes = R"(""''{})") {
    std::vector<S> result;
    auto current = str.find_first_of(delim);
    auto prev = str.find_first_not_of(delim);
    if(prev == S::npos)
        prev = 0;

    while(current != S::npos) {
        auto lead = quotes.find_first_of(str[prev]);
        if(lead != S::npos) {
            auto tail = str.find_first_of(quotes[lead + 1], prev + 1);
            if(tail != S::npos) {
                current = tail;
                result.emplace_back(str.substr(prev, current - prev + 1));
                goto finish; // NOLINT
            }
        }
        result.emplace_back(str.substr(prev, current - prev));
finish:
        prev = str.find_first_not_of(delim, ++current);
        if(prev == S::npos)
            prev = current;
        current = str.find_first_of(delim, prev);
    }
    if(prev < str.length())
        result.emplace_back(str.substr(prev));
    return result;
}

// convenience function for string conversions if not explicit for template

inline auto upper_case(const char *s) {
    return upper_case(std::string(s));
}

inline auto lower_case(const char *s) {
    return lower_case(std::string(s));
}

inline auto strip(const char *s) {
    return strip(std::string(s));
}

inline auto unquote(const char *s, const char *p = R"(""''{})") {
    return unquote(std::string(s), std::string(p));
}

inline auto begins_with(const char *s, const char *b) {
    return begins_with<std::string_view>(s, b);
}

inline auto ends_with(const char *s, const char *e) {
    return ends_with<std::string_view>(s, e);
}

inline auto eq(const char *p1, const char *p2) -> bool {
    if(!p1 && !p2)
        return true;

    if(!p1 || !p2)
        return false;

    return !strcmp(p1, p2);
}

inline auto eq(const char *p1, const char *p2, size_t len) -> bool {
    if(!p1 && !p2)
        return true;

    if(!p1 || !p2)
        return false;

    return !strncmp(p1, p2, len);
}
} // end namespace

/*!
 * Common string related functions and extensions.
 * \file string.hpp
 */
#endif
