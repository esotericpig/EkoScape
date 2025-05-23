/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "space_type.h"

namespace ekoscape {

bool SpaceTypes::is_valid(SpaceType type) {
  // NOTE: Don't use `default:` so that the compiler/IDE can catch new enum types.
  switch(type) {
    case SpaceType::kNil:
      return false;

    case SpaceType::kCell:
    case SpaceType::kDeadSpace:
    case SpaceType::kDeadSpaceGhost:
    case SpaceType::kEmpty:
    case SpaceType::kEnd:
    case SpaceType::kEndWall:
    case SpaceType::kFruit:
    case SpaceType::kPlayerEast:
    case SpaceType::kPlayerNorth:
    case SpaceType::kPlayerSouth:
    case SpaceType::kPlayerWest:
    case SpaceType::kPortal0:
    case SpaceType::kPortal1:
    case SpaceType::kPortal2:
    case SpaceType::kPortal3:
    case SpaceType::kPortal4:
    case SpaceType::kPortal5:
    case SpaceType::kPortal6:
    case SpaceType::kPortal7:
    case SpaceType::kPortal8:
    case SpaceType::kPortal9:
    case SpaceType::kRobot:
    case SpaceType::kRobotGhost:
    case SpaceType::kRobotSnake:
    case SpaceType::kRobotStatue:
    case SpaceType::kRobotWorm:
    case SpaceType::kVoid:
    case SpaceType::kWall:
    case SpaceType::kWallGhost:
    case SpaceType::kWhite:
    case SpaceType::kWhiteFloor:
    case SpaceType::kWhiteGhost:
      return true;
  }

  return false;
}

bool SpaceTypes::is_valid_empty(SpaceType type) {
  // Cannot have 2+ things on a single space.
  return is_valid(type) && !is_player(type) && !is_thing(type);
}

bool SpaceTypes::is_player(SpaceType type) {
  switch(type) {
    case SpaceType::kPlayerNorth:
    case SpaceType::kPlayerSouth:
    case SpaceType::kPlayerEast:
    case SpaceType::kPlayerWest:
      return true;

    default: return false;
  }
}

bool SpaceTypes::is_thing(SpaceType type) {
  switch(type) {
    case SpaceType::kCell:
    case SpaceType::kEnd:
    case SpaceType::kFruit:
      return true;

    default: return is_robot(type);
  }
}

bool SpaceTypes::is_robot(SpaceType type) {
  switch(type) {
    case SpaceType::kRobot:
    case SpaceType::kRobotGhost:
    case SpaceType::kRobotSnake:
    case SpaceType::kRobotStatue:
    case SpaceType::kRobotWorm:
      return true;

    default: return false;
  }
}

bool SpaceTypes::is_portal(SpaceType type) {
  switch(type) {
    case SpaceType::kPortal0:
    case SpaceType::kPortal1:
    case SpaceType::kPortal2:
    case SpaceType::kPortal3:
    case SpaceType::kPortal4:
    case SpaceType::kPortal5:
    case SpaceType::kPortal6:
    case SpaceType::kPortal7:
    case SpaceType::kPortal8:
    case SpaceType::kPortal9:
      return true;

    default: return false;
  }
}

bool SpaceTypes::is_walkable(SpaceType type) { return !is_non_walkable(type); }

bool SpaceTypes::is_non_walkable(SpaceType type) {
  switch(type) {
    case SpaceType::kDeadSpace:
    case SpaceType::kEndWall:
    case SpaceType::kVoid:
    case SpaceType::kWall:
    case SpaceType::kWhite:
      return true;

    default: return false;
  }
}

Facing SpaceTypes::to_player_facing(SpaceType type) {
  switch(type) {
    case SpaceType::kPlayerNorth: return Facing::kNorth;
    case SpaceType::kPlayerSouth: return Facing::kSouth;
    case SpaceType::kPlayerEast:  return Facing::kEast;
    case SpaceType::kPlayerWest:  return Facing::kWest;

    default: return Facings::kFallback;
  }
}

SpaceType SpaceTypes::to_player(Facing facing) {
  switch(facing) {
    case Facing::kNorth: return SpaceType::kPlayerNorth;
    case Facing::kSouth: return SpaceType::kPlayerSouth;
    case Facing::kEast:  return SpaceType::kPlayerEast;
    case Facing::kWest:  return SpaceType::kPlayerWest;

    default: return SpaceType::kPlayerSouth;
  }
}

SpaceType SpaceTypes::to_space_type(char value) {
  const auto type = static_cast<SpaceType>(value);

  return is_valid(type) ? type : kFallback;
}

SpaceType SpaceTypes::to_space_type(int value) { return to_space_type(static_cast<char>(value)); }

char SpaceTypes::value_of(SpaceType type) {
  if(!is_valid(type)) { type = kFallback; }

  return static_cast<char>(type);
}

} // namespace ekoscape
