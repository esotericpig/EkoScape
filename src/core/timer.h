/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_TIMER_H_
#define EKOSCAPE_CORE_TIMER_H_

#include "common.h"

#include "duration.h"

namespace ekoscape {

class Timer {
public:
  Timer(bool start = false);

  Timer& start();
  Timer& end();

  Uint32 start_time() const;
  Uint32 end_time() const;
  const Duration& duration() const;

private:
  Uint32 start_time_ = 0;
  Uint32 end_time_ = 0;
  Duration duration_{};
};

} // Namespace.
#endif
