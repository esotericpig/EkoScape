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

#include "map/dantares_map.h"
#include "world/robot.h"
#include "game_context.h"
#include "game_hud.h"
#include "game_overlay.h"
#include "scene_action.h"

#include <filesystem>
#include <functional>
#include <ranges>
#include <unordered_map>
#include <vector>

namespace ekoscape {

class GameScene : public Scene {
public:
  struct State {
    bool show_mini_map = true;
  };

  using StateCallback = std::function<void(const State&)>;

  explicit GameScene(GameContext& ctx,const std::filesystem::path& map_file,const State& state
      ,const StateCallback& on_state_changed);

  void on_key_down_event(const KeyEvent& event,const ViewDimens& dimens) override;
  void handle_key_states(const KeyStates& keys,const ViewDimens& dimens) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren,const ViewDimens& dimens) override;

private:
  enum class GamePhase {
    kShowMapInfo,
    kPlay,
    kGameOver,
  };

  struct StoredKeyStates {
    bool is_down = false;
    bool is_left = false;
    bool is_right = false;
  };

  using MoveChecker = std::function<bool(const Pos3i&)>;

  static inline const Duration kInitExtraRobotDelay = Duration::from_millis(1'000);
  static inline const int kDantaresDist = 24; // Must be 2+.
  static inline const Duration kWarpDuration = Duration::from_millis(750);
  static inline const Duration kFruitDuration = Duration::from_millis(7'000);
  static inline const int kFruitWarnSecs = 2;

  GameContext& ctx_;
  State state_{};
  StateCallback on_state_changed_{};
  int scene_action_ = SceneAction::kNil;

  void init_map_texs();

  // - Classic values: {0.125f,-0.04f,0.04f}.
  // - The floor & ceiling heights' signs are swapped, so that the images aren't flipped vertically.
  //   - See set_space_texs(), which relies on this logic.
  Dantares dantares_{0.125f,0.04f,-0.04f}; // (SquareSize,FloorHeight,CeilingHeight)
  DantaresMap map_{dantares_,[&](Dantares& /*dan*/,int /*z*/,int /*grid_id*/) { init_map_texs(); }};

  GamePhase game_phase_ = GamePhase::kShowMapInfo;
  StoredKeyStates stored_keys_{};
  bool player_hit_end_ = false;
  bool player_warped_ = false;
  Duration player_warp_time_{};
  Duration player_fruit_time_{};

  std::vector<Robot> robots_{};
  Duration robot_move_time_{};
  Robot::MoveData robot_move_data_{map_};

  std::unordered_map<SpaceType,std::vector<Pos3i>> portal_to_pos_bag_{};

  GameHud hud_;
  GameOverlay overlay_;

  SpaceType init_map_space(const Pos3i& pos,SpaceType type);
  void init_map_default_empty(const Pos3i& pos,SpaceType type);

  void update_player(const FrameStep& step);
  void game_over(bool hit_end);

  void update_robots(const FrameStep& step);
  void move_robots(const FrameStep& step);
  void remove_robots_at(const Pos3i& pos);

  std::optional<Pos3i> fetch_portal_bro(const Pos3i& pos,SpaceType portal,const MoveChecker& can_move_to);

  void set_space_texs(SpaceType type,const Texture* tex);
  void set_space_texs(SpaceType type,const Texture* ceiling,const Texture* wall,const Texture* floor);
};

} // Namespace.
#endif
