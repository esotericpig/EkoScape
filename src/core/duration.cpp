/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "duration.h"

namespace ekoscape {

Duration::Duration() {}

Duration::Duration(double value)
    : value_(value) {}

Duration Duration::from_millis(double millis) { return Duration{}.set_from_millis(millis); }

Duration Duration::from_secs(double secs) { return Duration{}.set_from_secs(secs); }

bool Duration::operator<(const Duration& other) const { return value_ < other.value_; }

bool Duration::operator<=(const Duration& other) const { return value_ <= other.value_; }

bool Duration::operator==(const Duration& other) const { return value_ == other.value_; }

bool Duration::operator!=(const Duration& other) const { return value_ != other.value_; }

bool Duration::operator>(const Duration& other) const { return value_ > other.value_; }

bool Duration::operator>=(const Duration& other) const { return value_ >= other.value_; }

Duration Duration::operator+(const Duration& other) const { return Duration{value_ + other.value_}; }

Duration& Duration::operator+=(const Duration& other) {
  value_ += other.value_;

  return *this;
}

Duration Duration::operator-(const Duration& other) const { return Duration{value_ - other.value_}; }

Duration& Duration::operator-=(const Duration& other) {
  value_ -= other.value_;

  return *this;
}

Duration& Duration::set_from_millis(double millis) {
  value_ = millis;

  return *this;
}

Duration& Duration::set_from_secs(double secs) {
  value_ = secs * 1000;

  return *this;
}

double Duration::millis() const { return value_; }

double Duration::secs() const { return value_ / 1000.0; }

Uint32 Duration::round_millis() const { return static_cast<Uint32>(std::round(millis())); }

Uint32 Duration::round_secs() const { return static_cast<Uint32>(std::round(secs())); }

Uint32 Duration::whole_millis() const { return static_cast<Uint32>(millis()); }

Uint32 Duration::whole_secs() const { return static_cast<Uint32>(secs()); }

} // Namespace.
