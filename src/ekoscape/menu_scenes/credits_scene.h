/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MENU_SCENES_CREDITS_SCENE_H_
#define EKOSCAPE_MENU_SCENES_CREDITS_SCENE_H_

#include "ekoscape/global.h"

#include "ekoscape/core/game_session.h"

#include <cybel/chrono/ticker.h>
#include <cybel/gfx/color.h>
#include <cybel/math/pos.h>
#include <cybel/math/size.h>
#include <cybel/scenes/scene.h>
#include <cybel/scenes/scene_context.h>
#include <cybel/vfx/particle.h>

#include <cstddef>
#include <string>
#include <vector>

namespace ekoscape {

class CreditsScene final : public Scene {
public:
  explicit CreditsScene(GameSession& sesh);

  void on_scene_input_event(input_id_t input_id,SceneContext& ctx) override;
  void handle_scene_input(InputMan& input,SceneContext& ctx) override;

  void update_scene_logic(const FrameDelta& delta,SceneContext& ctx) override;
  void draw_scene(Renderer& ren,SceneContext& ctx) override;

private:
  struct WtfParticle final {
    Particle p{};
    Pos3f true_pos{};
    Size2f true_size{};
  };

  static constexpr std::string kWtfText = "wtf!?";

  GameSession& sesh_;

  std::vector<WtfParticle> wtfs_{};
  std::size_t active_wtf_count_ = 0;
  Ticker0f wtf_cooldown_{0.110f};

  void birth_wtfs(const Viewport& view);
  static Color4f rand_color();
  void update_wtfs(const FrameDelta& delta,const SceneContext& ctx);
  void draw_wtfs(Renderer& ren,const SceneContext& ctx);
};

} // namespace ekoscape
#endif
