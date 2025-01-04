/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_WORLD_ROBOT_H_
#define EKOSCAPE_WORLD_ROBOT_H_

#include "common.h"

#include "cybel/types/pos.h"

#include "map/map.h"
#include "map/space.h"
#include "map/space_type.h"

#include <vector>

namespace ekoscape {

class Robot {
public:
  class MoveData {
  public:
    Map& map;
    Pos3i player_pos{};
    bool player_ate_fruit = false;
    std::vector<Robot> new_robots{};

    explicit MoveData(Map& map);

    void refresh(bool player_ate_fruit);
  };

  static Robot build_statue(const Pos3i& pos,float lifespan = kDefaultLifespan);
  static Robot build_normal(const Pos3i& pos,float lifespan = kDefaultLifespan);
  static Robot build_ghost(const Pos3i& pos,float lifespan = kDefaultLifespan);
  static Robot build_snake(const Pos3i& pos,float lifespan = kDefaultLifespan);
  static Robot build_worm(const Pos3i& pos,float lifespan = kDefaultLifespan);

  bool move(MoveData& data);
  bool warp_to(MoveData& data,const Pos3i& to_pos);
  void age(double delta_time);

  bool is_alive() const;
  bool is_dead() const;
  const Pos3i& pos() const;
  SpaceType portal_type() const;
  bool warped() const;
  bool can_move_to(const Space* space) const;

private:
  static inline const int kLikeStatue = 1 << 0; // No movement.
  static inline const int kLikeNormal = 1 << 1;
  static inline const int kLikeGhost = 1 << 2; // Can go through walls.
  static inline const int kLikeSnake = 1 << 3; // Leaves behind a "shadow"/tail of statues.

  static inline const float kDefaultLifespan{};
  static inline const float kSnakeTailLifespan = 9.0f;
  static inline std::vector<Pos2i> rand_move_vels_{
    Pos2i{ 0,-1}, // North.
    Pos2i{ 0, 1}, // South.
    Pos2i{ 1, 0}, // East.
    Pos2i{-1, 0}, // West.
  };

  Pos3i pos_{};
  int moves_like_{};
  float lifespan_{};
  float age_{};

  Pos3i last_seen_player_pos_{0,0,-1};
  SpaceType portal_type_ = SpaceType::kNil;
  bool warped_ = false;

  explicit Robot(const Pos3i& pos,int moves_like,float lifespan);

  bool move_smart(MoveData& data);
  bool move_rand(MoveData& data);
  bool try_move(MoveData& data,int x_vel,int y_vel);
};

} // Namespace.
#endif
