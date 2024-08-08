/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "menu_scene.h"

namespace ekoscape {

MenuScene::MenuScene(GameEngine& game_engine,Assets& assets,TryPlayMapHandler try_play_map)
    : game_engine_(game_engine),assets_(assets),try_play_map_(try_play_map) {}

void MenuScene::init_scene(Renderer& /*ren*/) {}

void MenuScene::on_key_down_event(SDL_Keycode key) {
  if(key == SDLK_RETURN) {
    try {
      try_play_map_("assets/maps/classic/tron.txt");
      scene_result_ = SceneResult::kNextScene;
    } catch(const EkoScapeError& e) {
      game_engine_.show_error(e.what());
    }
  }
}

int MenuScene::update_scene_logic(const FrameStep& /*step*/) {
  int result = scene_result_;
  scene_result_ = SceneResult::kNil; // Avoid possible infinite loop.

  return result;
}

void MenuScene::draw_scene(Renderer& ren) {
  ren.begin_2d_scene();
}

} // Namespace.
