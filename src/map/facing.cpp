/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "facing.h"

namespace ekoscape {

Facing Facings::to_facing(int value) {
  Facing facing = static_cast<Facing>(value);

  switch(facing) {
    case Facing::kNorth:
    case Facing::kSouth:
    case Facing::kEast:
    case Facing::kWest:
      return facing;

    // Don't use `default:` so that the compiler/IDE can catch new enum types.
  }

  return Facing::kSouth;
}

int Facings::value_of(Facing facing) { return static_cast<int>(facing); }

} // Namespace.
