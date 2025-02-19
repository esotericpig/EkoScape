/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_scene.h"

#include "cybel/util/rando.h"

#if defined(CYBEL_RENDERER_GLES)
#else // CYBEL_RENDERER_GL.
  #include "Dantares/Dantares2GL.h"
#endif

#include "input/input_action.h"
#include "map/dantares_map.h"

#include <ranges>

namespace ekoscape {

GameScene::GameScene(GameContext& ctx,State& state,const std::filesystem::path& map_file)
  : ctx_(ctx),state_(state) {
  #if defined(CYBEL_RENDERER_GLES)
  #else // CYBEL_RENDERER_GL.
    dantares_renderer_ = std::make_unique<Dantares2GLRenderer>();
  #endif

  // Dantares2(...,SquareSize,FloorHeight,CeilingHeight).
  // - Classic values: (0.125f,-0.04f,0.04f).
  // - The floor & ceiling heights' signs are swapped, so that the images aren't flipped vertically.
  //   - See set_space_texs(), which relies on this logic.
  dantares_ = std::make_unique<Dantares2>(*dantares_renderer_,0.125f,0.04f,-0.04f);
  map_ = std::make_unique<DantaresMap>(*dantares_,[&](auto& /*dan*/,int /*z*/,int /*grid_id*/) {
    init_map_texs();
  });
  robot_move_data_ = std::make_unique<Robot::MoveData>(*map_);

  init_map(map_file);

  // Extra delay to give some time for the Player to initially orient/adjust.
  robot_move_time_ = map_->robot_delay() + kInitExtraRobotDelay;

  hud_ = std::make_unique<GameHud>(ctx,GameHud::State{
    .map = *map_,
    .show_mini_map = state_.show_mini_map,
    .player_fruit_time = player_fruit_time_,
    .player_hit_end = player_hit_end_,
    .show_speedrun = state.show_speedrun,
  });
  overlay_ = std::make_unique<GameOverlay>(ctx,*map_,player_hit_end_);
}

void GameScene::init_map(const std::filesystem::path& map_file) {
  std::vector<Pos3i> cells{};

  map_->load_file(
    map_file,
    [&](const auto& pos,SpaceType type) { return init_map_space(pos,type,cells); },
    [&](const auto& pos,SpaceType type) { init_map_default_empty(pos,type); }
  );
  if(ctx_.assets.is_weird()) { make_map_weird(cells); }

  std::cout << "[INFO] Map file ['" << map_file.string() << "'] w/ grids [" << map_->grid_count() << "]:\n"
            << *map_ << std::endl;

  map_->add_to_bridge();
}

SpaceType GameScene::init_map_space(const Pos3i& pos,SpaceType type,std::vector<Pos3i>& cells) {
  switch(type) {
    case SpaceType::kCell:
      cells.push_back(pos);
      break;

    default:
      if(SpaceTypes::is_robot(type)) {
        robots_.push_back(Robot::build(type,pos));
      } else if(SpaceTypes::is_portal(type)) {
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

void GameScene::make_map_weird(std::vector<Pos3i>& cells) {
  // For weird, flip Robots & Cells, where a random Cell becomes a random Robot.
  // If we run out of Robots, then just use normal ones.

  // First, flip all Robots to Cells in Map, because we might have more Robots than Cells.
  for(auto& robot : robots_) {
    map_->set_raw_thing(robot.pos(),SpaceType::kCell);
  }

  Rando::it().shuffle(robots_.begin(),robots_.end());
  Rando::it().shuffle(cells.begin(),cells.end());

  std::vector<Robot> new_robots{};

  for(std::size_t robot_i = 0; const auto& cell_pos : cells) {
    if(robot_i < robots_.size()) {
      auto robot = std::move(robots_[robot_i]);
      ++robot_i;

      map_->set_raw_thing(cell_pos,robot.type());
      robot.set_raw_pos(cell_pos);

      new_robots.push_back(std::move(robot));
    } else { // No Robots left.
      auto robot = Robot::build_normal(cell_pos);

      map_->set_raw_thing(cell_pos,robot.type());

      new_robots.push_back(std::move(robot));
    }
  }

  robots_ = std::move(new_robots);
}

void GameScene::init_map_texs() {
  const auto& a = ctx_.assets;

  set_space_texs(SpaceType::kCell,&a.ceiling_tex(),&a.cell_tex(),&a.floor_tex());
  set_space_texs(SpaceType::kDeadSpace,&a.dead_space_tex(),nullptr,&a.dead_space_tex());
  set_space_texs(SpaceType::kDeadSpaceGhost,&a.dead_space_ghost_tex(),nullptr,&a.dead_space_ghost_tex());
  set_space_texs(SpaceType::kEmpty,&a.ceiling_tex(),nullptr,&a.floor_tex());
  set_space_texs(SpaceType::kEnd,&a.end_tex());
  set_space_texs(SpaceType::kEndWall,&a.end_wall_tex());
  set_space_texs(SpaceType::kFruit,&a.fruit_tex());
  // SpaceType::kPlayer* - No textures.
  set_space_texs(SpaceType::kPortal0,&a.portal_tex());
  set_space_texs(SpaceType::kPortal1,&a.portal_tex());
  set_space_texs(SpaceType::kPortal2,&a.portal_tex());
  set_space_texs(SpaceType::kPortal3,&a.portal_tex());
  set_space_texs(SpaceType::kPortal4,&a.portal_tex());
  set_space_texs(SpaceType::kPortal5,&a.portal_tex());
  set_space_texs(SpaceType::kPortal6,&a.portal_tex());
  set_space_texs(SpaceType::kPortal7,&a.portal_tex());
  set_space_texs(SpaceType::kPortal8,&a.portal_tex());
  set_space_texs(SpaceType::kPortal9,&a.portal_tex());
  set_space_texs(SpaceType::kRobot,&a.robot_tex());
  set_space_texs(SpaceType::kRobotGhost,&a.robot_tex());
  set_space_texs(SpaceType::kRobotSnake,&a.robot_tex());
  set_space_texs(SpaceType::kRobotStatue,&a.robot_tex());
  set_space_texs(SpaceType::kRobotWorm,&a.robot_tex());
  // SpaceType::kVoid - No textures.
  set_space_texs(SpaceType::kWall,&a.ceiling_tex(),&a.wall_tex(),&a.floor_tex());
  set_space_texs(SpaceType::kWallGhost,&a.ceiling_tex(),&a.wall_ghost_tex(),&a.floor_tex());
  set_space_texs(SpaceType::kWhite,&a.white_tex());
  set_space_texs(SpaceType::kWhiteFloor,&a.white_tex(),nullptr,&a.white_tex());
  set_space_texs(SpaceType::kWhiteGhost,&a.white_ghost_tex());
}

void GameScene::init_scene(const ViewDimens& /*dimens*/) {
  if(game_phase_ == GamePhase::kPlay) {
    speedrun_timer_.resume();
  }
}

void GameScene::on_scene_exit() {
  if(game_phase_ == GamePhase::kPlay) {
    speedrun_timer_.pause();
  }
}

void GameScene::on_input_event(int action,const ViewDimens& /*dimens*/) {
  switch(action) {
    case InputAction::kToggleMiniMap:
      state_.show_mini_map = !state_.show_mini_map;
      break;

    case InputAction::kToggleSpeedrun:
      // The speedrun time is always shown on Game Over, so don't toggle.
      if(game_phase_ != GamePhase::kGameOver) {
        state_.show_speedrun = !state_.show_speedrun;
      }
      break;
  }

  scene_action_ = overlay_->on_input_event(action);
}

void GameScene::handle_input_states(const std::vector<bool>& states,const ViewDimens& /*dimens*/) {
  // Input states are stored because in Dantares you can't turn/walk while turning/walking,
  //     and without storing the states and trying again on the next frame,
  //     it feels unresponsive and frustrating.
  const bool is_up = states[InputAction::kUp];
  stored_inputs_.is_down = (stored_inputs_.is_down || states[InputAction::kDown]);
  stored_inputs_.is_left = (stored_inputs_.is_left || states[InputAction::kLeft]);
  stored_inputs_.is_right = (stored_inputs_.is_right || states[InputAction::kRight]);

  const bool is_walking = dantares_->IsWalking();

  // Must check Left/Right first, so that the Player can turn while walking forward/backward,
  //     which is an important mechanic for the game.
  if(stored_inputs_.is_left) {
    if(!stored_inputs_.is_right) {
      if(!is_walking) {
        dantares_->TurnLeft();
        stored_inputs_.is_left = false;
      } // Else, try again on next frame.
    } else {
      stored_inputs_.is_left = false;
    }

    stored_inputs_.is_down = false;
    stored_inputs_.is_right = false;
  } else if(stored_inputs_.is_right) {
    if(!is_walking) {
      dantares_->TurnRight();
      stored_inputs_.is_right = false;
    } // Else, try again on next frame.

    stored_inputs_.is_down = false;
  } else if(game_phase_ == GamePhase::kPlay && player_warp_time_ <= Duration::kZero) {
    // Check Down first so that it can override continuously moving forward.
    if(stored_inputs_.is_down) {
      if(!is_up) {
        if(!is_walking) {
          dantares_->StepBackward();
          stored_inputs_.is_down = false;
        } // Else, try again on next frame.
      } else {
        stored_inputs_.is_down = false;
      }
    } else {
      dantares_->StepForward(); // Always keep moving forward.
    }
  } else {
    stored_inputs_.is_down = false;
  }
}

int GameScene::update_scene_logic(const FrameStep& step,const ViewDimens& dimens) {
  if(scene_action_ != SceneAction::kNil) {
    return std::exchange(scene_action_,SceneAction::kNil);
  }

  switch(game_phase_) {
    case GamePhase::kShowMapInfo:
      // Still paused showing the Map Info?
      if(!overlay_->update_map_info(step)) { return SceneAction::kNil; }

      game_phase_ = GamePhase::kPlay;
      robot_move_time_ += step.dpf;
      speedrun_timer_.start();
      break;

    case GamePhase::kPlay:
      break;

    case GamePhase::kGameOver:
      overlay_->update_game_over(step,dimens);
      break;
  }

  hud_->state.speedrun_time = speedrun_timer_.peek();
  hud_->update(step);
  overlay_->update(step);

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
        overlay_->flash(ctx_.assets.fruit_color());
      }
    }
  }

  const Pos3i player_pos = map_->player_pos();
  SpaceType player_space_type = map_->player_space_type();

  switch(player_space_type) {
    case SpaceType::kCell:
      map_->remove_thing(player_pos);
      break;

    // Check for End before Robots & Portals.
    case SpaceType::kEnd:
      game_over(true);
      return;

    // Check for Fruit before Robots.
    case SpaceType::kFruit:
      map_->remove_thing(player_pos);
      overlay_->flash(ctx_.assets.fruit_color());
      player_fruit_time_ = kFruitDuration;
      break;

    default: break;
  }

  // The previous logic above might have updated the type/empty, so check/recheck here.
  const Space* player_space = map_->player_space();
  auto player_empty_type = SpaceType::kEmpty;
  player_space_type = map_->player_space_type();

  if(player_space == nullptr) {
    std::cerr << "[ERROR] Player space is null for some reason." << std::endl;
  } else {
    player_empty_type = player_space->empty_type();
  }

  // Portals are like safe zones, so if the Player & a Robot are on a Portal, the Player shouldn't die.
  //     Therefore, we check for Portals first.
  if(SpaceTypes::is_portal(player_empty_type)) {
    if(!player_warped_) {
      const auto portal_bro = fetch_portal_bro(player_pos,player_empty_type,[&](const auto& pos) {
        // Allow the Player to warp even if there's a Robot/Thing on the Portal.
        return map_->space(pos) != nullptr;
      });

      if(portal_bro) {
        map_->move_player(*portal_bro);
        overlay_->flash(ctx_.assets.portal_color());
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
  hud_->state.speedrun_time = speedrun_timer_.pause();
  hud_->state.is_game_over = true;

  game_phase_ = GamePhase::kGameOver;
  player_hit_end_ = hit_end;

  // Because of how high speeds are handled, we need to manually sync the correct Player pos,
  //     since the pos might be beyond End, etc. after fully moving.
  map_->sync_player_pos();

  // Fade to death?
  if(!player_hit_end_) { overlay_->fade_to(ctx_.assets.eko_color()); }
  overlay_->game_over();
}

void GameScene::update_robots(const FrameStep& step) {
  // Remove dead Robots and age living Robots (only if lifespan was set).
  auto dead_robots = std::ranges::remove_if(robots_,[&](auto& robot) {
    if(robot.is_dead()) {
      map_->remove_thing(robot.pos());
      return true;
    }

    robot.age(step.delta_time);
    return false;
  });
  robots_.erase(dead_robots.begin(),dead_robots.end());
}

void GameScene::move_robots(const FrameStep& step) {
  // Not time to move Robots?
  if(robot_move_time_ > Duration::kZero) {
    robot_move_time_ -= step.dpf;
    return;
  }

  // Move Robots.
  robot_move_data_->refresh(player_fruit_time_ > Duration::kZero);

  for(auto& robot : robots_) {
    robot.move(*robot_move_data_);

    // Warp Robots that are on Portals.
    if(robot.portal_type() != SpaceType::kNil && !robot.warped()) {
      const auto portal_bro = fetch_portal_bro(robot.pos(),robot.portal_type(),[&](const auto& pos) {
        return robot.can_move_to(map_->space(pos));
      });

      if(portal_bro) { robot.warp_to(*robot_move_data_,*portal_bro); }
    }
  }

  // Add new Robots after the move loop, because we can't add new ones inside its loop.
  std::ranges::move(robot_move_data_->new_robots,std::back_inserter(robots_));
  robot_move_data_->new_robots.clear();

  // Reset the move time.
  robot_move_time_ = map_->robot_delay() + step.dpf;
}

void GameScene::remove_robots_at(const Pos3i& pos) {
  map_->remove_thing(pos);

  auto dead_robots = std::ranges::remove_if(robots_,[&](const auto& robot) {
    return robot.pos() == pos;
  });
  robots_.erase(dead_robots.begin(),dead_robots.end());
}

std::optional<Pos3i> GameScene::fetch_portal_bro(const Pos3i& pos,SpaceType portal,
                                                 const MoveChecker& can_move_to) {
  const auto it = portal_to_pos_bag_.find(portal);
  if(it == portal_to_pos_bag_.end()) { return std::nullopt; }

  auto& bros = it->second;
  if(bros.size() <= 1) { return std::nullopt; } // No bros. :(

  if(bros.size() > 2) { // More than 1 bro?
    Rando::it().shuffle(bros.begin(),bros.end());
  }

  for(auto& bro_pos : bros) { // Find Luigi.
    if(bro_pos != pos && can_move_to(bro_pos)) { return bro_pos; }
  }

  return std::nullopt;
}

void GameScene::draw_scene(Renderer& ren,const ViewDimens& dimens) {
  ren.begin_3d_scene();

  if(player_hit_end_) {
    // Even if fully transparent, continue to draw so that the Player can turn the mini map (just for fun).
    ren.wrap_color(ctx_.assets.end_color().with_a(1.0f - overlay_->game_over_age()),[&]() {
      dantares_->Draw(kDantaresDist);
    });
  } else {
    dantares_->Draw(kDantaresDist);
  }

  ren.begin_2d_scene();
  hud_->draw(ren,dimens);
  overlay_->draw(ren,dimens);

  switch(game_phase_) {
    case GamePhase::kShowMapInfo:
      overlay_->draw_map_info(ren);
      break;

    case GamePhase::kPlay:
      break;

    case GamePhase::kGameOver:
      overlay_->draw_game_over(ren);
      break;
  }
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void GameScene::set_space_texs(SpaceType type,const Texture* tex) {
  set_space_texs(type,tex,tex,tex);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void GameScene::set_space_texs(SpaceType type,const Texture* ceiling,const Texture* wall,
                               const Texture* floor) {
  const int space_id = SpaceTypes::value_of(type);

  // Ceiling & Floor textures are flipped due to using opposite values in Dantares ctor.
  if(ceiling != nullptr) { dantares_->SetFloorTexture(space_id,static_cast<int>(ceiling->gl_id())); }
  if(wall != nullptr) { dantares_->SetWallTexture(space_id,static_cast<int>(wall->gl_id())); }
  if(floor != nullptr) { dantares_->SetCeilingTexture(space_id,static_cast<int>(floor->gl_id())); }
}

} // Namespace.
