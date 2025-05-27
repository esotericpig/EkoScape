/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_GAME_OVERLAY_H_
#define EKOSCAPE_SCENES_GAME_OVERLAY_H_

#include "common.h"

#include "cybel/scene/scene.h"
#include "cybel/types/color.h"
#include "cybel/types/duration.h"
#include "cybel/types/size.h"

#include "core/game_context.h"
#include "map/map.h"
#include "scenes/scene_action.h"
#include "world/star_sys.h"

#include <vector>

namespace ekoscape {

class GameOverlay : public Scene {
public:
  struct State {
    bool is_map_info = true;
    bool player_hit_end = false;
  };

  explicit GameOverlay(GameContext& ctx,const Map& map);

  void flash(const Color4f& color);
  void fade_to(const Color4f& color);
  void game_over(bool player_hit_end);

  void update_state(const State& state);

  void on_scene_input_event(input_id_t input_id,const ViewDimens& dimens) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren,const ViewDimens& dimens) override;

  float game_over_age() const;

private:
  enum class OptionType {
    kPlayAgain,
    kGoBack,
  };

  class Option {
  public:
    OptionType type{};
    std::string text{};

    explicit Option() = default;
    explicit Option(OptionType type,std::string_view text);
  };

  static inline const Color4f kTextBgColor{0.0f,0.5f};
  static inline const Size2i kTextBgPadding{15,10};
  static constexpr float kAlpha = 0.33f;
  static inline const Duration kFlashDuration = Duration::from_millis(500);
  static inline const Duration kFadeDuration = Duration::from_millis(3'000);
  static inline const Duration kGameOverDuration = Duration::from_millis(3'000);

  GameContext& ctx_;
  const Map& map_;
  State state_{};
  int scene_action_ = SceneAction::kNil;

  std::string map_info_{};
  Size2i map_info_str_size_{};
  Color4f flash_color_{};
  float flash_age_ = -1.0f;
  float flash_age_dir_ = 0.0f;
  Color4f fade_color_{};
  float fade_age_ = -1.0f;
  float game_over_age_ = -1.0f;
  std::vector<Option> game_over_opts_{};
  std::size_t game_over_opt_index_ = 0;
  StarSys star_sys_{};

  void draw_map_info(Renderer& ren);
  void draw_game_over(Renderer& ren);
};

} // namespace ekoscape
#endif
