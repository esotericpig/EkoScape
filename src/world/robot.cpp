/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "robot.h"

#include "cybel/util/rando.h"

namespace ekoscape {

Robot::MoveData::MoveData(Map& map)
  : map(map) {}

void Robot::MoveData::refresh(bool player_ate_fruit) {
  player_pos = map.player_pos();
  this->player_ate_fruit = player_ate_fruit;
}

Robot Robot::build(SpaceType type,const Pos3i& pos,float lifespan) noexcept {
  switch(type) {
    case SpaceType::kRobotStatue: return build_statue(pos,lifespan);
    case SpaceType::kRobot: return build_normal(pos,lifespan);
    case SpaceType::kRobotGhost: return build_ghost(pos,lifespan);
    case SpaceType::kRobotSnake: return build_snake(pos,lifespan);
    case SpaceType::kRobotWorm: return build_worm(pos,lifespan);

    default:
      std::cerr << "[WARN] Tried to build invalid Robot type [" << SpaceTypes::value_of(type)
                << "]; building Normal instead." << std::endl;
      return build_normal(pos,lifespan);
  }
}

Robot Robot::build_statue(const Pos3i& pos,float lifespan) noexcept {
  return Robot{SpaceType::kRobotStatue,pos,kLikeStatue,lifespan};
}

Robot Robot::build_normal(const Pos3i& pos,float lifespan) noexcept {
  return Robot{SpaceType::kRobot,pos,kLikeNormal,lifespan};
}

Robot Robot::build_ghost(const Pos3i& pos,float lifespan) noexcept {
  return Robot{SpaceType::kRobotGhost,pos,kLikeGhost,lifespan};
}

Robot Robot::build_snake(const Pos3i& pos,float lifespan) noexcept {
  return Robot{SpaceType::kRobotSnake,pos,kLikeSnake,lifespan};
}

Robot Robot::build_worm(const Pos3i& pos,float lifespan) noexcept {
  return Robot{SpaceType::kRobotWorm,pos,kLikeSnake | kLikeGhost,lifespan};
}

Robot::Robot(SpaceType type,const Pos3i& pos,int moves_like,float lifespan) noexcept
  : type_(type),pos_(pos),moves_like_(moves_like),lifespan_(lifespan) {}

bool Robot::move(MoveData& data) {
  if(moves_like_ & kLikeStatue) { return false; }

  if(pos_.z == data.player_pos.z) {
    last_seen_player_pos_ = data.player_pos;
    return move_smart(data);
  }
  // Go towards the pos where we last saw the Player on our Z/grid.
  if(last_seen_player_pos_.z >= 0) {
    return move_smart(data);
  }

  return move_rand(data); // We haven't seen the Player on our Z/grid (recently).
}

bool Robot::move_smart(MoveData& data) {
  const bool on_player_x = (pos_.x == last_seen_player_pos_.x);
  const bool on_player_y = (pos_.y == last_seen_player_pos_.y);

  if(on_player_x && on_player_y) {
    // Move randomly again, to try to hit a Portal nearby if the last seen pos is off.
    last_seen_player_pos_.z = -1;
    return move_rand(data);
  }

  int x_vel;
  int y_vel;

  if(pos_.x < last_seen_player_pos_.x) {
    x_vel = 1;
  } else if(pos_.x > last_seen_player_pos_.x) {
    x_vel = -1;
  } else {
    x_vel = Rando::it().rand_bool() ? 1 : -1;
  }
  if(pos_.y < last_seen_player_pos_.y) {
    y_vel = 1;
  } else if(pos_.y > last_seen_player_pos_.y) {
    y_vel = -1;
  } else {
    y_vel = Rando::it().rand_bool() ? 1 : -1;
  }

  if(data.player_ate_fruit) {
    // Try to get away.
    x_vel = -x_vel;
    y_vel = -y_vel;
  }

  // We can't move diagonally, since the player can't either (it's only fair),
  //     so pick either X or Y (not both), von-Neumann style.

  // We pick randomly, but instead, we could do by whichever one is farther away,
  //     but I like the randomness.
  const bool try_x_first = Rando::it().rand_bool();

  // Try normally.
  if(try_x_first) {
    if(!on_player_x && try_move(data,x_vel,0)) { return true; }
    if(!on_player_y && try_move(data,0,y_vel)) { return true; }
  } else {
    if(!on_player_y && try_move(data,0,y_vel)) { return true; }
    if(!on_player_x && try_move(data,x_vel,0)) { return true; }
  }

  // Just make a move (if not stuck), so try the opposites.
  if(try_x_first) {
    if(on_player_x && try_move(data,x_vel,0)) { return true; }
    if(on_player_y && try_move(data,0,y_vel)) { return true; }
    if(try_move(data,-x_vel,0)) { return true; }
    if(try_move(data,0,-y_vel)) { return true; }
  } else {
    if(on_player_y && try_move(data,0,y_vel)) { return true; }
    if(on_player_x && try_move(data,x_vel,0)) { return true; }
    if(try_move(data,0,-y_vel)) { return true; }
    if(try_move(data,-x_vel,0)) { return true; }
  }

  return false;
}

bool Robot::move_rand(MoveData& data) {
  // ReSharper disable once CppDFAUnreachableCode
  if constexpr(rand_move_vels_.empty()) { return false; }

  // Try once without shuffling.
  const auto& rand_move_vel = rand_move_vels_[Rando::it().rand_sizet(rand_move_vels_.size())];

  if(try_move(data,rand_move_vel.x,rand_move_vel.y)) { return true; }

  Rando::it().shuffle(rand_move_vels_.begin(),rand_move_vels_.end());

  for(const auto& move_vel : rand_move_vels_) {
    if(try_move(data,move_vel.x,move_vel.y)) { return true; }
  }

  return false;
}

bool Robot::try_move(MoveData& data,int x_vel,int y_vel) {
  const Pos3i to_pos{pos_.x + x_vel,pos_.y + y_vel,pos_.z};
  const Space* to_space = data.map.space(to_pos);

  if(!can_move_to(to_space)) { return false; }
  if(!data.map.move_thing(pos_,to_pos)) { return false; }

  const auto from_pos = pos_; // Store origin for snake's tail.
  pos_ = to_pos;
  portal_type_ = to_space->is_portal() ? to_space->empty_type() : SpaceType::kNil;

  if(portal_type_ == SpaceType::kNil) {
    warped_ = false;

    if((moves_like_ & kLikeSnake) && data.map.place_thing(SpaceType::kRobotStatue,from_pos)) {
      // Grow tail.
      data.new_robots.push_back(build_statue(from_pos,kSnakeTailLifespan));
    }
  }

  return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Robot::warp_to(MoveData& data,const Pos3i& to_pos) {
  if(pos_ != to_pos) {
    if(!can_move_to(data.map.space(to_pos))) { return false; }
    if(!data.map.move_thing(pos_,to_pos)) { return false; }
  }

  pos_ = to_pos;
  warped_ = true;
  last_seen_player_pos_.z = -1; // Move randomly again, in case the Player isn't on this new Z/grid.

  return true;
}

void Robot::age(double delta_time) {
  if(lifespan_ <= 0.0f) { return; } // Besides immortality, prevent divide by 0.

  // Divide by lifespan to normalize to [0,1].
  age_ += (static_cast<float>(delta_time) / lifespan_);
}

void Robot::set_raw_pos(const Pos3i& pos) { pos_ = pos; }

bool Robot::is_alive() const { return lifespan_ <= 0.0f || age_ <= 1.0f; }

bool Robot::is_dead() const { return !is_alive(); }

SpaceType Robot::type() const { return type_; }

const Pos3i& Robot::pos() const { return pos_; }

SpaceType Robot::portal_type() const { return portal_type_; }

bool Robot::warped() const { return warped_; }

bool Robot::can_move_to(const Space* space) const {
  if(space == nullptr || space->has_thing()) { return false; }
  if(!(moves_like_ & kLikeGhost) && space->is_non_walkable()) { return false; }

  return true;
}

} // namespace ekoscape
