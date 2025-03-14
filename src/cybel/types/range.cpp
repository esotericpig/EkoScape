/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "range.h"

namespace cybel {

Range2i::Range2i(int min,int max) noexcept
  : min(min),max(max) {}

bool Range2i::in_range(int value) const { return value >= min && value <= max; }

Range2i& Range2i::set(int min,int max) {
  this->min = min;
  this->max = max;

  return *this;
}

} // namespace cybel
