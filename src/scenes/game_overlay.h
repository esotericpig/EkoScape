/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_GAME_OVERLAY_H_
#define EKOSCAPE_SCENES_GAME_OVERLAY_H_

#include "common.h"

#include "cybel/gfx/renderer.h"
#include "cybel/input/input_types.h"
#include "cybel/scene/scene_context.h"
#include "cybel/types/color.h"
#include "cybel/types/frame_step.h"
#include "cybel/types/size.h"
#include "cybel/util/tween.h"

#include "core/game_session.h"
#include "map/map.h"
#include "world/star_sys.h"

#include <string>
#include <vector>

namespace ekoscape {

class GameOverlay final {
public:
  struct State final {
    bool show_map_info = true;
    bool player_hit_end = false;
  };

  explicit GameOverlay(GameSession& sesh,const Map& map);

  void flash(const Color4f& color);
  void fade_to(const Color4f& color);
  void game_over(bool player_hit_end,const SceneContext& ctx);

  void update_state(const State& state);

  void on_input_event(input_id_t input_id,const SceneContext& ctx);

  void update_logic(const FrameStep& step,const SceneContext& ctx);
  void draw(Renderer& ren,const SceneContext& ctx);

  float game_over_age() const;

private:
  enum class OptionType {
    kPlayAgain,
    kGoBack,
  };

  struct Option final {
    OptionType type{};
    std::string text{};
  };

  static inline const Color4f kTextBgColor{0.0f,0.5f};
  static inline const Size2i kTextBgPadding{15,10};
  static constexpr float kEffectAlpha = 0.33f;

  GameSession& sesh_;
  const Map& map_;
  State state_{};

  std::string map_info_{};
  Size2i map_info_str_size_{};

  Color4f flash_color_{};
  Tween0f flash_tween_{1.0f};
  Color4f fade_color_{};
  Tween0f fade_tween_{3.0f};

  Tween0f game_over_tween_{3.0f};
  std::vector<Option> game_over_opts_{};
  std::size_t game_over_opt_index_ = 0;
  StarSys star_sys_{};

  void draw_map_info(Renderer& ren,const SceneContext& ctx);
  void draw_game_over(Renderer& ren,const SceneContext& ctx);
};

} // namespace ekoscape
#endif
