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
#include "cybel/util/timer.h"

#include "assets/assets.h"
#include "map/dantares_map.h"
#include "world/robot.h"
#include "hud.h"
#include "scene_action.h"

#include <filesystem>
#include <functional>
#include <vector>

namespace ekoscape {

class GameScene : public Scene {
public:
  struct State {
    bool show_mini_map = true;
  };

  using StateCallback = std::function<void(const State&)>;

  explicit GameScene(const Assets& assets,const std::filesystem::path& map_file,const State& state
      ,const StateCallback& on_state_changed);

  void init_scene(Renderer& ren) override;
  void on_scene_exit() override;
  void on_key_down_event(SDL_Keycode key) override;
  void handle_key_states(const Uint8* keys) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren) override;

private:
  enum class GamePhase {
    kShowMapInfo,
    kPlay,
    kGameOver,
  };

  static const Color4f kTextBgColor;
  static const Size2i kTextBgPadding;
  static const Duration kMapInfoDuration;
  static const Duration kInitRobotDelay;
  static const int kDantaresDist = 24; // Must be 2+.
  static const float kGameOverLifespan;

  const Assets& assets_;
  State state_{};
  StateCallback on_state_changed_{};
  int scene_action_ = SceneAction::kNil;

  // Classic values: {0.125f,-0.04f,0.04f}.
  //
  // The floor & ceiling heights' signs are swapped, so that the images aren't flipped vertically.
  // See set_space_textures(), which relies on this logic.
  Dantares dantares_{0.125f,0.04f,-0.04f}; // (SquareSize,FloorHeight,CeilingHeight)
  DantaresMap map_{dantares_};

  GamePhase game_phase_ = GamePhase::kShowMapInfo;
  Timer map_info_timer_{};
  std::vector<Robot> robots_{};
  Timer robot_move_timer_{};
  Duration robot_move_duration_{};
  Robot::MoveData robot_move_data_{map_};
  Hud hud_;
  bool player_hit_end_ = false;
  float game_over_age_ = 0.0f;

  void load_map(const std::filesystem::path& file);
  SpaceType init_map_space(const Pos2i& pos,SpaceType type);
  void generate_map();

  void update_player();
  void update_robots(const FrameStep& step);
  void move_robots(const FrameStep& step);

  void draw_map_info(Renderer& ren);
  void draw_game_over(Renderer& ren);

  void set_space_textures(SpaceType type,const Texture* texture);
  void set_space_textures(SpaceType type,const Texture* ceiling,const Texture* wall,const Texture* floor);
};

} // Namespace.
#endif
