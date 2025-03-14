/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_GAME_SCENE_H_
#define EKOSCAPE_SCENES_GAME_SCENE_H_

#include "common.h"

#include "cybel/gfx/texture.h"
#include "cybel/scene/scene.h"
#include "cybel/types/duration.h"
#include "cybel/types/pos.h"
#include "cybel/util/timer.h"

#include "map/map.h"
#include "map/space_type.h"
#include "scenes/game_hud.h"
#include "scenes/game_overlay.h"
#include "scenes/scene_action.h"
#include "world/robot.h"
#include "game_context.h"

#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>

namespace ekoscape {

class GameScene : public Scene {
public:
  struct State {
    bool show_mini_map = true;
    bool show_speedrun = true;
  };

  explicit GameScene(GameContext& ctx,State& state,const std::filesystem::path& map_file);

  void init_scene(const ViewDimens& dimens) override;
  void on_scene_exit() override;
  void on_input_event(int action,const ViewDimens& dimens) override;
  void handle_input_states(const std::vector<bool>& states,const ViewDimens& dimens) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren,const ViewDimens& dimens) override;

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

  GameContext& ctx_;
  State& state_;
  int scene_action_ = SceneAction::kNil;

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
  void game_over(bool hit_end);

  void update_robots(const FrameStep& step);
  void move_robots(const FrameStep& step);
  void remove_robots_at(const Pos3i& pos);
  std::optional<Pos3i> fetch_portal_bro(const Pos3i& pos,SpaceType portal,const MoveChecker& can_move_to);

  void set_space_texs(SpaceType type,const Texture* tex);
  void set_space_texs(SpaceType type,const Texture* ceiling,const Texture* wall,const Texture* floor);
};

} // namespace ekoscape
#endif
