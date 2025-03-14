/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "timer.h"

namespace cybel {

Timer::Timestamp Timer::now() { return SDL_GetTicks64(); }

Timer::Timer(bool start) {
  if(start) { this->start(); }
}

Timer& Timer::start() {
  raw_duration_ = 0;
  duration_.zero();

  return resume();
}

Timer& Timer::resume() {
  start_time_ = now();
  is_paused_ = false;

  return *this;
}

Duration Timer::peek() const {
  if(is_paused_) { return duration_; }

  const auto dur = raw_duration_ + (now() - start_time_);

  return Duration::from_millis(static_cast<double>(dur));
}

const Duration& Timer::pause() {
  if(!is_paused_) {
    is_paused_ = true;
    raw_duration_ += (now() - start_time_); // `+=` for resuming.
    duration_.set_from_millis(static_cast<double>(raw_duration_));
  }

  return duration_;
}

const Duration& Timer::duration() const { return duration_; }

} // namespace cybel
