/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

// ReSharper disable CppNonExplicitConvertingConstructor

#ifndef CYBEL_TYPES_H_
#define CYBEL_TYPES_H_

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
  Color4f(float rgb,float a = 1.0f);
  Color4f(float r,float g,float b,float a = 1.0f);

  Color4f& set(float rgb);
  Color4f& set(float rgb,float a);
  Color4f& set(float r,float g,float b);
  Color4f& set(float r,float g,float b,float a);
  Color4f& set_hex(std::uint32_t rgb);
  Color4f& set_hex(std::uint32_t rgb,std::uint8_t a);
  Color4f& set_bytes(std::uint8_t rgb);
  Color4f& set_bytes(std::uint8_t rgb,std::uint8_t a);
  Color4f& set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b);
  Color4f& set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a);

  Color4f with_r(float r) const;
  Color4f with_g(float g) const;
  Color4f with_b(float b) const;
  Color4f with_a(float a) const;
  Color4f with_byte_r(std::uint8_t r) const;
  Color4f with_byte_g(std::uint8_t g) const;
  Color4f with_byte_b(std::uint8_t b) const;
  Color4f with_byte_a(std::uint8_t a) const;
};

template <typename T>
class Pos3;

template <typename T>
class Pos2 {
public:
  T x{};
  T y{};

  Pos2() {}
  Pos2(T x,T y) : x(x),y(y) {}

  bool operator==(const Pos2&) const = default;

  Pos2& set(T x,T y) {
    this->x = x;
    this->y = y;
    return *this;
  }

  template <typename T2>
  Pos2<T2> to_pos2() const { return {static_cast<T2>(x),static_cast<T2>(y)}; }

  template <typename T2>
  Pos3<T2> to_pos3() const { return {static_cast<T2>(x),static_cast<T2>(y)}; }
};

using Pos2f = Pos2<float>;
using Pos2i = Pos2<int>;

template <typename T>
class Pos3 {
public:
  T x{};
  T y{};
  T z{};

  Pos3() {}
  Pos3(T x,T y,T z) : x(x),y(y),z(z) {}

  bool operator==(const Pos3&) const = default;

  Pos3& set(T x,T y) { return set(x,y,z); }

  Pos3& set(T x,T y,T z) {
    this->x = x;
    this->y = y;
    this->z = z;
    return *this;
  }

  template <typename T2>
  Pos2<T2> to_pos2() const { return {static_cast<T2>(x),static_cast<T2>(y)}; }

  template <typename T2>
  Pos3<T2> to_pos3() const { return {static_cast<T2>(x),static_cast<T2>(y),static_cast<T2>(z)}; }
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

  Range2i() {}
  Range2i(int min,int max) : min(min),max(max) {}

  bool in_range(int value) const;

  Range2i& set(int min,int max);
};

template <typename T>
class Size2 {
public:
  T w{};
  T h{};

  Size2() {}
  Size2(T w,T h) : w(w),h(h) {}

  bool in_bounds(const Pos2<T>& pos,const Size2<T>& size) const {
    return (pos.x + size.w) >= 0 && pos.x <= w
        && (pos.y + size.h) >= 0 && pos.y <= h;
  }

  Size2& set(T w,T h) {
    this->w = w;
    this->h = h;
    return *this;
  }

  template <typename T2>
  Size2<T2> to_size2() const { return {static_cast<T2>(w),static_cast<T2>(h)}; }
};

using Size2f = Size2<float>;
using Size2i = Size2<int>;

} // Namespace.
#endif
