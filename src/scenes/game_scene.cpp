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
  map_.load_file(map_file
    ,[&](const auto& pos,SpaceType type) { return init_map_space(pos,type); }
    ,[&](const auto& pos,SpaceType type) { init_map_default_empty(pos,type); }
  );

  std::cout << "[INFO] Map file ['" << map_file.string() << "'] with [" << map_.grid_count() << "] grid(s):\n"
            << map_ << std::endl;

  map_.add_to_bridge();

  // Extra delay to give some time for the Player to initially orient/adjust.
  robot_move_time_ = map_.robot_delay() + kInitExtraRobotDelay;
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
      robots_.push_back(Robot::build_normal(pos));
      break;

    case SpaceType::kRobotGhost:
      robots_.push_back(Robot::build_ghost(pos));
      break;

    case SpaceType::kRobotSnake:
      robots_.push_back(Robot::build_snake(pos));
      break;

    case SpaceType::kRobotStatue:
      robots_.push_back(Robot::build_statue(pos));
      break;

    case SpaceType::kRobotWorm:
      robots_.push_back(Robot::build_worm(pos));
      break;

    default:
      if(SpaceTypes::is_portal(type)) {
        portal_to_pos_bag_[type].push_back(pos);
      }
      break;
  }

  return type;
}

void GameScene::init_map_default_empty(const Pos3i& pos,SpaceType type) {
  if(SpaceTypes::is_portal(type)) {
    portal_to_pos_bag_[type].push_back(pos);
  }
}

void GameScene::init_map_texs() {
  set_space_texs(SpaceType::kCell,&assets_.ceiling_tex(),&assets_.cell_tex(),&assets_.floor_tex());
  set_space_texs(SpaceType::kDeadSpace,&assets_.dead_space_tex(),nullptr,&assets_.dead_space_tex());
  set_space_texs(SpaceType::kDeadSpaceGhost,&assets_.dead_space_ghost_tex(),nullptr
      ,&assets_.dead_space_ghost_tex());
  set_space_texs(SpaceType::kEmpty,&assets_.ceiling_tex(),nullptr,&assets_.floor_tex());
  set_space_texs(SpaceType::kEnd,&assets_.end_tex());
  set_space_texs(SpaceType::kEndWall,&assets_.end_wall_tex());
  set_space_texs(SpaceType::kFruit,&assets_.fruit_tex());
  // SpaceType::kPlayer* - No textures.
  set_space_texs(SpaceType::kPortal0,&assets_.portal_tex());
  set_space_texs(SpaceType::kPortal1,&assets_.portal_tex());
  set_space_texs(SpaceType::kPortal2,&assets_.portal_tex());
  set_space_texs(SpaceType::kPortal3,&assets_.portal_tex());
  set_space_texs(SpaceType::kPortal4,&assets_.portal_tex());
  set_space_texs(SpaceType::kPortal5,&assets_.portal_tex());
  set_space_texs(SpaceType::kPortal6,&assets_.portal_tex());
  set_space_texs(SpaceType::kPortal7,&assets_.portal_tex());
  set_space_texs(SpaceType::kPortal8,&assets_.portal_tex());
  set_space_texs(SpaceType::kPortal9,&assets_.portal_tex());
  set_space_texs(SpaceType::kRobot,&assets_.robot_tex());
  set_space_texs(SpaceType::kRobotGhost,&assets_.robot_tex());
  set_space_texs(SpaceType::kRobotSnake,&assets_.robot_tex());
  set_space_texs(SpaceType::kRobotStatue,&assets_.robot_tex());
  set_space_texs(SpaceType::kRobotWorm,&assets_.robot_tex());
  // SpaceType::kVoid - No textures.
  set_space_texs(SpaceType::kWall,&assets_.ceiling_tex(),&assets_.wall_tex(),&assets_.floor_tex());
  set_space_texs(SpaceType::kWallGhost,&assets_.ceiling_tex(),&assets_.wall_ghost_tex(),&assets_.floor_tex());
  set_space_texs(SpaceType::kWhite,&assets_.white_tex());
  set_space_texs(SpaceType::kWhiteFloor,&assets_.white_tex(),nullptr,&assets_.white_tex());
  set_space_texs(SpaceType::kWhiteGhost,&assets_.white_ghost_tex());
}

void GameScene::on_key_down_event(const KeyEvent& event,const ViewDimens& /*dimens*/) {
  switch(event.key) {
    // Toggle mini map.
    case SDLK_m:
      state_.show_mini_map = !state_.show_mini_map;
      on_state_changed_(state_);
      break;
  }

  scene_action_ = overlay_.on_key_down_event(event);
}

void GameScene::handle_key_states(const KeyStates& keys,const ViewDimens& /*dimens*/) {
  // Key states are stored because in Dantares you can't turn/walk while turning/walking,
  //     and without storing the states and trying again on the next frame,
  //     it feels unresponsive and frustrating.
  // In TurnLeft/Right() & StepForward/Backward(), you can pass in true to force this,
  //     but then this makes the animation a tiny bit strange as you turn off a step.
  const bool is_up = (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]);
  stored_keys_.is_down = (stored_keys_.is_down || keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]);
  stored_keys_.is_left = (stored_keys_.is_left || keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]);
  stored_keys_.is_right = (stored_keys_.is_right || keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]);

  const bool is_walking = (dantares_.IsWalking() >= 0);

  // Must check Left/Right first, so that the Player can turn while walking forward/backward,
  //     which is an important mechanic for the game.
  if(stored_keys_.is_left) {
    if(!stored_keys_.is_right) {
      if(!is_walking) {
        dantares_.TurnLeft();
        stored_keys_.is_left = false;
      } // Else, try again on next frame.
    } else {
      stored_keys_.is_left = false;
    }

    stored_keys_.is_down = false;
    stored_keys_.is_right = false;
  } else if(stored_keys_.is_right) {
    if(!is_walking) {
      dantares_.TurnRight();
      stored_keys_.is_right = false;
    } // Else, try again on next frame.

    stored_keys_.is_down = false;
  } else if(game_phase_ == GamePhase::kPlay && player_warp_time_ <= Duration::kZero) {
    // Check Down first so that it can override continuously moving forward.
    if(stored_keys_.is_down) {
      if(!is_up) {
        if(!is_walking) {
          dantares_.StepBackward();
          stored_keys_.is_down = false;
        } // Else, try again on next frame.
      } else {
        stored_keys_.is_down = false;
      }
    } else {
      dantares_.StepForward(); // Always keep moving forward.
    }
  } else {
    stored_keys_.is_down = false;
  }
}

int GameScene::update_scene_logic(const FrameStep& step,const ViewDimens& dimens) {
  if(scene_action_ != SceneAction::kNil) {
    return std::exchange(scene_action_,SceneAction::kNil);
  }

  switch(game_phase_) {
    case GamePhase::kShowMapInfo:
      if(!overlay_.update_map_info(step)) { return SceneAction::kNil; }

      game_phase_ = GamePhase::kPlay;
      robot_move_time_ += step.dpf;
      break;

    case GamePhase::kPlay:
      break;

    case GamePhase::kGameOver:
      overlay_.update_game_over(step,dimens,player_hit_end_);
      break;
  }

  overlay_.update(step);
  update_player(step);
  update_robots(step);
  move_robots(step);

  return SceneAction::kNil;
}

void GameScene::update_player(const FrameStep& step) {
  if(game_phase_ == GamePhase::kGameOver) { return; }

  if(player_warp_time_ > Duration::kZero) {
    player_warp_time_ -= step.dpf;
    if(player_warp_time_ < Duration::kZero) { player_warp_time_.zero(); }
  }
  if(player_fruit_time_ > Duration::kZero) {
    const auto prev_fruit_secs = player_fruit_time_.round_secs();

    player_fruit_time_ -= step.dpf;

    if(player_fruit_time_ < Duration::kZero) {
      player_fruit_time_.zero();
    } else {
      const auto fruit_secs = player_fruit_time_.round_secs();

      if(fruit_secs != prev_fruit_secs && fruit_secs <= kFruitWarnSecs) {
        overlay_.flash(assets_.fruit_color());
      }
    }
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
      overlay_.flash(assets_.fruit_color());
      player_fruit_time_ = kFruitDuration;
      break;

    default: break;
  }

  // The previous logic above might have updated the type/empty, so check/recheck here.
  const Space* player_space = map_.player_space();
  auto player_empty_type = SpaceType::kEmpty;
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
      const auto portal_bro = fetch_portal_bro(player_pos,player_empty_type,[&](const auto& pos) {
        const Space* space = map_.space(pos);
        return space != nullptr && !space->has_thing();
      });

      if(portal_bro) {
        map_.move_player(*portal_bro);
        overlay_.flash(assets_.portal_color());
        player_warped_ = true;
        player_warp_time_ = kWarpDuration;
      }
    }

    return;
  }

  player_warped_ = false;

  if(SpaceTypes::is_robot(player_space_type)) {
    if(player_fruit_time_ > Duration::kZero) {
      remove_robots_at(player_pos);
    } else {
      game_over(false);
    }
  }
}

void GameScene::game_over(bool hit_end) {
  game_phase_ = GamePhase::kGameOver;
  player_hit_end_ = hit_end;

  // Because of how high speeds are handled, we need to manually sync the correct Player pos,
  //     since the pos might be beyond End, etc. after fully moving.
  map_.sync_player_pos();

  if(!player_hit_end_) { overlay_.fade_to(assets_.eko_color()); }
  overlay_.game_over(map_,player_hit_end_);
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
  if(robot_move_time_ > Duration::kZero) {
    robot_move_time_ -= step.dpf;
    return;
  }

  // Move Robots.
  robot_move_data_.refresh(player_fruit_time_ > Duration::kZero);

  for(auto& robot: robots_) {
    robot.move(robot_move_data_);

    // Warp Robots that are on Portals.
    if(robot.portal_type() != SpaceType::kNil && !robot.warped()) {
      const auto portal_bro = fetch_portal_bro(robot.pos(),robot.portal_type(),[&](const auto& pos) {
        return robot.can_move_to(map_.space(pos));
      });

      if(portal_bro) { robot.warp_to(robot_move_data_,*portal_bro); }
    }
  }

  // Add new Robots after the move loop, because we can't add new ones inside its loop.
  std::ranges::move(robot_move_data_.new_robots,std::back_inserter(robots_));
  robot_move_data_.new_robots.clear();

  // Reset the move time.
  robot_move_time_ = map_.robot_delay() + step.dpf;
}

void GameScene::remove_robots_at(const Pos3i& pos) {
  map_.remove_thing(pos);

  auto result = std::ranges::remove_if(robots_,[&](const auto& robot) {
    return robot.pos() == pos;
  });
  robots_.erase(result.begin(),result.end());
}

std::optional<Pos3i> GameScene::fetch_portal_bro(const Pos3i& pos,SpaceType portal
    ,const MoveChecker& can_move_to) {
  auto it = portal_to_pos_bag_.find(portal);
  if(it == portal_to_pos_bag_.end()) { return std::nullopt; }

  auto& bros = it->second;
  if(bros.size() <= 1) { return std::nullopt; } // No bros. :(

  if(bros.size() > 2) { // More than 1 bro?
    Rando::it().shuffle(bros.begin(),bros.end());
  }

  for(auto& bro_pos: bros) { // Find Luigi.
    if(bro_pos != pos && can_move_to(bro_pos)) { return bro_pos; }
  }

  return std::nullopt;
}

void GameScene::draw_scene(Renderer& ren,const ViewDimens& dimens) {
  ren.begin_3d_scene();

  if(player_hit_end_) {
    // Even if fully transparent, continue to draw so that the Player can turn the mini map (just for fun).
    ren.wrap_color(assets_.end_color().with_a(1.0f - overlay_.game_over_age()),[&]() {
      dantares_.Draw(kDantaresDist);
    });
  } else {
    dantares_.Draw(kDantaresDist);
  }

  ren.begin_2d_scene();
  hud_.draw(ren,dimens,map_,state_.show_mini_map,player_fruit_time_,player_hit_end_);
  overlay_.draw(ren,dimens);

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

// ReSharper disable once CppDFAUnreachableFunctionCall
void GameScene::set_space_texs(SpaceType type,const Texture* tex) {
  set_space_texs(type,tex,tex,tex);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void GameScene::set_space_texs(SpaceType type,const Texture* ceiling,const Texture* wall
    ,const Texture* floor) {
  const int space_id = SpaceTypes::value_of(type);

  // Ceiling & Floor textures are flipped due to using opposite values in Dantares ctor.
  if(ceiling != nullptr) { dantares_.SetFloorTexture(space_id,ceiling->gl_id()); }
  if(wall != nullptr) { dantares_.SetWallTexture(space_id,wall->gl_id()); }
  if(floor != nullptr) { dantares_.SetCeilingTexture(space_id,floor->gl_id()); }
}

} // Namespace.
