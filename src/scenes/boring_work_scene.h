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
#include "cybel/cybel_engine.h"

#include "assets/assets.h"
#include "scene_action.h"

namespace ekoscape {

class BoringWorkScene : public Scene {
public:
  explicit BoringWorkScene(CybelEngine& cybel_engine,Assets& assets);

  void init_scene(Renderer& ren) override;
  void on_scene_exit() override;
  void on_key_down_event(SDL_Keycode key) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren) override;

private:
  CybelEngine& cybel_engine_;
  Assets& assets_;
  int scene_action_ = SceneAction::kNil;
};

} // Namespace.
#endif
