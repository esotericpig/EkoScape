/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_EKOSCAPE_H_
#define EKOSCAPE_EKOSCAPE_H_

#include "cybel/common.h"

#include "cybel/scene/scene.h"
#include "cybel/scene/scene_bag.h"
#include "cybel/scene/scene_man.h"
#include "cybel/util/cybel_error.h"
#include "cybel/game_engine.h"

#include "scenes/boring_work_scene.h"
#include "scenes/game_scene.h"
#include "scenes/menu_scene.h"
#include "scenes/scene_action.h"
#include "assets.h"

#include <filesystem>

namespace ekoscape {

class EkoScape : public Scene {
private:
  // NOTE: This must be defined first so that its dtor is called last.
  std::unique_ptr<GameEngine> game_engine_{};

public:
  struct Config : public GameEngine::Config {
    int dantares_dist = 24;
  };

  static const std::string kTitle;

  explicit EkoScape(Config config);

  void run();

  void on_key_down_event(SDL_Keycode key) override;
  int update_scene_logic(const FrameStep& step) override;
  void draw_scene(Renderer& ren) override;

  void play_music();

  static void show_error_global(const std::string& error);

private:
  int dantares_dist_ = 0;
  SceneMan* scene_man_ = nullptr;
  std::unique_ptr<Assets> assets_{};
  std::filesystem::path map_file_{};

  SceneBag build_scene(int type);
  void pop_scene();
};

} // Namespace.
#endif
