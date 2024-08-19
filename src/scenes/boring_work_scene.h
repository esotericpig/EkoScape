/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_BORING_WORK_SCENE_H_
#define EKOSCAPE_SCENES_BORING_WORK_SCENE_H_

#include "core/common.h"

#include "core/scene/scene.h"
#include "core/game_engine.h"
#include "assets.h"
#include "scene_action.h"

namespace ekoscape {

class BoringWorkScene : public Scene {
public:
  BoringWorkScene(GameEngine& game_engine,Assets& assets);

  void init_scene(Renderer& ren) override;
  void on_scene_exit() override;
  void on_key_down_event(SDL_Keycode key) override;
  int update_scene_logic(const FrameStep& step) override;
  void draw_scene(Renderer& ren) override;

private:
  GameEngine& game_engine_;
  Assets& assets_;
};

} // Namespace.
#endif
