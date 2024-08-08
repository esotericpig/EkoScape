/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_RANDO_H_
#define EKOSCAPE_CORE_RANDO_H_

#include "common.h"

#include <random>

namespace ekoscape {

/**
 * Min & Max are always inclusive.
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

private:
  std::random_device device_{};
  std::mt19937 generator_{device_()};
  std::bernoulli_distribution bool_distr_{0.5};

  Rando();
};

} // Namespace.
#endif
