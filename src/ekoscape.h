/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_EKOSCAPE_H_
#define EKOSCAPE_EKOSCAPE_H_

#include "core/common.h"

#include "core/game_engine.h"
#include "core/scene.h"

#include "scenes/game_scene.h"
#include "scenes/menu_scene.h"
#include "scenes/scene_result.h"

#include "assets.h"

namespace ekoscape {

class EkoScape : public Scene {
private:
  // Define this first so that its dtor is called last.
  std::unique_ptr<GameEngine> game_engine_{};

public:
  struct Config : public GameEngine::Config {
    int dantares_dist = 24;
  };

  static const std::string kTitle;

  EkoScape(Config config);

  void play_music();
  void run();

  void init_scene(Renderer& ren) override;
  void resize_scene(Renderer& ren,const ViewDimens& dimens) override;
  void on_key_down_event(SDL_Keycode key) override;
  void handle_key_states(const Uint8* keys) override;
  int update_scene_logic(const FrameStep& step) override;
  void draw_scene(Renderer& ren) override;

  static void show_error_global(const std::string& error);

private:
  int dantares_dist_ = 0;

  std::unique_ptr<Assets> assets_{};
  std::unique_ptr<Scene> current_scene_{};
  std::unique_ptr<Scene> next_scene_{};

  std::unique_ptr<MenuScene> build_menu_scene();
  std::unique_ptr<GameScene> build_game_scene(const std::string& map_file);
};

} // Namespace.
#endif
