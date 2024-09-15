/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_GAME_HUD_H_
#define EKOSCAPE_SCENES_GAME_HUD_H_

#include "common.h"

#include "cybel/render/renderer.h"
#include "cybel/util/util.h"

#include "assets/assets.h"
#include "map/dantares_map.h"

namespace ekoscape {

class GameHud {
public:
  explicit GameHud(const Assets& assets);

  void draw(Renderer& ren,const DantaresMap& map,bool show_mini_map);

private:
  static const Size2i kMiniMapHoodRadius;
  static const Size2i kMiniMapBlockSize;
  static const Size2i kMiniMapSize;
  static const std::uint8_t kAlpha = 127;

  const Assets& assets_;

  Color4f mini_map_eko_color_{}; // Cell & Player.
  Color4f mini_map_end_color_{};
  Color4f mini_map_non_walkable_color_{};
  Color4f mini_map_robot_color_{};
  Color4f mini_map_walkable_color_{};
};

} // Namespace.
#endif
