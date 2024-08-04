/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_scene.h"

namespace ekoscape {

GameScene::GameScene(Assets& assets,const std::string& map_file,int dantares_dist)
    : assets_(assets),dantares_dist_(dantares_dist) {
  load_map(map_file);
  generate_map();
}

void GameScene::load_map(const std::string& file) {
  map_.load_file(file);

  std::cout << "[INFO] Map file: '" << file << "'\n";
  std::cout << map_ << std::endl;

  map_.add_to_dantares([&](int x,int y,Space& space,SpaceType type) {
    init_map_space(x,y,space,type);
  });
  map_.make_current_in_dantares();
}

void GameScene::init_map_space(int x,int y,Space&,SpaceType type) {
  switch(type) {
    case SpaceType::kRobot:
      robots_.push_back(std::make_unique<RobotNormal>(type,x,y));
      break;

    case SpaceType::kRobotGhost:
      robots_.push_back(std::make_unique<RobotGhost>(type,x,y));
      break;

    case SpaceType::kRobotSnake:
      robots_.push_back(std::make_unique<RobotSnake>(type,x,y));
      break;

    case SpaceType::kRobotStatue:
      robots_.push_back(std::make_unique<RobotStatue>(type,x,y));
      break;

    case SpaceType::kRobotWorm:
      robots_.push_back(std::make_unique<RobotWorm>(type,x,y));
      break;

    default:
      break; // Ignore.
  }
}

void GameScene::generate_map() {
  set_space_textures(SpaceType::kCell,&assets_.ceiling_texture(),&assets_.cell_texture()
      ,&assets_.floor_texture());
  // SpaceType::kDeadSpace - No textures.
  set_space_textures(SpaceType::kEmpty,&assets_.ceiling_texture(),nullptr,&assets_.floor_texture());
  set_space_textures(SpaceType::kEnd,&assets_.end_texture());
  set_space_textures(SpaceType::kEndWall,&assets_.end_texture());
  // SpaceType::kLivingSpace - No textures.
  // SpaceType::kPlayer* - No textures.
  set_space_textures(SpaceType::kRobot,&assets_.robot_texture());
  set_space_textures(SpaceType::kRobotGhost,&assets_.robot_texture());
  set_space_textures(SpaceType::kRobotSnake,&assets_.robot_texture());
  set_space_textures(SpaceType::kRobotStatue,&assets_.robot_texture());
  set_space_textures(SpaceType::kRobotWorm,&assets_.robot_texture());
  set_space_textures(SpaceType::kWall,&assets_.ceiling_texture(),&assets_.wall_texture()
      ,&assets_.floor_texture());
  set_space_textures(SpaceType::kWallGhost,&assets_.ceiling_texture(),&assets_.wall_texture()
      ,&assets_.floor_texture());
  set_space_textures(SpaceType::kWhite,&assets_.white_texture());
  set_space_textures(SpaceType::kWhiteFloor,&assets_.white_texture(),nullptr,&assets_.white_texture());
  set_space_textures(SpaceType::kWhiteGhost,&assets_.white_texture());

  map_.generate_in_dantares(); // Must be called after setting the textures.
}

void GameScene::init_scene() {
  // Delay by an additional second to give some time for the player to orient/adjust.
  robot_move_duration_.set_from_millis(map_.robot_delay().round_millis() + 1000);
  robot_move_timer_.start();
}

void GameScene::handle_key_states(const Uint8* keys) {
  bool is_up = (keys[SDL_SCANCODE_UP] == 1 || keys[SDL_SCANCODE_W] == 1);
  bool is_down = (keys[SDL_SCANCODE_DOWN] == 1 || keys[SDL_SCANCODE_S] == 1);
  bool is_left = (keys[SDL_SCANCODE_LEFT] == 1 || keys[SDL_SCANCODE_A] == 1);
  bool is_right = (keys[SDL_SCANCODE_RIGHT] == 1 || keys[SDL_SCANCODE_D] == 1);

  // Must check Left/Right first, so that the player can turn while holding down Up/Down,
  //     which is an important mechanic for the game.
  if(is_left) {
    if(!is_right) { dantares_.TurnLeft(); }
  } else if(is_right) {
    if(!is_left) { dantares_.TurnRight(); }
  } else if(is_down) { // Check Down first to override Up.
    // By allowing Down to always override Up, the player can use two hands
    //     to quickly back out of a nook, since Up is held down most of the time.
    dantares_.StepBackward();
  } else if(is_up) {
    if(!is_down) { dantares_.StepForward(); }
  } else {
    //dantares_.StepForward(); // TODO: Cruise control? Or just always have on?
  }
}

int GameScene::update_scene_logic(const Duration& last_dpf,double delta_time) {
  SpaceType player_space_type = map_.player_space_type();

  switch(player_space_type) {
    case SpaceType::kCell:
      map_.unlock_cell(map_.player_x(),map_.player_y());
      break;

    case SpaceType::kEnd:
      // TODO: Implement Game Over scene.
      printf("Congratulations!\nYou freed %d Eko(s) out of a total of %d Eko(s).\n\n"
          ,map_.total_rescues(),map_.total_cells());

      if(map_.total_rescues() == map_.total_cells()) {
        // TODO: Implement Secret. Press some key combination to see weird colors, etc.
        std::cout << "You've unlocked a secret!\n";
      }

      return SceneResults::value_of(SceneResult::kMenuScene);

    default:
      if(SpaceTypes::is_robot(player_space_type)) {
        // TODO: Implement Game Over scene.
        printf("You're dead!\nYou freed %d Eko(s) out of a total of %d Eko(s).\n\n"
            ,map_.total_rescues(),map_.total_cells());

        return SceneResults::value_of(SceneResult::kMenuScene);
      }
      break;
  }

  // Remove dead robots and age living robots (only if lifespan was set).
  for(auto it = robots_.begin(); it != robots_.end();) {
    auto& robot = *it;

    if(robot->is_dead()) {
      map_.remove_thing(robot->x(),robot->y());
      it = robots_.erase(it);
    } else {
      robot->age(delta_time);
      ++it;
    }
  }

  // Move robots?
  if(robot_move_timer_.end().duration() >= robot_move_duration_) {
    robot_move_data_.refresh();

    for(auto& robot: robots_) {
      robot->move(robot_move_data_);
    }

    // Add new robots after the move loop, because we can't add new ones inside of its loop.
    for(auto& new_robot: robot_move_data_.new_robots) {
      robots_.push_back(std::move(new_robot));
    }
    robot_move_data_.new_robots.clear();

    // Reset timer.
    robot_move_duration_ = map_.robot_delay() + last_dpf;
    robot_move_timer_.start();
  }

  return SceneResults::value_of(SceneResult::kNil);
}

void GameScene::draw_scene() {
  dantares_.Draw(dantares_dist_);
}

void GameScene::set_space_textures(SpaceType type,const Texture* texture) {
  set_space_textures(type,texture,texture,texture);
}

void GameScene::set_space_textures(SpaceType type,const Texture* ceiling,const Texture* wall
    ,const Texture* floor) {
  int space_id = SpaceTypes::value_of(type);

  // Ceiling & Floor textures are flipped due to using opposite values in Dantares ctor.
  if(ceiling != nullptr) { dantares_.SetCeilingTexture(space_id,floor->id()); }
  if(wall != nullptr) { dantares_.SetWallTexture(space_id,wall->id()); }
  if(floor != nullptr) { dantares_.SetFloorTexture(space_id,ceiling->id()); }
}

} // Namespace.
