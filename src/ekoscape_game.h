/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_EKOSCAPE_GAME_H_
#define EKOSCAPE_EKOSCAPE_GAME_H_

#include "cybel/common.h"

#include "cybel/scene/scene.h"
#include "cybel/scene/scene_bag.h"
#include "cybel/scene/scene_man.h"
#include "cybel/util/cybel_error.h"
#include "cybel/cybel_engine.h"

#include "scenes/boring_work_scene.h"
#include "scenes/game_scene.h"
#include "scenes/menu_credits_scene.h"
#include "scenes/menu_play_scene.h"
#include "scenes/menu_scene.h"
#include "scenes/scene_action.h"
#include "world/star_sys.h"
#include "assets.h"

#include <filesystem>

namespace ekoscape {

class EkoScapeGame : public Scene {
private:
  // NOTE: This must be defined first so that its dtor is called last.
  std::unique_ptr<CybelEngine> cybel_engine_{};

public:
  static const std::string kTitle;

  explicit EkoScapeGame();

  void run();

  void on_key_down_event(SDL_Keycode key) override;
  int update_scene_logic(const FrameStep& step) override;
  void draw_scene(Renderer& ren) override;

  void play_music();
  void select_map_file(const std::filesystem::path& file,bool is_rand);

  static void show_error_global(const std::string& error);

private:
  static const int kDantaresDist;

  SceneMan* scene_man_ = nullptr;
  std::unique_ptr<Assets> assets_{};
  StarSys star_sys_{};

  std::filesystem::path map_file_{};
  bool is_rand_map_ = false;

  SceneBag build_scene(int type);
  void pop_scene();
};

} // Namespace.
#endif
