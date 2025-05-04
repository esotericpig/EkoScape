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
#include "cybel/vfx/particle.h"

#include "core/game_context.h"
#include "scenes/scene_action.h"

#include <vector>

namespace ekoscape {

class MenuCreditsScene : public Scene {
public:
  static Color4f rand_color();

  explicit MenuCreditsScene(GameContext& ctx);

  void on_scene_input_event(int input_id,const ViewDimens& dimens) override;
  void handle_scene_input_states(const std::vector<bool>& states,const ViewDimens& dimens) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren,const ViewDimens& dimens) override;

private:
  class WtfParticle : public Particle {
  public:
    Pos3f true_pos{};
    Size2f true_size{};
  };

  static inline const std::string kWtfText = "wtf!?";

  GameContext& ctx_;
  int scene_action_ = SceneAction::kNil;

  std::vector<WtfParticle> wtfs_{};
  int active_wtf_count_ = 0;
  Duration wtf_cooldown_time_{};

  void birth_wtfs(const ViewDimens& dimens);
  void update_wtfs(const FrameStep& step,const ViewDimens& dimens);
  void draw_wtfs(Renderer& ren);
};

} // namespace ekoscape
#endif
