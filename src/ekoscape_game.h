/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_EKOSCAPE_GAME_H_
#define EKOSCAPE_EKOSCAPE_GAME_H_

#include "common.h"

#include "cybel/cybel_engine.h"
#include "cybel/game.h"
#include "cybel/input/input_man.h"
#include "cybel/scene/scene_context.h"

#include "assets/assets.h"
#include "core/game_session.h"
#include "world/star_sys.h"

namespace ekoscape {

class EkoScapeGame final : public Game {
public:
  static constexpr auto* kTitle = "EkoScape v2.4";

  static CybelEngine::Config build_config();

  explicit EkoScapeGame(CybelEngine& engine);

  void on_game_start(CybelEngine& engine,SceneContext& ctx) override;
  SceneBag build_scene(scene_id_t id,SceneContext& ctx) override;

  void on_scene_input_event(input_id_t input_id,SceneContext& ctx) override;

  void update_scene_logic(const FrameStep& step,SceneContext& ctx) override;
  void draw_scene(Renderer& ren,SceneContext& ctx) override;

private:
  std::shared_ptr<Assets> assets_{};
  GameSession sesh_;

  bool was_music_playing_ = false;
  StarSys star_sys_{};
  float avg_fps_age_ = -1.0f;
  std::string avg_fps_str_{};

  void init_input_map(InputMan& im);

  void quit(const SceneContext& ctx);

  void play_music(const SceneContext& ctx,bool rand_pos = false);
  void stop_music(const SceneContext& ctx,bool going_to_boring_work = false);
};

} // namespace ekoscape
#endif
