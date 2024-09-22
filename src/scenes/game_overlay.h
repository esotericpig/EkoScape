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

  void flash(const Color4f& color);
  void fade_to(const Color4f& color);

  void update(const FrameStep& step);
  bool update_map_info();
  void update_game_over(const FrameStep& step,bool player_hit_end);

  void draw(Renderer& ren);
  void draw_map_info(Renderer& ren,const DantaresMap& map);
  void draw_game_over(Renderer& ren,const DantaresMap& map,bool player_hit_end);

  float game_over_age() const;

private:
  static inline const Color4f kTextBgColor{0.0f,0.5f};
  static inline const Size2i kTextBgPadding{15,10};
  static inline const Duration kMapInfoDuration = Duration::from_millis(3'000);
  static inline const Duration kFlashDuration = Duration::from_millis(500);
  static inline const Duration kFadeDuration = Duration::from_millis(3'000);
  static inline const Duration kGameOverDuration = Duration::from_millis(3'000);
  static inline const float kAlpha = 0.33f;

  Assets& assets_;
  ViewDimens view_dimens_{};

  Timer map_info_timer_{};
  Color4f flash_color_{};
  float flash_age_ = -1.0f;
  float flash_age_dir_ = 0.0f;
  Color4f fade_color_{};
  float fade_age_ = -1.0f;
  float game_over_age_ = 0.0f;
  StarSys star_sys_{};
};

} // Namespace.
#endif
