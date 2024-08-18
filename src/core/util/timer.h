/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_UTIL_TIMER_H_
#define EKOSCAPE_CORE_UTIL_TIMER_H_

#include "core/common.h"

#include "duration.h"

namespace cybel {

class Timer {
public:
  Timer(bool start = false);

  Timer& start();
  Timer& end();

  std::uint32_t start_time() const;
  std::uint32_t end_time() const;
  const Duration& duration() const;

private:
  std::uint32_t start_time_ = 0;
  std::uint32_t end_time_ = 0;
  Duration duration_{};
};

} // Namespace.
#endif
