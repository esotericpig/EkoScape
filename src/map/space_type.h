/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_SPACE_TYPE_H_
#define EKOSCAPE_MAP_SPACE_TYPE_H_

#include "cybel/common.h"

#include "facing.h"

namespace ekoscape {

/**
 * If change a value, need to update:
 * - assets/maps/README.md
 *
 * If add a new type, need to update:
 * - to_space_type(char)
 * - [maybe] Map.parse_grid()
 * - GameScene.generate_map()
 */
enum class SpaceType : char {
  kNil         = 0, // Not for game users.
  kCell        = '@',
  kDeadSpace   = 'x',
  kEmpty       = ' ',
  kEnd         = '$',
  kEndWall     = '&',
  kLivingSpace = '*',
  kPlayerEast  = '>',
  kPlayerNorth = '^',
  kPlayerSouth = 'v',
  kPlayerWest  = '<',
  kRobot       = '!',
  kRobotGhost  = '|',
  kRobotSnake  = 'Q',
  kRobotStatue = '=',
  kRobotWorm   = '?',
  kWall        = '#',
  kWallGhost   = '%',
  kWhite       = 'W',
  kWhiteFloor  = 'w',
  kWhiteGhost  = '+',
};

namespace SpaceTypes {
  inline const SpaceType kDefault = SpaceType::kDeadSpace;

  bool is_player(SpaceType type);
  bool is_robot(SpaceType type);
  bool is_walkable(SpaceType type);
  bool is_non_walkable(SpaceType type);

  Facing to_player_facing(SpaceType type);
  SpaceType to_player(Facing facing);

  SpaceType to_space_type(char value);

  /**
   * Convenience function for converting from Dantares' values to ours.
   * For example: Dantares.GetCurrentSpace()
   */
  SpaceType to_space_type(int value);

  char value_of(SpaceType type);
}

} // Namespace.
#endif
