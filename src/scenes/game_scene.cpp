/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_scene.h"

namespace ekoscape {

GameScene::GameScene(const Assets& assets,const std::filesystem::path& map_file,int dantares_dist)
    : assets_(assets),dantares_dist_(std::max(dantares_dist,2)) {
  load_map(map_file);
  generate_map();

  // Extra delay to give some time for the player to orient/adjust.
  const auto kExtraDelay = Duration::from_millis(5000);
  robot_move_duration_ = map_.robot_delay() + kExtraDelay;
}

void GameScene::load_map(const std::filesystem::path& file) {
  map_.load_file(file);

  std::cout << "[INFO] Map file: '" << file.string() << "'\n";
  std::cout << map_ << std::endl;

  map_.add_to_dantares([&](const Pos2i& pos,Space& space,SpaceType type) {
    init_map_space(pos,space,type);
  });
  map_.make_current_in_dantares();
}

void GameScene::init_map_space(const Pos2i& pos,Space&,SpaceType type) {
  switch(type) {
    case SpaceType::kRobot:
      robots_.emplace_back(std::make_unique<RobotNormal>(type,pos));
      break;

    case SpaceType::kRobotGhost:
      robots_.emplace_back(std::make_unique<RobotGhost>(type,pos));
      break;

    case SpaceType::kRobotSnake:
      robots_.emplace_back(std::make_unique<RobotSnake>(type,pos));
      break;

    case SpaceType::kRobotStatue:
      robots_.emplace_back(std::make_unique<RobotStatue>(type,pos));
      break;

    case SpaceType::kRobotWorm:
      robots_.emplace_back(std::make_unique<RobotWorm>(type,pos));
      break;

    default: break; // Ignore.
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

void GameScene::init_scene(Renderer& /*ren*/) {
  // If was paused, adjust robot_move_duration_ to be accurate.
  // - For maps that depend on accurate time, like `60 Seconds`, this prevents cheating.
  if(robot_move_timer_.duration().millis() > 0.0) {
    robot_move_duration_ -= robot_move_timer_.duration();

    if(robot_move_duration_.millis() < 0.0) {
      robot_move_duration_.set_from_millis(0.0);
    }
  }

  robot_move_timer_.start();
}

void GameScene::on_scene_exit() {
  robot_move_timer_.end();
}

void GameScene::on_key_down_event(SDL_Keycode /*key*/) {
  // TODO: Use this to press enter for Game Over.
}

void GameScene::handle_key_states(const Uint8* keys) {
  bool is_up = (keys[SDL_SCANCODE_UP] == 1 || keys[SDL_SCANCODE_W] == 1);
  bool is_down = (keys[SDL_SCANCODE_DOWN] == 1 || keys[SDL_SCANCODE_S] == 1);
  bool is_left = (keys[SDL_SCANCODE_LEFT] == 1 || keys[SDL_SCANCODE_A] == 1);
  bool is_right = (keys[SDL_SCANCODE_RIGHT] == 1 || keys[SDL_SCANCODE_D] == 1);

  // TODO: Wild idea. If pressing Left&Right at same time, step left & right? Need a bool flag?

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

int GameScene::update_scene_logic(const FrameStep& step,const ViewDimens& /*dimens*/) {
  int action = update_player();

  if(action != SceneAction::kNil) { return action; }

  update_robots(step);
  move_robots(step);

  return SceneAction::kNil;
}

int GameScene::update_player() {
  SpaceType player_space_type = map_.player_space_type();

  switch(player_space_type) {
    case SpaceType::kCell:
      map_.unlock_cell(map_.player_pos());
      break;

    case SpaceType::kEnd:
      // TODO: Implement Game Over scene.
      printf("Congratulations!\nYou freed %d Eko(s) out of a total of %d Eko(s).\n\n"
          ,map_.total_rescues(),map_.total_cells());

      if(map_.total_rescues() == map_.total_cells()) {
        // TODO: Implement Secret. Press some key combination to see weird colors, etc.
        std::cout << "You've unlocked a secret!\n";
      }
      return SceneAction::kGoBack;

    default:
      if(SpaceTypes::is_robot(player_space_type)) {
        // TODO: Implement Game Over scene.
        printf("You're dead!\nYou freed %d Eko(s) out of a total of %d Eko(s).\n\n"
            ,map_.total_rescues(),map_.total_cells());

        return SceneAction::kGoBack;
      }
      break;
  }

  return SceneAction::kNil;
}

void GameScene::update_robots(const FrameStep& step) {
  // Remove dead robots and age living robots (only if lifespan was set).
  for(auto it = robots_.begin(); it != robots_.end();) {
    auto& robot = *it;

    if(robot->is_dead()) {
      map_.remove_thing(robot->pos());
      it = robots_.erase(it);
    } else {
      robot->age(step.delta_time);
      ++it;
    }
  }
}

void GameScene::move_robots(const FrameStep& step) {
  // Not time to move robots?
  if(robot_move_timer_.end().duration() < robot_move_duration_) { return; }

  // Move robots.
  robot_move_data_.refresh();

  for(auto& robot: robots_) {
    robot->move(robot_move_data_);
  }

  // Add new robots after the move loop, because we can't add new ones inside its loop.
  for(auto& new_robot: robot_move_data_.new_robots) {
    robots_.emplace_back(std::move(new_robot));
  }
  robot_move_data_.new_robots.clear();

  // Reset the move timer.
  robot_move_duration_ = map_.robot_delay() + step.dpf;
  robot_move_timer_.start();
}

void GameScene::draw_scene(Renderer& ren) {
  ren.begin_3d_scene();
  dantares_.Draw(dantares_dist_);
}

void GameScene::set_space_textures(SpaceType type,const Texture* texture) {
  set_space_textures(type,texture,texture,texture);
}

void GameScene::set_space_textures(SpaceType type,const Texture* ceiling,const Texture* wall
    ,const Texture* floor) {
  int space_id = SpaceTypes::value_of(type);

  // Ceiling & Floor textures are flipped due to using opposite values in Dantares ctor.
  if(ceiling != nullptr) { dantares_.SetFloorTexture(space_id,ceiling->gl_id()); }
  if(wall != nullptr) { dantares_.SetWallTexture(space_id,wall->gl_id()); }
  if(floor != nullptr) { dantares_.SetCeilingTexture(space_id,floor->gl_id()); }
}

} // Namespace.
