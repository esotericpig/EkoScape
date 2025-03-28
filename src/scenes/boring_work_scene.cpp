/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "boring_work_scene.h"

#include "input/input_action.h"

namespace ekoscape {

BoringWorkScene::BoringWorkScene(GameContext& ctx) noexcept
  : ctx_(ctx) {}

void BoringWorkScene::init_scene(const ViewDimens& /*dimens*/) {
  ctx_.cybel_engine.set_title("robots.xlsx - EkoOffice Calc");

  if(!ctx_.cybel_engine.is_cursor_visible()) {
    ctx_.cybel_engine.set_cursor_visible(true);
  }
}

void BoringWorkScene::on_scene_exit() {
  ctx_.cybel_engine.reset_title();

  if(ctx_.cybel_engine.is_fullscreen()) {
    ctx_.cybel_engine.set_cursor_visible(false);
  }
}

void BoringWorkScene::on_input_event(int action,const ViewDimens& /*dimens*/) {
  switch(action) {
    case InputAction::kSelect:
      scene_action_ = SceneAction::kGoBack;
      break;
  }
}

int BoringWorkScene::update_scene_logic(const FrameStep& /*step*/,const ViewDimens& /*dimens*/) {
  return std::exchange(scene_action_,SceneAction::kNil);
}

void BoringWorkScene::draw_scene(Renderer& ren,const ViewDimens& dimens) {
  ren.begin_2d_scene();

#if defined(__EMSCRIPTEN__)
  ren.begin_auto_center_scale();
  ren.wrap_sprite(ctx_.assets.boring_work_sprite(),[&](auto& s) {
    s.draw_quad(Pos3i{0,0,0},dimens.target_size);
  });
  ren.end_scale();
#else
  ren.wrap_sprite(ctx_.assets.boring_work_sprite(),[&](auto& s) {
    s.draw_quad(Pos3i{0,0,0},dimens.size);
  });
#endif
}

} // namespace ekoscape
