/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "boring_work_scene.h"

namespace ekoscape {

BoringWorkScene::BoringWorkScene(Assets& assets)
    : assets_(assets) {}

void BoringWorkScene::on_key_down_event(SDL_Keycode /*key*/) {
}

int BoringWorkScene::update_scene_logic(const FrameStep& /*step*/) {
  return SceneAction::kNil;
}

void BoringWorkScene::draw_scene(Renderer& ren) {
  ren.begin_2d_scene();

  ren.wrap_sprite(assets_.boring_work_sprite(),[&](auto& s) {
    s.draw_quad(0,0,ren.dimens().size.w,ren.dimens().size.h);
  });
}

} // Namespace.
