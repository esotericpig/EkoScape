/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_MENU_SCENE_H_
#define EKOSCAPE_SCENES_MENU_SCENE_H_

#include "core/common.h"

#include "core/game_engine.h"
#include "core/scene.h"

#include "assets.h"
#include "scene_result.h"

#include <functional>

namespace ekoscape {

class MenuScene : public Scene {
public:
  using TryPlayMapHandler = std::function<void(const std::string& map_file)>;

  MenuScene(GameEngine& game_engine,Assets& assets,TryPlayMapHandler try_play_map);

  void init_scene() override;
  void on_key_down_event(SDL_Keycode key) override;
  int update_scene_logic(FrameStep step) override;
  void draw_scene(Dimens dimens) override;

private:
  GameEngine& game_engine_;
  Assets& assets_;
  TryPlayMapHandler try_play_map_;
  int scene_result_ = SceneResult::kNil;
};

} // Namespace.
#endif
