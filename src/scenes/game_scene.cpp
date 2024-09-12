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
const Size2i GameScene::kMiniMapHoodRadius{4,3};
const Size2i GameScene::kMiniMapBlockSize{30,30};
const Size2i GameScene::kMiniMapSize{
  // +1 for player.
  ((kMiniMapHoodRadius.w << 1) + 1) * kMiniMapBlockSize.w,
  ((kMiniMapHoodRadius.h << 1) + 1) * kMiniMapBlockSize.h
};

GameScene::GameScene(const Assets& assets,const std::filesystem::path& map_file,const State& state
    ,const StateCallback& on_state_changed)
    : assets_(assets),state_(state),on_state_changed_(on_state_changed) {
  load_map(map_file);
  generate_map();

  // Extra delay to give some time for the player to orient/adjust.
  robot_move_duration_ = map_.robot_delay() + kInitRobotDelay;

  mini_map_eko_color_ = assets_.is_weird()
      ? Color4f::hex(0x0000ff,kMiniMapAlpha)
      : Color4f::hex(0xff0000,kMiniMapAlpha);
  mini_map_end_color_ = assets_.is_weird()
      ? Color4f::hex(0xffff00,kMiniMapAlpha) // Yellow.
      : Color4f::hex(0x0000ff,kMiniMapAlpha);
  mini_map_non_walkable_color_.set_hex(0x00ff00,kMiniMapAlpha);
  mini_map_robot_color_.set_bytes(214,kMiniMapAlpha);
  mini_map_walkable_color_.set_bytes(0,kMiniMapAlpha);
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
  if(game_phase_ == GamePhase::kShowMapInfo) {
    if(map_info_timer_.end().duration() < kMapInfoDuration) { return SceneAction::kNil; }

    game_phase_ = GamePhase::kPlay;
    robot_move_timer_.start();
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

    case SpaceType::kEnd:
      // TODO: Implement Game Over scene.
      printf("Congratulations!\nYou freed %d Eko(s) out of a total of %d Eko(s).\n\n"
          ,map_.total_rescues(),map_.total_cells());
      if(map_.total_rescues() == map_.total_cells()) {
        std::cout << "You've unlocked a secret!\n";
      }
      game_phase_ = GamePhase::kGameOver;
      break;

    default:
      if(SpaceTypes::is_robot(player_space_type)) {
        // TODO: Implement Game Over scene.
        printf("You're dead!\nYou freed %d Eko(s) out of a total of %d Eko(s).\n\n"
            ,map_.total_rescues(),map_.total_cells());
        game_phase_ = GamePhase::kGameOver;
      }
      break;
  }

  if(game_phase_ == GamePhase::kGameOver) {
    // Because of high speeds, we need to manually set the correct pos,
    //     since the pos might be beyond End, etc.
    map_.set_player_pos();
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

  ren.begin_auto_anchor_scale({0.0f,1.0f}); // Anchor mini map to bottom left.
  draw_mini_map(ren);
  ren.end_scale();

  ren.begin_auto_center_scale();
  switch(game_phase_) {
    case GamePhase::kShowMapInfo:
      draw_map_info(ren);
      break;

    case GamePhase::kPlay:
      break;

    case GamePhase::kGameOver:
      break;
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

void GameScene::draw_mini_map(Renderer& ren) {
  const int total_h = kMiniMapBlockSize.h + (state_.show_mini_map ? kMiniMapSize.h : 0);
  Pos3i pos{10,ren.dimens().target_size.h - 10 - total_h};

  ren.wrap_color(mini_map_walkable_color_,[&]() {
    ren.draw_quad(pos,{kMiniMapSize.w,kMiniMapBlockSize.h});
  });
  assets_.font_renderer().wrap(ren,pos,0.33f,[&](auto& font) {
    font.print();

    auto orig_color = font.font_color;
    font.font_color = (map_.total_rescues() < map_.total_cells()) ? mini_map_eko_color_ : mini_map_end_color_;
    font.print(std::to_string(map_.total_rescues()));

    font.font_color = orig_color;
    font.print(Util::build_str('/',map_.total_cells()," ekos"));
  });

  if(!state_.show_mini_map) { return; }

  pos.y += kMiniMapBlockSize.h;

  const Pos2i player_pos = map_.player_pos();
  Pos3i block_pos = pos;

  for(int y = -kMiniMapHoodRadius.h; y <= kMiniMapHoodRadius.h; ++y,block_pos.y += kMiniMapBlockSize.h) {
    for(int x = -kMiniMapHoodRadius.w; x <= kMiniMapHoodRadius.w; ++x,block_pos.x += kMiniMapBlockSize.w) {
      Pos2i map_pos = player_pos;

      // "Rotate" the mini map according to the direction the player is facing.
      // - Remember that the grid is flipped vertically in Map for the Y calculations.
      switch(map_.player_facing()) {
        case Facing::kNorth:
          map_pos.x += x;
          map_pos.y -= y;
          break;

        case Facing::kSouth:
          map_pos.x -= x;
          map_pos.y += y;
          break;

        case Facing::kEast:
          map_pos.x -= y;
          map_pos.y -= x;
          break;

        case Facing::kWest:
          map_pos.x += y;
          map_pos.y += x;
          break;
      }

      const Space* space = map_.space(map_pos);
      const SpaceType type = (space != nullptr) ? space->type() : SpaceType::kNil;
      Color4f* color = &mini_map_walkable_color_;

      switch(type) {
        case SpaceType::kCell:
          color = &mini_map_eko_color_;
          break;

        case SpaceType::kEnd:
          color = &mini_map_end_color_;
          break;

        default:
          if(SpaceTypes::is_robot(type)) {
            color = &mini_map_robot_color_;
          } else if(SpaceTypes::is_non_walkable(type)) {
            color = &mini_map_non_walkable_color_;
          }
          break;
      }

      ren.begin_color(*color);
      ren.draw_quad(block_pos,kMiniMapBlockSize);

      if(x == 0 && y == 0) { // Player block?
        ren.begin_color(mini_map_eko_color_);
        ren.wrap_font_atlas(assets_.font_atlas(),block_pos,kMiniMapBlockSize,{},[&](auto& font) {
          font.print("↑");
        });
      }
    }

    block_pos.x = pos.x;
  }
  ren.end_color();
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
