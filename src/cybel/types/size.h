/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_TYPES_SIZE_H_
#define CYBEL_TYPES_SIZE_H_

#include "cybel/common.h"

#include "cybel/types/pos.h"

namespace cybel {

template <typename T>
class Size2 {
public:
  T w{};
  T h{};

  explicit Size2() noexcept = default;
  explicit Size2(T w,T h) noexcept
    : w(w),h(h) {}

  auto operator<=>(const Size2&) const = default;

  bool in_bounds(const Pos2<T>& pos,const Size2& size) const {
    return ((pos.x + size.w) >= 0 && pos.x < w) &&
           ((pos.y + size.h) >= 0 && pos.y < h);
  }

  Size2& set(T w,T h) {
    this->w = w;
    this->h = h;

    return *this;
  }

  T area() const { return w * h; }

  float aspect_ratio() const {
    return static_cast<float>(w) / std::max(static_cast<float>(h),0.01f);
  }

  template <typename T2>
  Size2<T2> to_size2() const { return Size2<T2>{static_cast<T2>(w),static_cast<T2>(h)}; }
};

using Size2f = Size2<float>;
using Size2i = Size2<int>;

template <typename T>
std::ostream& operator<<(std::ostream& out,const Size2<T>& size) {
  return out << size.w << 'x' << size.h;
}

} // namespace cybel
#endif
