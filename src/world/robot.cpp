/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "robot.h"

namespace ekoscape {

const double Robot::kSnakeTailLifespan = 9.0; // In seconds.

Robot::MoveData::MoveData(DantaresMap& map)
    : map(map) {
  refresh();
}

void Robot::MoveData::refresh() {
  player_pos = map.player_pos();
}

Robot::Robot(SpaceType type,const Pos2i& pos,double lifespan)
    : Thing(type,pos),lifespan_(lifespan) {}

void Robot::move_like(int likes,MoveData& data) {
  bool on_player_x = (pos_.x == data.player_pos.x);
  bool on_player_y = (pos_.y == data.player_pos.y);

  if(on_player_x && on_player_y) { return; } // No need to move.

  int x_vel;
  int y_vel;

  if(pos_.x < data.player_pos.x) {
    x_vel = 1;
  } else if(pos_.x > data.player_pos.x) {
    x_vel = -1;
  } else {
    x_vel = Rando::it().rand_bool() ? 1 : -1;
  }
  if(pos_.y < data.player_pos.y) {
    y_vel = 1;
  } else if(pos_.y > data.player_pos.y) {
    y_vel = -1;
  } else {
    y_vel = Rando::it().rand_bool() ? 1 : -1;
  }

  // We can't move diagonally, since the player can't either (it's only fair),
  //     so pick either X or Y (not both), von-Neumann style.

  // We pick randomly, but instead, we could do by whichever one is farther away,
  //     but I like the randomness.
  bool try_x_first = Rando::it().rand_bool();

  // Try normally.
  if(try_x_first) {
    if(!on_player_x && try_move(x_vel,0,likes,data)) { return; }
    if(!on_player_y && try_move(0,y_vel,likes,data)) { return; }
  } else {
    if(!on_player_y && try_move(0,y_vel,likes,data)) { return; }
    if(!on_player_x && try_move(x_vel,0,likes,data)) { return; }
  }

  // Just make a move (if not stuck), so try the opposites.
  if(try_x_first) {
    if(on_player_x && try_move(x_vel,0,likes,data)) { return; }
    if(on_player_y && try_move(0,y_vel,likes,data)) { return; }
    if(try_move(-x_vel,0,likes,data)) { return; }
    if(try_move(0,-y_vel,likes,data)) { return; }
  } else {
    if(on_player_y && try_move(0,y_vel,likes,data)) { return; }
    if(on_player_x && try_move(x_vel,0,likes,data)) { return; }
    if(try_move(0,-y_vel,likes,data)) { return; }
    if(try_move(-x_vel,0,likes,data)) { return; }
  }
}

bool Robot::try_move(int x_vel,int y_vel,int likes,MoveData& data) {
  const Pos2i to_pos{pos_.x + x_vel,pos_.y + y_vel};
  const Space* to_space = data.map.space(to_pos);

  if(to_space == nullptr || to_space->has_thing()) { return false; }
  if(!(likes & kGhostLike) && to_space->is_non_walkable()) { return false; }

  Pos2i from_pos = pos_; // Store origin for snake's tail.

  if(!data.map.move_thing(from_pos,to_pos)) { return false; }
  pos_ = to_pos;

  if(likes & kSnakeLike) {
    if(data.map.place_thing(SpaceType::kRobotStatue,from_pos)) {
      data.new_robots.emplace_back(std::make_unique<RobotStatue>(from_pos,kSnakeTailLifespan));
    }
  }

  return true;
}

void Robot::age(double delta_time) {
  if(lifespan_ <= 0.0) { return; } // Besides immortality, prevent divide by 0.

  // Divide delta time by lifespan to normalize age between 0 and 1
  //     so that the age ends at 1.
  age_ += (delta_time / lifespan_);
}

bool Robot::is_alive() const { return lifespan_ <= 0.0 || age_ < 1.0; }

bool Robot::is_dead() const { return !is_alive(); }

void RobotNormal::move(MoveData& data) { move_like(0,data); }

void RobotGhost::move(MoveData& data) { move_like(kGhostLike,data); }

void RobotSnake::move(MoveData& data) { move_like(kSnakeLike,data); }

RobotStatue::RobotStatue(const Pos2i& pos,double lifespan)
    : Robot(SpaceType::kRobotStatue,pos,lifespan) {}

void RobotStatue::move(MoveData& /*data*/) {}

void RobotWorm::move(MoveData& data) { move_like(kGhostLike | kSnakeLike,data); }

} // Namespace.
