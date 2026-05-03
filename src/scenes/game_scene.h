/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_GAME_SCENE_H_
#define EKOSCAPE_SCENES_GAME_SCENE_H_

#include "common.h"

#include "cybel/gfx/renderer.h"
#include "cybel/gfx/texture.h"
#include "cybel/scene/scene.h"
#include "cybel/scene/scene_context.h"
#include "cybel/types/duration.h"
#include "cybel/types/pos.h"
#include "cybel/util/timer.h"

#include "core/game_session.h"
#include "map/map.h"
#include "map/space_type.h"
#include "scenes/game_hud.h"
#include "scenes/game_overlay.h"
#include "world/robot.h"

#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>

namespace ekoscape {

class GameScene final : public Scene {
public:
  explicit GameScene(GameSession& sesh,Renderer& ren,const std::filesystem::path& map_file);

  void on_scene_enter(SceneContext& ctx) override;
  void on_scene_exit(SceneContext& ctx) override;

  void on_scene_context_restore(SceneContext& ctx) override;

  void on_scene_input_event(input_id_t input_id,SceneContext& ctx) override;
  void handle_scene_input(const InputStates& states,InputMan& input,SceneContext& ctx) override;

  void update_scene_logic(const FrameStep& step,SceneContext& ctx) override;
  void draw_scene(Renderer& ren,SceneContext& ctx) override;

private:
  enum class GamePhase {
    kShowMapInfo,
    kPlay,
    kGameOver,
  };

  struct StoredInputs {
    bool is_down = false;
    bool is_left = false;
    bool is_right = false;
  };

  using MoveChecker = std::function<bool(const Pos3i&)>;

  static inline const Duration kMapInfoDuration = Duration::from_millis(2'500);
  static inline const Duration kInitExtraRobotDelay = Duration::from_millis(1'000);
  static constexpr int kDantaresDist = 24; // Must be 2+.
  static inline const Duration kWarpDuration = Duration::from_millis(750);
  static inline const Duration kFruitDuration = Duration::from_millis(7'000);
  static constexpr int kFruitWarnSecs = 2;

  GameSession& sesh_;

  std::unique_ptr<Dantares2::RendererClass> dantares_renderer_{};
  std::unique_ptr<Dantares2> dantares_{};
  std::unique_ptr<Map> map_{};

  GamePhase game_phase_ = GamePhase::kShowMapInfo;
  Timer map_info_timer_{};

  StoredInputs stored_inputs_{};
  bool player_hit_end_ = false;
  bool player_warped_ = false;
  Duration player_warp_time_{};
  Duration player_fruit_time_{};
  Timer speedrun_timer_{};

  std::vector<Robot> robots_{};
  Duration robot_move_time_{};
  std::unique_ptr<Robot::MoveData> robot_move_data_{};
  std::unordered_map<SpaceType,std::vector<Pos3i>> portal_to_pos_bag_{};

  std::unique_ptr<GameHud> hud_{};
  std::unique_ptr<GameOverlay> overlay_{};

  void init_map(const std::filesystem::path& map_file);
  SpaceType init_map_space(const Pos3i& pos,SpaceType type,std::vector<Pos3i>& cells);
  void init_map_default_empty(const Pos3i& pos,SpaceType type);
  void make_map_weird(std::vector<Pos3i>& cells);
  void init_map_texs();

  void update_player(const FrameStep& step);
  void game_over(bool player_hit_end);

  void update_robots(const FrameStep& step);
  void move_robots(const FrameStep& step);
  void remove_robots_at(const Pos3i& pos);
  std::optional<Pos3i> fetch_portal_bro(const Pos3i& pos,SpaceType portal,const MoveChecker& can_move_to);

  void update_mods(const FrameStep& step,SceneContext& ctx);

  void set_space_texs(SpaceType type,const Texture* tex);
  void set_space_texs(SpaceType type,const Texture* ceiling,const Texture* wall,const Texture* floor);
};

} // namespace ekoscape
#endif
