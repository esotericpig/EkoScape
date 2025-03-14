/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_TYPES_RANGE_H_
#define CYBEL_TYPES_RANGE_H_

#include "cybel/common.h"

namespace cybel {

class Range2i {
public:
  int min{};
  int max{};

  explicit Range2i() noexcept = default;
  explicit Range2i(int min,int max) noexcept;

  bool in_range(int value) const;

  Range2i& set(int min,int max);
};

} // namespace cybel
#endif
