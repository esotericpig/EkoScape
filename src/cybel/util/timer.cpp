/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "timer.h"

namespace cybel {

Timer::Timer(bool start) {
  if(start) { this->start(); }
}

Timer& Timer::start() {
  start_time_ = SDL_GetTicks();
  end_time_ = 0;

  return *this;
}

Timer& Timer::end() {
  end_time_ = SDL_GetTicks();
  duration_.set_from_millis(end_time_ - start_time_);

  return *this;
}

std::uint32_t Timer::start_time() const { return start_time_; }

std::uint32_t Timer::end_time() const { return end_time_; }

const Duration& Timer::duration() const { return duration_; }

} // Namespace.
