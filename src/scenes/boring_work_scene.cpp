/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "boring_work_scene.h"

namespace ekoscape {

BoringWorkScene::BoringWorkScene(CybelEngine& cybel_engine,Assets& assets)
    : cybel_engine_(cybel_engine),assets_(assets) {}

void BoringWorkScene::init_scene(Renderer& /*ren*/) {
  cybel_engine_.set_title("robots.xlsx - EkoOffice Calc");
}

void BoringWorkScene::on_scene_exit() {
  cybel_engine_.reset_title();
}

void BoringWorkScene::on_key_down_event(SDL_Keycode key) {
  switch(key) {
    case SDLK_RETURN:
    case SDLK_SPACE:
    case SDLK_KP_ENTER:
      scene_action_ = SceneAction::kGoBack;
      break;
  }
}

int BoringWorkScene::update_scene_logic(const FrameStep& /*step*/) {
  const int action = scene_action_;
  scene_action_ = SceneAction::kNil;

  return action;
}

void BoringWorkScene::draw_scene(Renderer& ren) {
  ren.begin_2d_scene();

  ren.wrap_sprite(assets_.boring_work_sprite(),[&](auto& s) {
    s.draw_quad({0,0},ren.dimens().size);
  });
}

} // Namespace.
