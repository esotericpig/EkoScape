/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_scene.h"

namespace ekoscape {

GameScene::GameScene(Assets& assets,const std::filesystem::path& map_file,const State& state
    ,const StateCallback& on_state_changed)
    : assets_(assets),state_(state),on_state_changed_(on_state_changed),hud_(assets),overlay_(assets) {
  load_map(map_file);

  // Extra delay to give some time for the player to orient/adjust.
  robot_move_duration_ = map_.robot_delay() + kInitExtraRobotDelay;
}

void GameScene::load_map(const std::filesystem::path& file) {
  map_.load_file(file
    ,[&](const auto& pos,SpaceType type) { return init_map_space(pos,type); }
    ,[&](const auto& pos,SpaceType type) { init_map_default_empty(pos,type); }
  );

  std::cout << "[INFO] Map file: '" << file.string() << "'\n"
            << map_ << std::endl;

  map_.add_to_dantares([&](auto& /*dan*/,int /*z*/,int /*id*/) {
    init_map_textures();
  });
}

SpaceType GameScene::init_map_space(const Pos3i& pos,SpaceType type) {
  // For weird, flip Robots & Cells.
  if(assets_.is_weird()) {
    if(type == SpaceType::kCell) {
      type = SpaceType::kRobot;
    } else if(SpaceTypes::is_robot(type)) {
      type = SpaceType::kCell;
    }
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

    default:
      if(SpaceTypes::is_portal(type)) {
        portal_to_pos_bag_[type].emplace_back(pos);
      }
      break;
  }

  return type;
}

void GameScene::init_map_default_empty(const Pos3i& pos,SpaceType type) {
  if(SpaceTypes::is_portal(type)) {
    portal_to_pos_bag_[type].emplace_back(pos);
  }
}

void GameScene::init_map_textures() {
  set_space_textures(SpaceType::kCell,&assets_.ceiling_texture(),&assets_.cell_texture()
      ,&assets_.floor_texture());
  // SpaceType::kDeadSpace - No textures.
  set_space_textures(SpaceType::kEmpty,&assets_.ceiling_texture(),nullptr,&assets_.floor_texture());
  set_space_textures(SpaceType::kEnd,&assets_.end_texture());
  set_space_textures(SpaceType::kEndWall,&assets_.end_texture());
  // SpaceType::kLivingSpace - No textures.
  // SpaceType::kPlayer* - No textures.
  set_space_textures(SpaceType::kPortal0,&assets_.portal_texture());
  set_space_textures(SpaceType::kPortal1,&assets_.portal_texture());
  set_space_textures(SpaceType::kPortal2,&assets_.portal_texture());
  set_space_textures(SpaceType::kPortal3,&assets_.portal_texture());
  set_space_textures(SpaceType::kPortal4,&assets_.portal_texture());
  set_space_textures(SpaceType::kPortal5,&assets_.portal_texture());
  set_space_textures(SpaceType::kPortal6,&assets_.portal_texture());
  set_space_textures(SpaceType::kPortal7,&assets_.portal_texture());
  set_space_textures(SpaceType::kPortal8,&assets_.portal_texture());
  set_space_textures(SpaceType::kPortal9,&assets_.portal_texture());
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
}

void GameScene::init_scene(Renderer& ren) {
  // For timers, use resume() so that it's adjusted correctly after being paused.
  // - For maps that depend on accurate time, like `60 Seconds`, this prevents cheating.

  overlay_.init(ren.dimens());
  robot_move_timer_.resume();
  fruit_timer_.resume();
}

void GameScene::on_scene_exit() {
  robot_move_timer_.end();
  fruit_timer_.end();
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
  // TODO: Fix left/right not being more responsive.

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
      robot_move_duration_ += step.dpf;
      robot_move_timer_.start();
      break;

    case GamePhase::kPlay:
      break;

    case GamePhase::kGameOver:
      overlay_.update_game_over(step,player_hit_end_);
      break;
  }

  update_player(step);
  update_robots(step);
  move_robots(step);

  return SceneAction::kNil;
}

void GameScene::update_player(const FrameStep& step) {
  if(game_phase_ == GamePhase::kGameOver) { return; }

  if(player_ate_fruit_ && fruit_timer_.end().duration() > fruit_duration_) {
    player_ate_fruit_ = false;
  }

  const Pos3i player_pos = map_.player_pos();
  SpaceType player_space_type = map_.player_space_type();

  switch(player_space_type) {
    case SpaceType::kCell:
      map_.remove_thing(player_pos);
      break;

    // Check for End before Robots & Portals.
    case SpaceType::kEnd:
      game_over(true);
      return;

    // Check for Fruit before Robots.
    case SpaceType::kFruit:
      map_.remove_thing(player_pos);
      player_ate_fruit_ = true;
      fruit_duration_ = Duration::from_millis(7'000) + step.dpf;
      fruit_timer_.start();
      break;

    default: break;
  }

  // The previous logic above might have updated the type/empty, so check/recheck here.
  const Space* player_space = map_.player_space();
  SpaceType player_empty_type = SpaceType::kEmpty;
  player_space_type = map_.player_space_type();

  if(player_space == nullptr) {
    std::cerr << "[ERROR] Player space is null for some reason." << std::endl;
  } else {
    player_empty_type = player_space->empty_type();
  }

  // Portals are like safe zones, so if the Player & a Robot are on a portal, the Player shouldn't die.
  //     Therefore, we check for Portals first.
  if(SpaceTypes::is_portal(player_empty_type)) {
    if(!player_warped_) {
      const Pos3i* portal_bro = fetch_portal_bro(player_pos,player_empty_type,[&](const auto& pos) {
        const Space* space = map_.space(pos);
        return space != nullptr && !space->has_thing();
      });

      if(portal_bro != nullptr) {
        map_.set_player_pos(*portal_bro);
        player_warped_ = true;
      }
    }

    return;
  }

  player_warped_ = false;

  if(SpaceTypes::is_robot(player_space_type)) {
    if(player_ate_fruit_) {
      remove_robots_at(player_pos);
    } else {
      game_over(false);
    }
  }
}

void GameScene::game_over(bool hit_end) {
  game_phase_ = GamePhase::kGameOver;
  player_hit_end_ = hit_end;

  // Because of how high speeds are handled, we need to manually sync the correct player pos,
  //     since the pos might be beyond End, etc. after fully moving.
  map_.set_player_pos();
}

void GameScene::update_robots(const FrameStep& step) {
  // Remove dead Robots and age living Robots (only if lifespan was set).
  for(auto it = robots_.begin(); it != robots_.end();) {
    Robot& robot = *it;

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
  // Not time to move Robots?
  if(robot_move_timer_.end().duration() < robot_move_duration_) { return; }

  // Move Robots.
  robot_move_data_.refresh(player_ate_fruit_);

  for(auto& robot: robots_) {
    robot.move(robot_move_data_);

    // Warp Robots on portals.
    if(robot.portal_type() != SpaceType::kNil && !robot.warped()) {
      const Pos3i* portal_bro = fetch_portal_bro(robot.pos(),robot.portal_type(),[&](const auto& pos) {
        return robot.can_move_to(map_.space(pos));
      });

      if(portal_bro != nullptr) { robot.warp_to(robot_move_data_,*portal_bro); }
    }
  }

  // Add new Robots after the move loop, because we can't add new ones inside its loop.
  for(auto& new_robot: robot_move_data_.new_robots) {
    robots_.emplace_back(new_robot);
  }
  robot_move_data_.new_robots.clear();

  // Reset the move timer.
  robot_move_duration_ = map_.robot_delay() + step.dpf;
  robot_move_timer_.start();
}

void GameScene::remove_robots_at(const Pos3i& pos) {
  map_.remove_thing(pos);

  auto result = std::ranges::remove_if(robots_,[&](const auto& robot) {
    return robot.pos() == pos;
  });
  robots_.erase(result.begin(),result.end());
}

const Pos3i* GameScene::fetch_portal_bro(const Pos3i& pos,SpaceType portal,const MoveChecker& can_move_to) {
  auto it = portal_to_pos_bag_.find(portal);
  if(it == portal_to_pos_bag_.end()) { return nullptr; }

  auto& bros = it->second;
  if(bros.size() <= 1) { return nullptr; } // No bros. :(

  if(bros.size() > 2) { // More than 1 bro?
    Rando::it().shuffle(bros.begin(),bros.end());
  }

  for(auto& bro_pos: bros) { // Find Luigi.
    if(bro_pos != pos && can_move_to(bro_pos)) { return &bro_pos; }
  }

  return nullptr;
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
  const int space_id = SpaceTypes::value_of(type);

  // Ceiling & Floor textures are flipped due to using opposite values in Dantares ctor.
  if(ceiling != nullptr) { dantares_.SetFloorTexture(space_id,ceiling->gl_id()); }
  if(wall != nullptr) { dantares_.SetWallTexture(space_id,wall->gl_id()); }
  if(floor != nullptr) { dantares_.SetCeilingTexture(space_id,floor->gl_id()); }
}

} // Namespace.
