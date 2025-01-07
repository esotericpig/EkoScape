/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "facing.h"

namespace ekoscape {

bool Facings::is_valid(Facing facing) {
  // Don't use `default:` so that the compiler/IDE can catch new enum types.
  switch(facing) {
    case Facing::kNorth:
    case Facing::kSouth:
    case Facing::kEast:
    case Facing::kWest:
      return true;
  }

  return false;
}

Facing Facings::to_facing(int value) {
  const auto facing = static_cast<Facing>(value);

  return is_valid(facing) ? facing : kFallback;
}

int Facings::value_of(Facing facing) {
  if(!is_valid(facing)) { facing = kFallback; }

  return static_cast<int>(facing);
}

} // Namespace.
