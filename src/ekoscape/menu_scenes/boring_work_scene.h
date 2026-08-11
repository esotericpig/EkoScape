/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MENU_SCENES_BORING_WORK_SCENE_H_
#define EKOSCAPE_MENU_SCENES_BORING_WORK_SCENE_H_

#include "ekoscape/global.h"

#include <cybel/scenes/scene.h>

namespace ekoscape {

class BoringWorkScene final : public Scene {
public:
  void on_scene_enter(SceneContext& ctx) override;
  void on_scene_exit(SceneContext& ctx) override;

  void on_scene_input_event(input_id_t input_id,SceneContext& ctx) override;

  void draw_scene(Renderer& ren,SceneContext& ctx) override;
};

} // namespace ekoscape
#endif
