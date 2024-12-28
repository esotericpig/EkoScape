/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "facing.h"

namespace ekoscape {

Facing Facings::to_facing(int value) {
  auto facing = static_cast<Facing>(value);

  // Don't use `default:` so that the compiler/IDE can catch new enum types.
  switch(facing) {
    case Facing::kNorth:
    case Facing::kSouth:
    case Facing::kEast:
    case Facing::kWest:
      return facing;
  }

  return Facing::kSouth;
}

int Facings::value_of(Facing facing) {
  auto value = Facing::kSouth;

  switch(facing) {
    case Facing::kNorth:
    case Facing::kSouth:
    case Facing::kEast:
    case Facing::kWest:
      value = facing;
      break;
  }

  return static_cast<int>(value);
}

} // Namespace.
