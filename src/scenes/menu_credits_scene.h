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

#include "assets/assets.h"
#include "scene_action.h"

#include <vector>

namespace ekoscape {

class MenuCreditsScene : public Scene {
public:
  static Color4f rand_color();

  explicit MenuCreditsScene(Assets& assets);

  void init_scene(Renderer& ren) override;
  void on_key_down_event(SDL_Keycode key) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren) override;

private:
  class WtfParticle : public Particle {
  public:
    Pos3f true_pos{};
    Size2f true_size{};
  };

  Assets& assets_;
  int scene_action_ = SceneAction::kNil;

  ViewDimens view_dimens_{};
  tiny_utf8::string wtf_text_{"wtf!?"};
  std::vector<WtfParticle> wtfs_{};
  int active_wtf_count_ = 0;

  void init_wtfs();
  void update_wtfs(const FrameStep& step);
  void draw_wtfs(Renderer& ren);
};

} // Namespace.
#endif
