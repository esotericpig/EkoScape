/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_TYPES_POS_H_
#define CYBEL_TYPES_POS_H_

#include "cybel/common.h"

namespace cybel {

template <typename T>
class Pos3;

template <typename T>
class Pos2 {
public:
  T x{};
  T y{};

  explicit Pos2() noexcept = default;
  explicit Pos2(T x,T y) noexcept
    : x(x),y(y) {}

  bool operator==(const Pos2&) const = default;

  Pos2& set(T x,T y) {
    this->x = x;
    this->y = y;

    return *this;
  }

  template <typename T2>
  Pos2<T2> to_pos2() const { return Pos2<T2>{static_cast<T2>(x),static_cast<T2>(y)}; }

  template <typename T2>
  Pos3<T2> to_pos3() const { return Pos3<T2>{static_cast<T2>(x),static_cast<T2>(y),T2{}}; }
};

using Pos2f = Pos2<float>;
using Pos2i = Pos2<int>;

template <typename T>
class Pos3 {
public:
  T x{};
  T y{};
  T z{};

  explicit Pos3() noexcept = default;
  // NOTE: Not defaulting z to 0 on purpose, to help distinguish between Pos2.
  explicit Pos3(T x,T y,T z) noexcept
    : x(x),y(y),z(z) {}

  bool operator==(const Pos3&) const = default;

  Pos3 operator-() const { return Pos3{-x,-y,-z}; }

  Pos3& set(T x,T y) { return set(x,y,z); }

  Pos3& set(T x,T y,T z) {
    this->x = x;
    this->y = y;
    this->z = z;

    return *this;
  }

  template <typename T2>
  Pos2<T2> to_pos2() const { return Pos2<T2>{static_cast<T2>(x),static_cast<T2>(y)}; }

  template <typename T2>
  Pos3<T2> to_pos3() const { return Pos3<T2>{static_cast<T2>(x),static_cast<T2>(y),static_cast<T2>(z)}; }
};

using Pos3f = Pos3<float>;
using Pos3i = Pos3<int>;

struct Pos4f {
  float x1{};
  float y1{};
  float x2{};
  float y2{};
};

struct Pos5f {
  float x1{};
  float y1{};
  float x2{};
  float y2{};
  float z{};
};

} // Namespace.
#endif
