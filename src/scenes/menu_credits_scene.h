/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_MENU_CREDITS_SCENE_H_
#define EKOSCAPE_SCENES_MENU_CREDITS_SCENE_H_

#include "common.h"

#include "cybel/render/render_types.h"
#include "cybel/scene/scene.h"
#include "cybel/util/rando.h"
#include "cybel/vfx/particle.h"

#include "game_context.h"
#include "scene_action.h"

#include <vector>

namespace ekoscape {

class MenuCreditsScene : public Scene {
public:
  static Color4f rand_color();

  explicit MenuCreditsScene(GameContext& ctx);

  void on_key_down_event(const KeyEvent& event,const ViewDimens& dimens) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren,const ViewDimens& dimens) override;

private:
  class WtfParticle : public Particle {
  public:
    Pos3f true_pos{};
    Size2f true_size{};
  };

  static inline const StrUtf8 kWtfText = "wtf!?";

  GameContext& ctx_;
  int scene_action_ = SceneAction::kNil;

  std::vector<WtfParticle> wtfs_{};
  int active_wtf_count_ = 0;

  void init_wtfs(const ViewDimens& dimens);
  void update_wtfs(const FrameStep& step,const ViewDimens& dimens);
  void draw_wtfs(Renderer& ren);
};

} // Namespace.
#endif
