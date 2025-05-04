/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_BORING_WORK_SCENE_H_
#define EKOSCAPE_SCENES_BORING_WORK_SCENE_H_

#include "common.h"

#include "cybel/scene/scene.h"

#include "core/game_context.h"
#include "scenes/scene_action.h"

namespace ekoscape {

class BoringWorkScene : public Scene {
public:
  explicit BoringWorkScene(GameContext& ctx) noexcept;

  void init_scene(const ViewDimens& dimens) override;
  void on_scene_exit() override;
  void on_scene_input_event(input_id_t input_id,const ViewDimens& dimens) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren,const ViewDimens& dimens) override;

private:
  GameContext& ctx_;
  int scene_action_ = SceneAction::kNil;
};

} // namespace ekoscape
#endif
