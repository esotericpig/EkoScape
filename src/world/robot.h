/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_WORLD_ROBOT_H_
#define EKOSCAPE_WORLD_ROBOT_H_

#include "common.h"

#include "cybel/util/rando.h"

#include "map/dantares_map.h"

namespace ekoscape {

class Robot {
public:
  class MoveData {
  public:
    DantaresMap& map;
    Pos2i player_pos{};
    std::vector<Robot> new_robots{};

    explicit MoveData(DantaresMap& map);

    void refresh();
  };

  static const int kLikeStatue = 1 << 0; // No movement.
  static const int kLikeNormal = 1 << 1;
  static const int kLikeGhost = 1 << 2; // Can go through walls.
  static const int kLikeSnake = 1 << 3; // Leaves behind a "shadow"/tail of statues.

  static const double kSnakeTailLifespan;

  static Robot build_statue(const Pos2i& pos,double lifespan = 0.0);
  static Robot build_normal(const Pos2i& pos,double lifespan = 0.0);
  static Robot build_ghost(const Pos2i& pos,double lifespan = 0.0);
  static Robot build_snake(const Pos2i& pos,double lifespan = 0.0);
  static Robot build_worm(const Pos2i& pos,double lifespan = 0.0);

  void move(MoveData& data);
  void age(double delta_time);

  bool is_alive() const;
  bool is_dead() const;
  const Pos2i& pos() const;

private:
  Pos2i pos_{};
  int moves_like_ = 0;
  double lifespan_ = 0.0; // Seconds.
  double age_ = 0.0;

  explicit Robot(const Pos2i& pos,int moves_like,double lifespan = 0.0);

  bool try_move(int x_vel,int y_vel,MoveData& data);
};

} // Namespace.
#endif
