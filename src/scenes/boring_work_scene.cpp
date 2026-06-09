/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "boring_work_scene.h"

#include "cybel/scene/scene_context.h"

#include "assets/asset_ids.h"
#include "core/input_action.h"

namespace ekoscape {

void BoringWorkScene::on_scene_enter(SceneContext& ctx) {
  ctx.engine.set_title("robots.xlsx - EkoOffice Calc");

  if(!ctx.engine.is_cursor_visible()) {
    ctx.engine.set_cursor_visible(true);
  }
}

void BoringWorkScene::on_scene_exit(SceneContext& ctx) {
  ctx.engine.reset_title();

  if(ctx.engine.is_fullscreen()) {
    ctx.engine.set_cursor_visible(false);
  }
}

void BoringWorkScene::on_scene_input_event(input_id_t input_id,SceneContext& ctx) {
  switch(static_cast<InputAction>(input_id)) {
    case InputAction::kSelect:
      ctx.scenes.pop_scene();
      break;

    default: break;
  }
}

void BoringWorkScene::draw_scene(Renderer& ren,SceneContext& ctx) {
  ren.begin_2d_scene();

  const auto& boring_work = ctx.assets.sprite(SpriteId::kBoringWork);

#if defined(__EMSCRIPTEN__)
  ren.begin_auto_center_scale();
  ren.wrap_sprite(boring_work,[&](auto& s) {
    s.draw_quad(Pos3i{0,0,0},ctx.dimens.target_size);
  });
  ren.end_scale();
#else
  ren.wrap_sprite(boring_work,[&](auto& s) {
    s.draw_quad(Pos3i{0,0,0},ctx.dimens.size);
  });
#endif
}

} // namespace ekoscape
