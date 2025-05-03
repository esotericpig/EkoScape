/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_STYLED_TEX_ID_H_
#define EKOSCAPE_ASSETS_STYLED_TEX_ID_H_

#include "common.h"

#include "cybel/asset/asset_types.h"

namespace ekoscape {

enum class StyledTexId : asset_id_t {
  kCeiling,
  kCell,
  kDeadSpace,
  kDeadSpaceGhost,
  kEnd,
  kEndWall,
  kFloor,
  kFruit,
  kPortal,
  kRobot,
  kWall,
  kWallGhost,
  kWhite,
  kWhiteGhost,

  kMax
};

} // namespace ekoscape
#endif
