/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UTIL_UTIL_H_
#define CYBEL_UTIL_UTIL_H_

#include "cybel/common.h"

#include <cctype>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace cybel {

namespace Util {
  template <typename... Args>
  std::string build_str(const Args&... args);

  std::string strip_str(const std::string& str);
  StrUtf8 ellips_str(const StrUtf8& str,std::size_t max_len);
  StrUtf8 pad_str(const StrUtf8& str,std::size_t len);
  std::string wrap_str(const std::string& str,std::size_t max_len);
  int comparei_str(const StrUtf8& str1,const StrUtf8& str2);

  template <typename... Args>
  std::size_t build_hash(const Args&... args);

  template <typename T>
  std::vector<T> unique(const std::vector<T>& vec);

  std::string get_sdl_error();
  std::string get_sdl_img_error();
  std::string get_sdl_mix_error();
  std::string get_gl_error(GLenum error);
  std::string get_glew_error(GLenum error);
}

// See Example at bottom:
// - https://en.cppreference.com/w/cpp/language/fold
template <typename... Args>
std::string Util::build_str(const Args&... args) {
  std::ostringstream ss{};

  (ss << ... << args);
  return ss.str();
}

template <typename... Args>
std::size_t Util::build_hash(const Args&... args) {
  std::size_t seed = 0;

  ((seed ^= std::hash<Args>{}(args) + 0x9e3779b9 + (seed << 6) + (seed >> 2)),... );
  return seed;
}

template <typename T>
std::vector<T> Util::unique(const std::vector<T>& vec) {
  std::unordered_set<T> seen{};
  std::vector<T> unique{};

  for(const auto& e: vec) {
    bool is_unique = seen.insert(e).second; // Was inserted?

    if(is_unique) { unique.push_back(e); }
  }

  return unique;
}

} // Namespace.
#endif
