/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_TYPES_H_
#define EKOSCAPE_CORE_TYPES_H_

#include "common.h"

namespace cybel {

/**
 * Clamped floats to [0,1].
 */
class Color4f {
public:
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  float a = 1.0f;

  static Color4f hex(std::uint32_t rgb,std::uint8_t a = 255);
  static Color4f bytes(std::uint8_t rgb,std::uint8_t a = 255);
  static Color4f bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a = 255);

  Color4f();
  explicit Color4f(float rgb,float a = 1.0f);
  Color4f(float r,float g,float b,float a = 1.0f);

  Color4f& set(float rgb,float a = 1.0f);
  Color4f& set(float r,float g,float b,float a = 1.0f);
  Color4f& set_hex(std::uint32_t rgb,std::uint8_t a = 255);
  Color4f& set_bytes(std::uint8_t rgb,std::uint8_t a = 255);
  Color4f& set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a = 255);
};

struct Pos2f {
  float x = 0.0f;
  float y = 0.0f;
};

struct Pos2i {
  int x = 0;
  int y = 0;
};

template <typename T>
class Pos3 {
public:
  T x{};
  T y{};
  T z{};

  Pos3() {}
  Pos3(T x,T y,T z = {})
      : x(x),y(y),z(z) {}
};

using Pos3f = Pos3<float>;
using Pos3i = Pos3<int>;

struct Pos4f {
  float x1 = 0.0f;
  float y1 = 0.0f;
  float x2 = 0.0f;
  float y2 = 0.0f;
};

struct Pos5f {
  float x1 = 0.0f;
  float y1 = 0.0f;
  float x2 = 0.0f;
  float y2 = 0.0f;
  float z = 0.0f;
};

class Range2i {
public:
  int min = 0;
  int max = 0;

  bool in_range(int value) const;
};

template <typename T>
class Size2 {
public:
  T w{};
  T h{};

  template <typename P>
  bool in_bounds(P x,P y) const { return x >= 0 && x < w && y >= 0 && y < h; }
};

using Size2f = Size2<float>;
using Size2i = Size2<int>;

struct ViewDimens {
  Size2i size{};
  Size2i target_size{};
  Size2i init_size{};
  float scale = 1.0f;
};

} // Namespace.
#endif
