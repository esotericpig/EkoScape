/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_UTIL_H_
#define EKOSCAPE_CORE_UTIL_H_

#include "common.h"

#include <cctype>
#include <sstream>

namespace ekoscape {

namespace Util {
  template <typename... Args>
  std::string build_string(const Args&... args);

  std::string strip(const std::string& str);

  template <typename... Args>
  std::size_t build_hash(const Args&... args);

  std::string get_sdl_error();
  std::string get_sdl_img_error();
  std::string get_sdl_mix_error();
  std::string get_gl_error(GLenum error);
  std::string get_glew_error(GLenum error);
}

/**
 * See Example at bottom:
 * - https://en.cppreference.com/w/cpp/language/fold
 */
template <typename... Args>
std::string Util::build_string(const Args&... args) {
  std::stringstream ss{};

  (ss << ... << args);

  return ss.str();
}

template <typename... Args>
std::size_t Util::build_hash(const Args&... args) {
  std::size_t seed = 0;

  ((seed ^= std::hash<Args>{}(args) + 0x9e3779b9 + (seed << 6) + (seed >> 2)), ...);

  return seed;
}

} // Namespace.
#endif
