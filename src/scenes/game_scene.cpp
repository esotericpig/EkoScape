/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_scene.h"

namespace ekoscape {

const Duration GameScene::kInitRobotDelay = Duration::from_millis(1'000);

GameScene::GameScene(const Assets& assets,const std::filesystem::path& map_file,const State& state
    ,const StateCallback& on_state_changed)
    : assets_(assets),state_(state),on_state_changed_(on_state_changed),hud_(assets),overlay_(assets) {
  load_map(map_file);
  generate_map();

  // Extra delay to give some time for the player to orient/adjust.
  robot_move_duration_ = map_.robot_delay() + kInitRobotDelay;
}

void GameScene::load_map(const std::filesystem::path& file) {
  map_.load_file(file,[&](const Pos2i& pos,SpaceType type) {
    return init_map_space(pos,type);
  });

  std::cout << "[INFO] Map file: '" << file.string() << "'\n"
            << map_ << std::endl;

  map_.add_to_dantares();
  map_.make_current_in_dantares();
}

SpaceType GameScene::init_map_space(const Pos2i& pos,SpaceType type) {
  // For weird, flip robots & cells.
  if(assets_.is_weird()) {
    if(SpaceTypes::is_robot(type)) {
      type = SpaceType::kCell;
    } else if(type == SpaceType::kCell) {
      type = SpaceType::kRobot;
      robots_.emplace_back(Robot::build_normal(pos));
    }

    return type;
  }

  switch(type) {
    case SpaceType::kRobot:
      robots_.emplace_back(Robot::build_normal(pos));
      break;

    case SpaceType::kRobotGhost:
      robots_.emplace_back(Robot::build_ghost(pos));
      break;

    case SpaceType::kRobotSnake:
      robots_.emplace_back(Robot::build_snake(pos));
      break;

    case SpaceType::kRobotStatue:
      robots_.emplace_back(Robot::build_statue(pos));
      break;

    case SpaceType::kRobotWorm:
      robots_.emplace_back(Robot::build_worm(pos));
      break;

    default: break; // Ignore.
  }

  return type;
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

void GameScene::init_scene(Renderer& ren) {
  // If was paused, adjust robot_move_duration_ to be accurate.
  // - For maps that depend on accurate time, like `60 Seconds`, this prevents cheating.
  if(robot_move_timer_.duration() > Duration::kZero) {
    robot_move_duration_ -= robot_move_timer_.duration();
    if(robot_move_duration_ < Duration::kZero) { robot_move_duration_.zero(); }
  }

  overlay_.init(ren.dimens());
  robot_move_timer_.start();
}

void GameScene::on_scene_exit() {
  robot_move_timer_.end();
}

void GameScene::on_key_down_event(SDL_Keycode key) {
  switch(key) {
    // Toggle mini map.
    case SDLK_m:
      state_.show_mini_map = !state_.show_mini_map;
      on_state_changed_(state_);
      break;

    case SDLK_RETURN:
    case SDLK_SPACE:
    case SDLK_KP_ENTER:
      if(game_phase_ == GamePhase::kGameOver) { scene_action_ = SceneAction::kGoBack; }
      break;
  }
}

void GameScene::handle_key_states(const Uint8* keys) {
  // bool is_up = (keys[SDL_SCANCODE_UP] == 1 || keys[SDL_SCANCODE_W] == 1);
  bool is_down = (keys[SDL_SCANCODE_DOWN] == 1 || keys[SDL_SCANCODE_S] == 1);
  bool is_left = (keys[SDL_SCANCODE_LEFT] == 1 || keys[SDL_SCANCODE_A] == 1);
  bool is_right = (keys[SDL_SCANCODE_RIGHT] == 1 || keys[SDL_SCANCODE_D] == 1);

  // TODO: Wild idea. If pressing Left&Right at same time, step left & right? Need a bool flag?

  // Must check Left/Right first, so that the player can turn while holding down Up/Down,
  //     which is an important mechanic for the game.
  if(is_left) {
    if(!is_right) { dantares_.TurnLeft(); }
  } else if(is_right) {
    dantares_.TurnRight();
  } else if(game_phase_ == GamePhase::kPlay) {
    if(is_down) {
      dantares_.StepBackward();
    } else {
      dantares_.StepForward(); // Always keep moving forward.
    }
  }
}

int GameScene::update_scene_logic(const FrameStep& step,const ViewDimens& /*dimens*/) {
  if(scene_action_ != SceneAction::kNil) {
    const int action = scene_action_;
    scene_action_ = SceneAction::kNil;

    return action;
  }

  switch(game_phase_) {
    case GamePhase::kShowMapInfo:
      if(!overlay_.update_map_info()) { return SceneAction::kNil; }

      game_phase_ = GamePhase::kPlay;
      robot_move_timer_.start();
      break;

    case GamePhase::kPlay:
      break;

    case GamePhase::kGameOver:
      overlay_.update_game_over(step,player_hit_end_);
      break;
  }

  update_player();
  update_robots(step);
  move_robots(step);

  return SceneAction::kNil;
}

void GameScene::update_player() {
  if(game_phase_ == GamePhase::kGameOver) { return; }

  const SpaceType player_space_type = map_.player_space_type();

  switch(player_space_type) {
    case SpaceType::kCell:
      map_.unlock_cell(map_.player_pos());
      break;

    default:
      const bool hit_end = (player_space_type == SpaceType::kEnd);

      if(hit_end || SpaceTypes::is_robot(player_space_type)) {
        game_phase_ = GamePhase::kGameOver;
        player_hit_end_ = hit_end;

        // Because of how high speeds are handled, we need to manually reset the correct player pos,
        //     since the pos might be beyond End, etc. after fully moving.
        map_.set_player_pos();
      }
      break;
  }
}

void GameScene::update_robots(const FrameStep& step) {
  // Remove dead robots and age living robots (only if lifespan was set).
  for(auto it = robots_.begin(); it != robots_.end();) {
    auto& robot = *it;

    if(robot.is_dead()) {
      map_.remove_thing(robot.pos());
      it = robots_.erase(it);
    } else {
      robot.age(step.delta_time);
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
    robot.move(robot_move_data_);
  }

  // Add new robots after the move loop, because we can't add new ones inside its loop.
  for(auto& new_robot: robot_move_data_.new_robots) {
    robots_.emplace_back(new_robot);
  }
  robot_move_data_.new_robots.clear();

  // Reset the move timer.
  robot_move_duration_ = map_.robot_delay() + step.dpf;
  robot_move_timer_.start();
}

void GameScene::draw_scene(Renderer& ren) {
  ren.begin_3d_scene();

  if(player_hit_end_) {
    ren.begin_color({1.0f,1.0f - overlay_.game_over_age()});
    dantares_.Draw(kDantaresDist);
    ren.end_color();
  } else {
    dantares_.Draw(kDantaresDist);
  }

  ren.begin_2d_scene();
  hud_.draw(ren,map_,state_.show_mini_map,player_hit_end_);

  switch(game_phase_) {
    case GamePhase::kShowMapInfo:
      overlay_.draw_map_info(ren,map_);
      break;

    case GamePhase::kPlay:
      break;

    case GamePhase::kGameOver:
      overlay_.draw_game_over(ren,map_,player_hit_end_);
      break;
  }
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
