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
#include "world/star_sys.h"

namespace ekoscape {

class GameOverlay {
public:
  explicit GameOverlay(Assets& assets);

  void init(const ViewDimens& dimens);

  bool update_map_info();
  void update_game_over(const FrameStep& step,bool player_hit_end);

  void draw_map_info(Renderer& ren,const DantaresMap& map);
  void draw_game_over(Renderer& ren,const DantaresMap& map,bool player_hit_end);

  float game_over_age() const;

private:
  static inline const Color4f kTextBgColor{0.0f,0.5f};
  static inline const Size2i kTextBgPadding{15,10};
  static inline const Duration kMapInfoDuration = Duration::from_millis(3'000);
  static inline const float kGameOverLifespan = 3.0f; // Seconds.

  Assets& assets_;
  ViewDimens view_dimens_{};

  Timer map_info_timer_{};
  float game_over_age_ = 0.0f;
  StarSys star_sys_{};
};

} // Namespace.
#endif