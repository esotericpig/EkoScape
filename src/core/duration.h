/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_DURATION_H_
#define EKOSCAPE_CORE_DURATION_H_

#include "common.h"

#include <cmath>

namespace ekoscape {

class Duration {
public:
  Duration();
  static Duration from_millis(double millis);
  static Duration from_secs(double secs);

  bool operator<(const Duration& other) const;
  bool operator<=(const Duration& other) const;
  bool operator==(const Duration& other) const;
  bool operator!=(const Duration& other) const;
  bool operator>(const Duration& other) const;
  bool operator>=(const Duration& other) const;
  Duration operator+(const Duration& other) const;
  Duration& operator+=(const Duration& other);
  Duration operator-(const Duration& other) const;
  Duration& operator-=(const Duration& other);

  Duration& set_from_millis(double millis);
  Duration& set_from_secs(double secs);

  double millis() const;
  double secs() const;

  Uint32 round_millis() const;
  Uint32 round_secs() const;

  Uint32 whole_millis() const;
  Uint32 whole_secs() const;

private:
  /**
   * Milliseconds, but made the name generic in case change it in the future.
   */
  double value_ = 0;

  Duration(double value);
};

} // Namespace.
#endif
