/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UTIL_RANDO_H_
#define CYBEL_UTIL_RANDO_H_

#include "cybel/common.h"

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
  double rand_double(double max);
  double rand_double(double min,double max);
  double rand_double_vel();
  double rand_double_vel(double max);
  double rand_double_vel(double min,double max);
  double rand_double_sign();

  /**
   * Between 0.0f & 1.0f (exclusive).
   */
  float rand_float();
  float rand_float(float max);
  float rand_float(float min,float max);
  float rand_float_vel();
  float rand_float_vel(float max);
  float rand_float_vel(float min,float max);
  float rand_float_sign();

  /**
   * Between 0 & 100 (exclusive).
   */
  int rand_int();
  int rand_int(int max);
  int rand_int(int min,int max);
  int rand_int_vel();
  int rand_int_vel(int max);
  int rand_int_vel(int min,int max);
  int rand_int_sign();

  std::size_t rand_sizet(std::size_t max);
  std::size_t rand_sizet(std::size_t min,std::size_t max);

  template <class RandIt>
  void shuffle(RandIt first,RandIt last);

private:
  std::random_device dev_{};
  std::mt19937 gen_{dev_()};

  std::bernoulli_distribution bool_distr_{0.5};
  std::uniform_real_distribution<> double_distr_{0.0,1.0};
  std::uniform_real_distribution<float> float_distr_{0.0f,1.0f};
  std::uniform_int_distribution<> int_distr_{0,99}; // I don't have 99 problems.
  std::uniform_int_distribution<std::size_t> sizet_distr_{};

  explicit Rando();
};

template <class RandIt>
void Rando::shuffle(RandIt first,RandIt last) { std::shuffle(first,last,gen_); }

} // Namespace.
#endif
