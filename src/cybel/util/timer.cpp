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

Timer& Timer::resume() {
  const std::uint32_t dur = duration_.round_millis();

  start();

  if(dur <= start_time_) {
    start_time_ -= dur;
  } else {
    start_time_ = 0; // Shouldn't happen, technically.
  }

  return *this;
}

Timer& Timer::end() {
  end_time_ = SDL_GetTicks();
  duration_.set_from_millis(end_time_ - start_time_);

  return *this;
}

const Duration& Timer::duration() const { return duration_; }

} // Namespace.
