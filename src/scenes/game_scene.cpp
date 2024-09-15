/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_scene.h"

namespace ekoscape {

const Color4f GameScene::kTextBgColor{0.0f,0.5f};
const Size2i GameScene::kTextBgPadding{15,10};
const Duration GameScene::kMapInfoDuration = Duration::from_millis(3'000);
const Duration GameScene::kInitRobotDelay = Duration::from_millis(1'000);
const float GameScene::kGameOverLifespan = 3.0f; // Seconds.

GameScene::GameScene(const Assets& assets,const std::filesystem::path& map_file,const State& state
    ,const StateCallback& on_state_changed)
    : assets_(assets),state_(state),on_state_changed_(on_state_changed),hud_(assets) {
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

void GameScene::init_scene(Renderer& /*ren*/) {
  // If was paused, adjust robot_move_duration_ to be accurate.
  // - For maps that depend on accurate time, like `60 Seconds`, this prevents cheating.
  if(robot_move_timer_.duration() > Duration::kZero) {
    robot_move_duration_ -= robot_move_timer_.duration();
    if(robot_move_duration_ < Duration::kZero) { robot_move_duration_.zero(); }
  }

  map_info_timer_.start();
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
      if(map_info_timer_.end().duration() < kMapInfoDuration) { return SceneAction::kNil; }

      game_phase_ = GamePhase::kPlay;
      robot_move_timer_.start();
      break;

    case GamePhase::kPlay:
      break;

    case GamePhase::kGameOver:
      if(game_over_age_ < 1.0f) {
        game_over_age_ += (static_cast<float>(step.delta_time) / kGameOverLifespan);
        if(game_over_age_ > 1.0f) { game_over_age_ = 1.0f; }
      }
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
  dantares_.Draw(kDantaresDist);

  ren.begin_2d_scene();
  hud_.draw(ren,map_,state_.show_mini_map);

  if(game_phase_ == GamePhase::kPlay) { return; }

  ren.begin_auto_center_scale();
  switch(game_phase_) {
    case GamePhase::kShowMapInfo:
      draw_map_info(ren);
      break;

    case GamePhase::kGameOver:
      draw_game_over(ren);
      break;

    default: break;
  }
  ren.end_scale();
}

void GameScene::draw_map_info(Renderer& ren) {
  assets_.font_renderer().wrap(ren,{395,395},[&](auto& font) {
    const tiny_utf8::string title = map_.title();
    const tiny_utf8::string author = "  by " + map_.author();
    const auto bg_w = static_cast<int>(std::max(title.length(),author.length()));

    font.draw_bg(kTextBgColor,{bg_w,2},kTextBgPadding);
    font.puts(title);
    font.puts(author);
  });
}

void GameScene::draw_game_over(Renderer& ren) {
  Color4f bg_color = kTextBgColor;
  const int total_rescues = map_.total_rescues();
  const int total_cells = map_.total_cells();
  const bool freed_all = (total_rescues >= total_cells);

  bg_color.a *= game_over_age_;

  ren.wrap_sprite(player_hit_end_ ? assets_.corngrits_sprite() : assets_.goodnight_sprite(),[&](auto& s) {
    ren.wrap_color({1.0f,game_over_age_},[&]() {
      s.draw_quad({10,10},{1200,450});
    });
  });
  assets_.font_renderer().wrap(ren,{460,460},0.60f,[&](auto& font) {
    font.font_color.a *= game_over_age_;

    const auto font_color = font.font_color;
    const Color4f miss_color{1.0f,0.0f,0.0f,font_color.a};
    const Color4f goal_color{0.0f,1.0f,0.0f,font_color.a};

    font.draw_bg(bg_color,{37,5},kTextBgPadding);
    font.puts(player_hit_end_ ? "Congrats!" : "You're dead!");

    font.print("You freed ");
    font.font_color = freed_all ? goal_color : miss_color;
    font.print(std::to_string(total_rescues));
    font.font_color = font_color;
    font.print(" eko");
    if(total_rescues != 1) { font.print('s'); }
    font.print(" out of ");
    font.font_color = goal_color;
    font.print(std::to_string(total_cells));
    font.font_color = font_color;
    font.print(" eko");
    if(total_cells != 1) { font.print('s'); }
    font.print('.');

    if(player_hit_end_ && freed_all) {
      font.puts_blanks(2);
      font.puts("You unlocked a secret!");
      font.print("Press ");
      font.font_color.set(1.0f,1.0f,0.0f,font_color.a);
      font.print("F");
      font.font_color = font_color;
      font.print(" key in the Credits scene.");
    }
  });

  assets_.font_renderer().wrap(ren,{580,790},[&](auto& font) {
    font.draw_bg(bg_color,{9,1},kTextBgPadding);
    font.font_color.a *= game_over_age_;
    font.draw_menu_opt("go back",FontRenderer::kMenuStyleSelected);
  });
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
