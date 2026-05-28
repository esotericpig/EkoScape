/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_scene.h"

#include "cybel/util/rando.h"

#include "core/input_action.h"
#include "map/dantares_map.h"
#include "scenes/dantares_renderer.h"

#include <algorithm>

namespace ekoscape {

GameScene::GameScene(GameSession& sesh,Renderer& ren,AssetMan& assets,const std::filesystem::path& map_file)
  : sesh_{sesh},
    dantares_renderer_{std::make_unique<DantaresRenderer>(ren)} {
  // Dantares2(...,SquareSize,FloorHeight,CeilingHeight).
  // - Classic values: (0.125f,-0.04f,0.04f).
  // - The floor & ceiling heights' signs are swapped, so that the images aren't flipped vertically.
  //   - See set_space_texs(), which relies on this logic.
  dantares_ = std::make_unique<Dantares2>(*dantares_renderer_,0.125f,0.04f,-0.04f);
  map_ = std::make_unique<DantaresMap>(*dantares_,[&](auto& /*dan*/,int /*z*/,int /*grid_id*/) {
    init_map_textures(assets);
  });
  robot_move_data_ = std::make_unique<Robot::MoveData>(*map_);

  init_map(map_file);

  // Extra delay to give some time for the Player to initially orient/adjust.
  robot_move_time_ = map_->robot_delay() + kInitExtraRobotDelay;

  hud_ = std::make_unique<GameHud>(sesh_,*map_);
  overlay_ = std::make_unique<GameOverlay>(sesh_,*map_);
}

void GameScene::init_map(const std::filesystem::path& map_file) {
  std::vector<Pos3i> cells{};

  map_->load_file(
    map_file,
    [&](const auto& pos,SpaceType type) { return init_map_space(pos,type,cells); },
    [&](const auto& pos,SpaceType type) { init_map_default_empty(pos,type); }
  );
  if(sesh_.assets.is_weird()) { make_map_weird(cells); }

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

void GameScene::init_map_textures(AssetMan& assets) {
  const auto& ids = sesh_.assets.styled_texture_ids();
  constexpr auto no_id = std::nullopt;

  set_space_textures(assets,SpaceType::kCell,ids.ceiling,ids.cell,ids.floor);
  set_space_textures(assets,SpaceType::kDeadSpace,ids.dead_space,no_id,ids.dead_space);
  set_space_textures(assets,SpaceType::kDeadSpaceGhost,ids.dead_space_ghost,no_id,ids.dead_space_ghost);
  set_space_textures(assets,SpaceType::kEmpty,ids.ceiling,no_id,ids.floor);
  set_space_textures(assets,SpaceType::kEnd,ids.end);
  set_space_textures(assets,SpaceType::kEndWall,ids.end_wall);
  set_space_textures(assets,SpaceType::kFruit,ids.fruit);
  // SpaceType::kPlayer* - No textures.
  set_space_textures(assets,SpaceType::kPortal0,ids.portal);
  set_space_textures(assets,SpaceType::kPortal1,ids.portal);
  set_space_textures(assets,SpaceType::kPortal2,ids.portal);
  set_space_textures(assets,SpaceType::kPortal3,ids.portal);
  set_space_textures(assets,SpaceType::kPortal4,ids.portal);
  set_space_textures(assets,SpaceType::kPortal5,ids.portal);
  set_space_textures(assets,SpaceType::kPortal6,ids.portal);
  set_space_textures(assets,SpaceType::kPortal7,ids.portal);
  set_space_textures(assets,SpaceType::kPortal8,ids.portal);
  set_space_textures(assets,SpaceType::kPortal9,ids.portal);
  set_space_textures(assets,SpaceType::kRobot,ids.robot);
  set_space_textures(assets,SpaceType::kRobotGhost,ids.robot);
  set_space_textures(assets,SpaceType::kRobotSnake,ids.robot);
  set_space_textures(assets,SpaceType::kRobotStatue,ids.robot);
  set_space_textures(assets,SpaceType::kRobotWorm,ids.robot);
  // SpaceType::kVoid - No textures.
  set_space_textures(assets,SpaceType::kWall,ids.ceiling,ids.wall,ids.floor);
  set_space_textures(assets,SpaceType::kWallGhost,ids.ceiling,ids.wall_ghost,ids.floor);
  set_space_textures(assets,SpaceType::kWhite,ids.white);
  set_space_textures(assets,SpaceType::kWhiteFloor,ids.white,no_id,ids.white);
  set_space_textures(assets,SpaceType::kWhiteGhost,ids.white_ghost);
}

void GameScene::on_scene_enter([[maybe_unused]] SceneContext& ctx) {
  switch(game_phase_) {
    case GamePhase::kShowMapInfo:
      map_info_timer_.resume();
      break;

    case GamePhase::kPlay:
      speedrun_timer_.resume();
      break;

    case GamePhase::kGameOver:
      break;
  }
}

void GameScene::on_scene_exit([[maybe_unused]] SceneContext& ctx) {
  switch(game_phase_) {
    case GamePhase::kShowMapInfo:
      map_info_timer_.pause();
      break;

    case GamePhase::kPlay:
      speedrun_timer_.pause();
      break;

    case GamePhase::kGameOver:
      break;
  }
}

void GameScene::on_scene_gpu_context_restore([[maybe_unused]] SceneContext& ctx) {
  // Can throw CybelError.
  map_->on_gpu_context_restore();
}

void GameScene::on_scene_input_event(input_id_t input_id,SceneContext& ctx) {
  switch(static_cast<InputAction>(input_id)) {
    case InputAction::kToggleMiniMap:
      sesh_.game_scene_state.show_mini_map = !sesh_.game_scene_state.show_mini_map;
      break;

    case InputAction::kToggleSpeedrun:
      // The speedrun time is always shown on Game Over, so don't toggle.
      if(game_phase_ != GamePhase::kGameOver) {
        sesh_.game_scene_state.show_speedrun = !sesh_.game_scene_state.show_speedrun;
      }
      break;

    default:
      overlay_->on_scene_input_event(input_id,ctx);
      break;
  }
}

void GameScene::handle_scene_input(InputMan& input,[[maybe_unused]] SceneContext& ctx) {
  // Input states are stored because in Dantares you can't turn/walk while turning/walking,
  //     and without storing the states and trying again on the next frame,
  //     it feels unresponsive and frustrating.
  const bool is_up = input[InputAction::kUp];
  stored_inputs_.is_down = (stored_inputs_.is_down || input[InputAction::kDown]);
  stored_inputs_.is_left = (stored_inputs_.is_left || input[InputAction::kLeft]);
  stored_inputs_.is_right = (stored_inputs_.is_right || input[InputAction::kRight]);

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

void GameScene::update_scene_logic(const FrameStep& step,SceneContext& ctx) {
  dantares_->UpdateDeltaTime(static_cast<float>(step.delta_time));

  if(game_phase_ == GamePhase::kShowMapInfo && map_info_timer_.peek() >= kMapInfoDuration) {
    game_phase_ = GamePhase::kPlay;
    robot_move_time_ += step.dpf;
    speedrun_timer_.restart();
  }
  if(game_phase_ != GamePhase::kShowMapInfo) {
    update_player(step);
    update_robots(step);
  }

  update_mods(step,ctx);
}

void GameScene::update_player(const FrameStep& step) {
  if(game_phase_ == GamePhase::kGameOver) { return; }

  if(player_warp_time_ > Duration::kZero) {
    player_warp_time_ -= step.dpf;
    if(player_warp_time_ < Duration::kZero) { player_warp_time_.set_to_zero(); }
  }
  if(player_fruit_time_ > Duration::kZero) {
    const auto prev_fruit_secs = player_fruit_time_.round_secs();

    player_fruit_time_ -= step.dpf;

    if(player_fruit_time_ < Duration::kZero) {
      player_fruit_time_.set_to_zero();
    } else {
      const auto fruit_secs = player_fruit_time_.round_secs();

      if(fruit_secs != prev_fruit_secs && fruit_secs <= kFruitWarnSecs) {
        overlay_->flash(sesh_.assets.fruit_color());
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
      overlay_->flash(sesh_.assets.fruit_color());
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
        overlay_->flash(sesh_.assets.portal_color());
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

void GameScene::game_over(bool player_hit_end) {
  speedrun_timer_.pause();
  game_phase_ = GamePhase::kGameOver;
  player_hit_end_ = player_hit_end;

  // Because of how high speeds are handled, we need to manually sync the correct Player pos,
  //     since the pos might be beyond End, etc. after fully moving.
  map_->sync_player_pos();

  // Fade to death?
  if(!player_hit_end_) { overlay_->fade_to(sesh_.assets.eko_color()); }
  overlay_->game_over(player_hit_end_);
}

void GameScene::update_robots(const FrameStep& step) {
  // Remove dead Robots and age living Robots (only if lifespan was set).
  std::erase_if(robots_,[&](auto& robot) {
    if(robot.is_dead()) {
      map_->remove_thing(robot.pos());
      return true;
    }

    robot.age(step.delta_time);
    return false;
  });

  move_robots(step);
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

  std::erase_if(robots_,[&](const auto& robot) {
    return robot.pos() == pos;
  });
}

std::optional<Pos3i> GameScene::fetch_portal_bro(const Pos3i& pos,SpaceType portal,
                                                 const MoveChecker& can_move_to) {
  const auto it = portal_to_pos_bag_.find(portal);
  if(it == portal_to_pos_bag_.end()) { return std::nullopt; }

  auto& bros = it->second;
  if(bros.size() <= 1) { return std::nullopt; } // No bros. :(

  if(bros.size() > 2) { // More than 1 bro?
    // Try once without shuffling for speed.
    const auto& rand_bro_pos = bros[Rando::it().rand_size_t(bros.size())];

    if(rand_bro_pos != pos && can_move_to(rand_bro_pos)) {
      return std::optional{rand_bro_pos};
    }

    Rando::it().shuffle(bros.begin(),bros.end());
  }

  for(const auto& bro_pos : bros) { // Find Luigi.
    if(bro_pos != pos && can_move_to(bro_pos)) {
      return std::optional{bro_pos};
    }
  }

  return std::nullopt;
}

void GameScene::update_mods(const FrameStep& step,SceneContext& ctx) {
  hud_->update_state(GameHud::State{
    .show_mini_map = sesh_.game_scene_state.show_mini_map,
    .player_fruit_time = player_fruit_time_,
    .player_hit_end = player_hit_end_,

    .is_game_over = (game_phase_ == GamePhase::kGameOver),
    .speedrun_time = speedrun_timer_.peek(),
    .show_speedrun = sesh_.game_scene_state.show_speedrun,
  });
  overlay_->update_state(GameOverlay::State{
    .is_map_info = (game_phase_ == GamePhase::kShowMapInfo),
    .player_hit_end = player_hit_end_,
  });

  hud_->update_scene_logic(step,ctx);
  overlay_->update_scene_logic(step,ctx);
}

void GameScene::draw_scene(Renderer& ren,SceneContext& ctx) {
  ren.begin_3d_scene();

  const bool move_player = ctx.engine.is_logic_running();

  if(player_hit_end_) {
    // Even if fully transparent, continue to draw so that the Player can turn the mini map (just for fun).
    ren.wrap_color(sesh_.assets.end_color().with_a(1.0f - overlay_->game_over_age()),[&] {
      dantares_->Draw(kDantaresDist,move_player);
    });
  } else {
    dantares_->Draw(kDantaresDist,move_player);
  }

  ren.begin_2d_scene();
  hud_->draw_scene(ren,ctx);
  overlay_->draw_scene(ren,ctx);
}

void GameScene::set_space_textures(AssetMan& assets,SpaceType type,asset_id_t id) {
  set_space_textures(assets,type,id,id,id);
}

void GameScene::set_space_textures(AssetMan& assets,SpaceType type,std::optional<asset_id_t> ceiling_id,
                                   std::optional<asset_id_t> wall_id,std::optional<asset_id_t> floor_id) {
  const int space_id = SpaceTypes::value_of(type);

  // NOTE: Ceiling & Floor textures are flipped due to using opposite values in Dantares ctor.
  if(ceiling_id) { dantares_->SetFloorTexture(space_id,assets.texture(*ceiling_id).handle()); }
  if(wall_id) { dantares_->SetWallTexture(space_id,assets.texture(*wall_id).handle()); }
  if(floor_id) { dantares_->SetCeilingTexture(space_id,assets.texture(*floor_id).handle()); }
}

} // namespace ekoscape
