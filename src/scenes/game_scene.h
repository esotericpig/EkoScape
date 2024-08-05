/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_GAME_SCENE_H_
#define EKOSCAPE_SCENES_GAME_SCENE_H_

#include "core/common.h"

#include "core/game_engine.h"
#include "core/scene.h"
#include "core/texture.h"
#include "core/timer.h"

#include "map/dantares_map.h"
#include "map/space.h"
#include "map/space_type.h"

#include "world/robot.h"

#include "assets.h"
#include "scene_result.h"

#include <vector>

namespace ekoscape {

class GameScene : public Scene {
public:
  GameScene(GameEngine& game_engine,Assets& assets,const std::string& map_file,int dantares_dist);

  void init_scene() override;
  void handle_key_states(const Uint8* keys) override;
  int update_scene_logic(FrameStep step) override;
  void draw_scene(Dimens dimens) override;

private:
  GameEngine& game_engine_;
  Assets& assets_;

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

  void load_map(const std::string& file);
  void init_map_space(int x,int y,Space&,SpaceType type);
  void generate_map();

  void set_space_textures(SpaceType type,const Texture* texture);
  void set_space_textures(SpaceType type,const Texture* ceiling,const Texture* wall,const Texture* floor);
};

} // Namespace.
#endif
