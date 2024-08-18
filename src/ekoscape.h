/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_EKOSCAPE_H_
#define EKOSCAPE_EKOSCAPE_H_

#include "core/common.h"

#include "core/cybel_error.h"
#include "core/game_engine.h"
#include "core/scene.h"
#include "scenes/boring_work_scene.h"
#include "scenes/game_scene.h"
#include "scenes/menu_scene.h"
#include "scenes/scene_action.h"
#include "assets.h"

#include <filesystem>

namespace ekoscape {

class EkoScape : public Scene {
public:
  struct Config : public GameEngine::Config {
    int dantares_dist = 24;
  };

  static const std::string kTitle;

  explicit EkoScape(Config config);

  std::shared_ptr<Scene> build_scene(int type);
  void run();
  void on_key_down_event(SDL_Keycode key) override;
  void play_music();

  static void show_error_global(const std::string& error);

private:
  // This must be defined first so that its dtor is called last.
  std::unique_ptr<GameEngine> game_engine_{};

private:
  int dantares_dist_ = 0;
  std::unique_ptr<Assets> assets_{};
  std::filesystem::path map_file_{};
  std::shared_ptr<GameScene> game_scene_{};

  void pop_scene();
};

} // Namespace.
#endif
