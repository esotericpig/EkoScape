/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_WORLD_ROBOT_H_
#define EKOSCAPE_WORLD_ROBOT_H_

#include "cybel/common.h"

#include "cybel/util/rando.h"
#include "cybel/types.h"

#include "map/dantares_map.h"
#include "thing.h"

namespace ekoscape {

class Robot : public Thing {
public:
  class MoveData {
  public:
    DantaresMap& map;
    Pos2i player_pos{};
    std::vector<std::unique_ptr<Robot>> new_robots{};

    explicit MoveData(DantaresMap& map);

    void refresh();
  };

  static const int kLikeGhost = 1 << 0; // Can go through walls.
  static const int kLikeSnake = 1 << 1; // Leaves behind a "shadow"/tail of statues.

  static const double kSnakeTailLifespan;

  using Thing::Thing;
  explicit Robot(SpaceType type,const Pos2i& pos,double lifespan = 0.0);

  virtual void move(MoveData& data) = 0;
  void age(double delta_time);

  bool is_alive() const;
  bool is_dead() const;

protected:
  double age_ = 0.0; // Between 0 and 1.
  double lifespan_ = 0.0; // In seconds.

  void move_like(int likes,MoveData& data);
  bool try_move(int x_vel,int y_vel,int likes,MoveData& data);
};

class RobotNormal : public Robot {
public:
  using Robot::Robot;

  void move(MoveData& data) override;
};

class RobotGhost : public Robot {
public:
  using Robot::Robot;

  void move(MoveData& data) override;
};

class RobotSnake : public Robot {
public:
  using Robot::Robot;

  void move(MoveData& data) override;
};

class RobotStatue : public Robot {
public:
  using Robot::Robot;

  explicit RobotStatue(const Pos2i& pos,double lifespan = 0.0);

  void move(MoveData& data) override;
};

class RobotWorm : public Robot {
public:
  using Robot::Robot;

  void move(MoveData& data) override;
};

} // Namespace.
#endif
