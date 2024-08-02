/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_WORLD_ROBOT_H_
#define EKOSCAPE_WORLD_ROBOT_H_

#include "core/common.h"

#include "core/util.h"

#include "map/map.h"

#include "thing.h"

namespace ekoscape {

class Robot : public Thing {
public:
  class MoveData {
  public:
    Dantares& dantares;
    Map& map;
    int player_x = 0;
    int player_y = 0;
    std::vector<std::unique_ptr<Robot>> new_robots{};

    MoveData(Dantares& dantares,Map& map);

    void refresh();
  };

  static const int kGhostLike = 1 << 0; // Can go through walls.
  static const int kSnakeLike = 1 << 1; // Leaves behind a "shadow"/trail of RobotStatue.

  using Thing::Thing;
  Robot(SpaceType type,int x,int y,double lifespan = 0.0);

  virtual void move(MoveData& data) = 0;
  void age(double delta_time);

  bool is_alive() const;
  bool is_dead() const;

protected:
  double age_ = 0.0; // Between 0 and 1.
  double lifespan_ = 0.0; // In seconds.

  void move_like(int likes,MoveData& data);
  bool try_move(int to_x,int to_y,int likes,MoveData& data);
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

  void move(MoveData& data) override;
};

class RobotWorm : public Robot {
public:
  using Robot::Robot;

  void move(MoveData& data) override;
};

} // Namespace.
#endif
