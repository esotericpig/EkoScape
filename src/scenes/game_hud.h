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
#include "cybel/types/color.h"
#include "cybel/types/duration.h"
#include "cybel/types/frame_step.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"
#include "cybel/types/view_dimens.h"

#include "map/map.h"
#include "game_context.h"

namespace ekoscape {

class GameHud {
public:
  struct State {
    const Map& map;
    const bool& show_mini_map;
    const Duration& player_fruit_time;
    const bool& player_hit_end;

    bool is_game_over = false;
    Duration speedrun_time{};
    const bool& show_speedrun;
  };

  State state;

  explicit GameHud(GameContext& ctx,const State& state);

  void update(const FrameStep& step);
  void draw(Renderer& ren,const ViewDimens& dimens);

private:
  static constexpr float kTextScale = 0.33f;
  static constexpr float kAlpha = 0.50f;

  static inline const Size2i kMiniMapHoodRadius{4,3};
  static inline const Size2i kMiniMapBlockSize{30,30};
  static inline const Size2i kMiniMapSize{
    // +1 for player.
    ((kMiniMapHoodRadius.w << 1) + 1) * kMiniMapBlockSize.w,
    ((kMiniMapHoodRadius.h << 1) + 1) * kMiniMapBlockSize.h
  };

  GameContext& ctx_;

  Color4f mini_map_eko_color_{}; // Cell & Player.
  Color4f mini_map_end_color_{};
  Color4f mini_map_fruit_color_{};
  Color4f mini_map_non_walkable_color_{};
  Color4f mini_map_portal_color_{};
  Color4f mini_map_robot_color_{};
  Color4f mini_map_walkable_color_{};

  Duration last_updated_speedrun_time_{};
  Duration last_speedrun_time_{};
  std::string speedrun_time_str_{};

  void update_speedrun_time_str();

  void draw_map_mod(Renderer& ren,const ViewDimens& dimens);
  void draw_mini_map(Renderer& ren,Pos3i pos);
  void draw_speedrun_mod(Renderer& ren,const ViewDimens& dimens);
};

} // namespace ekoscape
#endif
