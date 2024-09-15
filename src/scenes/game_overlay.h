/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_GAME_OVERLAY_H_
#define EKOSCAPE_SCENES_GAME_OVERLAY_H_

#include "common.h"

#include "cybel/render/renderer.h"
#include "cybel/scene/scene_types.h"
#include "cybel/util/duration.h"
#include "cybel/util/timer.h"

#include "assets/assets.h"
#include "map/dantares_map.h"

namespace ekoscape {

class GameOverlay {
public:
  explicit GameOverlay(const Assets& assets);

  void init_scene();

  bool update_map_info();
  void update_game_over(const FrameStep& step);

  void draw_map_info(Renderer& ren,const DantaresMap& map);
  void draw_game_over(Renderer& ren,const DantaresMap& map,bool player_hit_end);

private:
  static const Color4f kTextBgColor;
  static const Size2i kTextBgPadding;
  static const Duration kMapInfoDuration;
  static const float kGameOverLifespan;

  const Assets& assets_;

  Timer map_info_timer_{};
  float game_over_age_ = 0.0f;
};

} // Namespace.
#endif
