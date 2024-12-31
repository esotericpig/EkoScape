/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_EKOSCAPE_GAME_H_
#define EKOSCAPE_EKOSCAPE_GAME_H_

#include "common.h"

#include "cybel/scene/scene.h"
#include "cybel/scene/scene_bag.h"
#include "cybel/scene/scene_man.h"
#include "cybel/util/cybel_error.h"
#include "cybel/cybel_engine.h"

#include "assets/assets.h"
#include "scenes/boring_work_scene.h"
#include "scenes/game_scene.h"
#include "scenes/menu_credits_scene.h"
#include "scenes/menu_play_scene.h"
#include "scenes/menu_scene.h"
#include "scenes/scene_action.h"
#include "world/star_sys.h"
#include "game_context.h"

#include <filesystem>

namespace ekoscape {

class EkoScapeGame : public Scene {
private:
  // NOTE: This must be defined first so that its dtor is called last.
  std::unique_ptr<CybelEngine> cybel_engine_{};

public:
  static inline const std::string kTitle = "EkoScape v2.0";

  explicit EkoScapeGame();

  void run();

  void on_key_down_event(const KeyEvent& event,const ViewDimens& dimens) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren,const ViewDimens& dimens) override;

  void show_error(const std::string& error);
  static void show_error_global(const std::string& error);

private:
  SceneMan* scene_man_ = nullptr;
  bool was_music_playing_ = false;
  std::unique_ptr<Assets> assets_{};
  std::unique_ptr<GameContext> ctx_{};

  StarSys star_sys_{};
  float avg_fps_age_{};
  double avg_fps_ = -1.0;
  StrUtf8 avg_fps_str_{};

  MenuPlayScene::State menu_play_scene_state_{};
  GameScene::State game_scene_state_{};

  void play_music();
  void stop_music(bool on_boring_work = false);

  SceneBag build_scene(int type);
  void pop_scene();
};

} // Namespace.
#endif
