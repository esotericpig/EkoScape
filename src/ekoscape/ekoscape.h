/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_EKOSCAPE_H_
#define EKOSCAPE_EKOSCAPE_H_

#include "ekoscape/global.h"

#include "ekoscape/assets/assets.h"
#include "ekoscape/core/game_session.h"
#include "ekoscape/world/star_sys.h"

#include <cybel/game.h>
#include <cybel/chrono/ticker.h>
#include <cybel/scenes/scene_context.h>

#include <memory>
#include <string>

namespace ekoscape {

class EkoScape final : public Game {
public:
  static constexpr const auto* kTitle = "EkoScape v2.4";

  static CybelEngine::Config build_config();

  explicit EkoScape(CybelEngine& engine);

  void on_game_start(CybelEngine& engine,SceneContext& ctx) override;
  SceneBag build_scene(scene_id_t id,SceneContext& ctx) override;

  void on_scene_input_event(input_id_t input_id,SceneContext& ctx) override;

  void update_scene_logic(const FrameDelta& delta,SceneContext& ctx) override;
  void draw_scene(Renderer& ren,SceneContext& ctx) override;

private:
  std::shared_ptr<Assets> assets_{};
  GameSession sesh_;

  bool was_music_playing_ = false;
  StarSys star_sys_{};
  Ticker0f avg_fps_ticker_{1.0f,TickerFlag::kLoop};
  std::string avg_fps_str_{};

  void init_input_map(InputMan& im);

  void quit(const SceneContext& ctx);

  void play_music(const SceneContext& ctx,bool rand_pos = false);
  void stop_music(const SceneContext& ctx,bool going_to_boring_work = false);
};

} // namespace ekoscape
#endif
