/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_GAME_HUD_H_
#define EKOSCAPE_SCENES_GAME_HUD_H_

#include "common.h"

#include "cybel/gfx/renderer.h"
#include "cybel/scene/scene_context.h"
#include "cybel/types/color.h"
#include "cybel/types/duration.h"
#include "cybel/types/frame_step.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"
#include "cybel/util/ticker.h"

#include "core/game_session.h"
#include "map/map.h"

#include <string>

namespace ekoscape {

class GameHud final {
public:
  struct State final {
    bool show_mini_map = false;
    Duration player_fruit_time_left{};
    bool player_hit_end = false;

    bool is_game_over = false;
    Duration speedrun_time{};
    bool show_speedrun = false;
  };

  explicit GameHud(GameSession& sesh,const Map& map);

  void update_state(const State& state);

  void update_logic(const FrameStep& step);
  void draw(Renderer& ren,const SceneContext& ctx);

private:
  static constexpr float kTextScale = 0.33f;
  static constexpr float kHudAlpha = 0.50f;

  static inline const Size2i kMiniMapHoodRadius{4,3};
  static inline const Size2i kMiniMapBlockSize{30,30};
  static inline const Size2i kMiniMapSize{
    // +1 for player.
    ((kMiniMapHoodRadius.w << 1) + 1) * kMiniMapBlockSize.w,
    ((kMiniMapHoodRadius.h << 1) + 1) * kMiniMapBlockSize.h
  };

  GameSession& sesh_;
  const Map& map_;
  State state_{};

  Color4f mini_map_eko_color_{}; // Cell & Player.
  Color4f mini_map_end_color_{};
  Color4f mini_map_fruit_color_{};
  Color4f mini_map_non_walkable_color_{};
  Color4f mini_map_portal_color_{};
  Color4f mini_map_robot_color_{};
  Color4f mini_map_walkable_color_{};

  ChronoTicker speedrun_time_ticker_{Duration::from_millis(100.0),ChronoTicker::kLoop | ChronoTicker::kStart};
  Duration last_speedrun_time_{};
  std::string speedrun_time_str_{};

  void update_speedrun_time_str();

  void draw_map_mod(Renderer& ren,const SceneContext& ctx);
  void draw_mini_map(Renderer& ren,const SceneContext& ctx,Pos3i pos);
  void draw_speedrun_mod(Renderer& ren,const SceneContext& ctx);
};

} // namespace ekoscape
#endif
