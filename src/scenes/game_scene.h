/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_GAME_SCENE_H_
#define EKOSCAPE_SCENES_GAME_SCENE_H_

#include "cybel/common.h"

#include "cybel/gfx/texture.h"
#include "cybel/scene/scene.h"
#include "cybel/util/timer.h"

#include "map/dantares_map.h"
#include "world/robot.h"
#include "assets.h"
#include "scene_action.h"

#include <filesystem>
#include <vector>

namespace ekoscape {

class GameScene : public Scene {
public:
  explicit GameScene(const Assets& assets,const std::filesystem::path& map_file,int dantares_dist);

  void init_scene(Renderer& ren) override;
  void on_scene_exit() override;
  void on_key_down_event(SDL_Keycode key) override;
  void handle_key_states(const Uint8* keys) override;
  int update_scene_logic(const FrameStep& step) override;
  void draw_scene(Renderer& ren) override;

private:
  const Assets& assets_;

  // Classic values: {0.125f,-0.04f,0.04f}.
  //
  // The floor & ceiling heights' signs are swapped, so that the images aren't flipped vertically.
  // See set_space_textures(), which relies on this logic.
  Dantares dantares_{0.125f,0.04f,-0.04f}; // (SquareSize,FloorHeight,CeilingHeight)
  int dantares_dist_ = 0;

  DantaresMap map_{dantares_};
  Robot::MoveData robot_move_data_{map_};
  std::vector<std::unique_ptr<Robot>> robots_{};
  Timer robot_move_timer_{};
  Duration robot_move_duration_{};

  void load_map(const std::filesystem::path& file);
  void init_map_space(const Pos2i& pos,Space&,SpaceType type);
  void generate_map();

  int update_player();
  void update_robots(const FrameStep& step);
  void move_robots(const FrameStep& step);

  void set_space_textures(SpaceType type,const Texture* texture);
  void set_space_textures(SpaceType type,const Texture* ceiling,const Texture* wall,const Texture* floor);
};

} // Namespace.
#endif
