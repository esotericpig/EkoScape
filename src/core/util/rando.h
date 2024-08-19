/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_UTIL_RANDO_H_
#define EKOSCAPE_CORE_UTIL_RANDO_H_

#include "core/common.h"

#include <random>

namespace cybel {

/**
 * Min is always inclusive.
 * Max is always exclusive.
 *
 * Internally, this class passes the min/max to the distribution function.
 * In my benchmark tests (on my system) using chrono, there was no increased time difference doing this,
 * as opposed to using the constructor only, with a loop of 10 million.
 *
 * Rambo?
 */
class Rando {
public:
  static Rando& it();

  Rando(const Rando& other) = delete;
  Rando(Rando&& other) noexcept = delete;

  Rando& operator=(const Rando& other) = delete;
  Rando& operator=(Rando&& other) noexcept = delete;

  bool rand_bool();

  /**
   * Between 0.0 & 1.0 (exclusive);
   */
  double rand_double();
  double rand_double(double min,double max);

  /**
   * Between 0.0f & 1.0f (exclusive).
   */
  float rand_float();
  float rand_float(float min,float max);

  /**
   * Between 0 & 100 (exclusive).
   */
  int rand_int();
  int rand_int(int max);
  int rand_int(int min,int max);

private:
  std::random_device dev_{};
  std::mt19937 gen_{dev_()};

  std::bernoulli_distribution bool_distr_{0.5};
  std::uniform_real_distribution<double> double_distr_{0.0,1.0};
  std::uniform_real_distribution<float> float_distr_{0.0f,1.0f};
  std::uniform_int_distribution<int> int_distr_{0,99}; // I don't have 99 problems.

  Rando();
};

} // Namespace.
#endif
