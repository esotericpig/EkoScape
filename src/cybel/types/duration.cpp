/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "duration.h"

namespace cybel {

const Duration Duration::kZero{0.0};

Duration Duration::from_millis(double millis) noexcept { return Duration{}.set_from_millis(millis); }

Duration Duration::from_secs(double secs) noexcept { return Duration{}.set_from_secs(secs); }

Duration::Duration(double value) noexcept
  : value_(value) {}

bool Duration::operator<(const Duration& other) const { return value_ < other.value_; }

bool Duration::operator<=(const Duration& other) const { return value_ <= other.value_; }

bool Duration::operator==(const Duration& other) const { return value_ == other.value_; }

bool Duration::operator!=(const Duration& other) const { return value_ != other.value_; }

bool Duration::operator>(const Duration& other) const { return value_ > other.value_; }

bool Duration::operator>=(const Duration& other) const { return value_ >= other.value_; }

auto Duration::operator<=>(const Duration& other) const { return value_ <=> other.value_; }

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

Duration Duration::operator*(const Duration& other) const { return Duration{value_ * other.value_}; }

Duration& Duration::operator*=(const Duration& other) {
  value_ *= other.value_;

  return *this;
}

Duration Duration::operator/(const Duration& other) const { return Duration{value_ / other.value_}; }

Duration& Duration::operator/=(const Duration& other) {
  value_ /= other.value_;

  return *this;
}

Duration Duration::operator%(const Duration& other) const {
  return Duration{std::fmod(value_,other.value_)};
}

Duration& Duration::operator%=(const Duration& other) {
  value_ = std::fmod(value_,other.value_);

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

Duration& Duration::zero() {
  value_ = 0.0;

  return *this;
}

double Duration::millis() const { return value_; }

double Duration::secs() const { return value_ / 1000.0; }

std::uint32_t Duration::round_millis() const { return static_cast<std::uint32_t>(std::round(millis())); }

std::uint32_t Duration::round_secs() const { return static_cast<std::uint32_t>(std::round(secs())); }

std::uint32_t Duration::whole_millis() const { return static_cast<std::uint32_t>(millis()); }

std::uint32_t Duration::whole_secs() const { return static_cast<std::uint32_t>(secs()); }

} // Namespace.
