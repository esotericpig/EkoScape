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
  explicit GameHud(Assets& assets);

  void draw(Renderer& ren,const ViewDimens& dimens,const DantaresMap& map,bool show_mini_map
      ,const Duration& player_fruit_time,bool player_hit_end);

private:
  static inline const Size2i kMiniMapHoodRadius{4,3};
  static inline const Size2i kMiniMapBlockSize{30,30};
  static inline const Size2i kMiniMapSize{
    // +1 for player.
    ((kMiniMapHoodRadius.w << 1) + 1) * kMiniMapBlockSize.w,
    ((kMiniMapHoodRadius.h << 1) + 1) * kMiniMapBlockSize.h
  };
  static inline const float kAlpha = 0.50f;

  Assets& assets_;

  Color4f mini_map_eko_color_{}; // Cell & Player.
  Color4f mini_map_end_color_{};
  Color4f mini_map_fruit_color_{};
  Color4f mini_map_non_walkable_color_{};
  Color4f mini_map_portal_color_{};
  Color4f mini_map_robot_color_{};
  Color4f mini_map_walkable_color_{};
};

} // Namespace.
#endif
