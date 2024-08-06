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
#include <functional>
#include <random>
#include <sstream>

namespace ekoscape {

/**
 * Clamped float to [0,1].
 */
class Color4f {
public:
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  float a = 1.0f;

  Color4f();
  Color4f(float r,float g,float b,float a);
  Color4f(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a);

  Color4f& set_f(float r,float g,float b,float a);
  Color4f& set_i(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a);
};

/**
 * Min & Max are inclusive.
 *
 * Rambo?
 */
class Rando {
public:
  static Rando& it();

  Rando(const Rando& other) = delete;
  Rando(Rando&& other) noexcept = delete;

  Rando& operator=(const Rando& other) = delete;
  Rando& operator=(Rando&& other) noexcept = delete;

  bool rand_bool();
  std::uint32_t rand_uint(std::uint32_t min,std::uint32_t max);

private:
  std::random_device device_{};
  std::mt19937 generator_{device_()};
  std::bernoulli_distribution bool_distr_{0.5};
  std::uniform_int_distribution<std::uint32_t> uint_distr_{0,UINT32_MAX};

  Rando();
};

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
