/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_MENU_CREDITS_SCENE_H_
#define EKOSCAPE_SCENES_MENU_CREDITS_SCENE_H_

#include "common.h"

#include "cybel/scene/scene.h"
#include "cybel/types/color.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"
#include "cybel/util/ticker.h"
#include "cybel/vfx/particle.h"

#include "core/game_session.h"

#include <vector>

namespace ekoscape {

class MenuCreditsScene final : public Scene {
public:
  explicit MenuCreditsScene(GameSession& sesh);

  void on_scene_input_event(input_id_t input_id,SceneContext& ctx) override;
  void handle_scene_input(InputMan& input,SceneContext& ctx) override;

  void update_scene_logic(const FrameStep& step,SceneContext& ctx) override;
  void draw_scene(Renderer& ren,SceneContext& ctx) override;

private:
  struct WtfParticle final {
    Particle p{};
    Pos3f true_pos{};
    Size2f true_size{};
  };

  static inline const std::string kWtfText = "wtf!?";

  GameSession& sesh_;

  std::vector<WtfParticle> wtfs_{};
  std::size_t active_wtf_count_ = 0;
  Ticker0f wtf_cooldown_{0.110f};

  void birth_wtfs(const ViewDimens& dimens);
  static Color4f rand_color();
  void update_wtfs(const FrameStep& step,const SceneContext& ctx);
  void draw_wtfs(Renderer& ren,const SceneContext& ctx);
};

} // namespace ekoscape
#endif
