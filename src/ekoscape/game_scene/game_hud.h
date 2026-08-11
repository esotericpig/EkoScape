/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_GAME_SCENE_GAME_HUD_H_
#define EKOSCAPE_GAME_SCENE_GAME_HUD_H_

#include "ekoscape/global.h"

#include "ekoscape/core/game_session.h"
#include "ekoscape/maps/map.h"

#include <cybel/chrono/duration.h>
#include <cybel/chrono/frame_delta.h>
#include <cybel/chrono/ticker.h>
#include <cybel/gfx/color.h>
#include <cybel/math/pos.h>
#include <cybel/math/size.h>
#include <cybel/renderer/renderer.h>
#include <cybel/scenes/scene_context.h>

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

  void update_logic(const FrameDelta& delta);
  void draw(Renderer& ren,const SceneContext& ctx);

private:
  static constexpr float kTextScale = 0.33f;
  static constexpr float kHudAlpha = 0.50f;

  static constexpr Size2i kMiniMapHoodRadius{4,3};
  static constexpr Size2i kMiniMapBlockSize{30,30};
  static constexpr Size2i kMiniMapSize{
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

  ChronoTicker speedrun_time_ticker_{Duration::from_millis(100.0),TickerFlag::kLoop | TickerFlag::kStart};
  Duration last_speedrun_time_{};
  std::string speedrun_time_str_{};

  void update_speedrun_time_str();

  void draw_map_mod(Renderer& ren,const SceneContext& ctx);
  void draw_mini_map(Renderer& ren,const SceneContext& ctx,Pos3i pos);
  void draw_speedrun_mod(Renderer& ren,const SceneContext& ctx);
};

} // namespace ekoscape
#endif
