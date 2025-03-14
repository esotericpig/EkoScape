/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_FACING_H_
#define EKOSCAPE_MAP_FACING_H_

#include "common.h"

namespace ekoscape {

// - See Dantares.SetPlayerPosition() or Dantares.IsWalking() for values.
// - If add a new type, need to update: Facings::is_valid().
enum class Facing : int {
  kNorth = 0,
  kSouth = 2,
  kEast  = 1,
  kWest  = 3,
};

namespace Facings {
  inline const Facing kFallback = Facing::kSouth;

  bool is_valid(Facing facing);
  Facing to_facing(int value);
  int value_of(Facing facing);
}

} // namespace ekoscape
#endif
