/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "robot.h"

namespace ekoscape {

Robot::MoveData::MoveData(Dantares& dantares,Map& map)
    : dantares(dantares),map(map) {
  refresh();
}

void Robot::MoveData::refresh() {
  player_x = dantares.GetPlayerX();
  player_y = dantares.GetPlayerY();
}

Robot::Robot(SpaceType type,int x,int y,double lifespan)
    : Thing(type,x,y),lifespan_(lifespan) {}

void Robot::move_like(int likes,MoveData& data) {
  bool on_player_x = (x_ == data.player_x);
  bool on_player_y = (y_ == data.player_y);

  if(on_player_x && on_player_y) { return; } // No need to move.

  int x_vel;
  int y_vel;

  if(x_ < data.player_x) {
    x_vel = 1;
  } else if(x_ > data.player_x) {
    x_vel = -1;
  } else {
    x_vel = Rando::it().rand_bool() ? 1 : -1;
  }
  if(y_ < data.player_y) {
    y_vel = 1;
  } else if(y_ > data.player_y) {
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
    if(!on_player_x && try_move(x_ + x_vel,y_,likes,data)) { return; }
    if(!on_player_y && try_move(x_,y_ + y_vel,likes,data)) { return; }
  } else {
    if(!on_player_y && try_move(x_,y_ + y_vel,likes,data)) { return; }
    if(!on_player_x && try_move(x_ + x_vel,y_,likes,data)) { return; }
  }

  // Just make a move (if not stuck), so try the opposites.
  if(try_x_first) {
    if(on_player_x && try_move(x_ + x_vel,y_,likes,data)) { return; }
    if(on_player_y && try_move(x_,y_ + y_vel,likes,data)) { return; }
    if(try_move(x_ - x_vel,y_,likes,data)) { return; }
    if(try_move(x_,y_ - y_vel,likes,data)) { return; }
  } else {
    if(on_player_y && try_move(x_,y_ + y_vel,likes,data)) { return; }
    if(on_player_x && try_move(x_ + x_vel,y_,likes,data)) { return; }
    if(try_move(x_,y_ - y_vel,likes,data)) { return; }
    if(try_move(x_ - x_vel,y_,likes,data)) { return; }
  }
}

bool Robot::try_move(int to_x,int to_y,int likes,MoveData& data) {
  Space* to_space = data.map.space(to_x,to_y);

  if(to_space == nullptr || to_space->has_thing()) { return false; }
  if(!(likes & kGhostLike) && to_space->is_non_walkable()) { return false; }

  int from_x = x_;
  int from_y = y_;

  if(!data.map.move_thing(from_x,from_y,to_x,to_y,data.dantares)) { return false; }

  set_pos(to_x,to_y);

  if(likes & kSnakeLike) {
    if(data.map.place_thing(SpaceType::kRobotStatue,from_x,from_y,data.dantares)) {
      data.new_robots.push_back(std::make_unique<RobotStatue>(
        SpaceType::kRobotStatue,from_x,from_y,Rando::it().rand_uint(3,11)
      ));
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

bool Robot::is_alive() const { return lifespan_ <= 0.0 || age_ <= 1.0; }

bool Robot::is_dead() const { return !is_alive(); }

void RobotNormal::move(MoveData& data) { move_like(0,data); }

void RobotGhost::move(MoveData& data) { move_like(kGhostLike,data); }

void RobotSnake::move(MoveData& data) { move_like(kSnakeLike,data); }

void RobotStatue::move(MoveData& /*data*/) {}

void RobotWorm::move(MoveData& data) { move_like(kGhostLike | kSnakeLike,data); }

} // Namespace.
