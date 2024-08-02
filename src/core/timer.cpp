/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "timer.h"

namespace ekoscape {

Timer::Timer(bool start) {
  if(start) { this->start(); }
}

Timer& Timer::start() {
  start_time_ = SDL_GetTicks();

  return *this;
}

Timer& Timer::end() {
  end_time_ = SDL_GetTicks();
  duration_.set_from_millis(end_time_ - start_time_);

  return *this;
}

Uint32 Timer::start_time() const { return start_time_; }

Uint32 Timer::end_time() const { return end_time_; }

const Duration& Timer::duration() const { return duration_; }

} // Namespace.
