/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UTIL_TIMER_H_
#define CYBEL_UTIL_TIMER_H_

#include "cybel/common.h"

#include "cybel/types/duration.h"

namespace cybel {

class Timer {
public:
  explicit Timer(bool start = false);

  Timer& start();
  Timer& resume();
  Duration peek() const;
  const Duration& pause();

  const Duration& duration() const;

private:
  using timestamp_t = Uint64;

  timestamp_t start_time_{};
  bool is_paused_ = true;
  timestamp_t raw_duration_{};
  Duration duration_{};

  static timestamp_t now();
};

} // namespace cybel
#endif
