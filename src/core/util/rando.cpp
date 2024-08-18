/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rando.h"

namespace cybel {

Rando::Rando() {}

Rando& Rando::it() {
  static Rando it_{};
  return it_;
}

bool Rando::rand_bool() { return bool_distr_(generator_); }

} // Namespace.
