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

namespace cybel {

namespace Util {
  template <typename... Args>
  std::string build_str(const Args&... args);

  std::string strip_str(const std::string& str);
  tiny_utf8::string ellips_str(const tiny_utf8::string& str,std::size_t max_len);
  tiny_utf8::string pad_str(const tiny_utf8::string& str,std::size_t len);
  int comparei_str(const tiny_utf8::string& str1,const tiny_utf8::string& str2);

  template <typename... Args>
  std::size_t build_hash(const Args&... args);

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
  std::stringstream ss{};

  (ss << ... << args);
  return ss.str();
}

template <typename... Args>
std::size_t Util::build_hash(const Args&... args) {
  std::size_t seed = 0;

  ((seed ^= std::hash<Args>{}(args) + 0x9e3779b9 + (seed << 6) + (seed >> 2)),... );
  return seed;
}

} // Namespace.
#endif
