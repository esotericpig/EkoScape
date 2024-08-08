/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "menu_scene.h"

namespace ekoscape {

MenuScene::MenuScene(Assets& assets)
    : assets_(assets) {}

void MenuScene::init_scene(Renderer& /*ren*/) {}

void MenuScene::on_key_down_event(SDL_Keycode key) {
  if(key == SDLK_RETURN) {
    scene_action_ = SceneAction::kGoToGame;
  }
}

int MenuScene::update_scene_logic(const FrameStep& /*step*/) {
  int action = scene_action_;
  scene_action_ = SceneAction::kNil; // Avoid possible infinite loop.

  return action;
}

void MenuScene::draw_scene(Renderer& ren) {
  ren.begin_2d_scene();
}

} // Namespace.
