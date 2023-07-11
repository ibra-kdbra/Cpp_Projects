// Copyright (C) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#ifndef FILESYSTEM_HPP_
#define FILESYSTEM_HPP_

#if __has_include(<filesystem>)
#include <filesystem>
namespace fsys = std::filesystem; // NOLINT
#else
#include <experimental/filesystem>
namespace fsys = std::experimental::filesystem; // NOLINT
#endif

/*!
 * Filesystem support.
 * \file filesystem.hpp
 */
#endif
