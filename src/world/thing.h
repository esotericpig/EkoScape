/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_WORLD_THING_H_
#define EKOSCAPE_WORLD_THING_H_

#include "core/common.h"

#include "map/space_type.h"

namespace ekoscape {

class Thing {
public:
  Thing();
  Thing(SpaceType type,int x,int y);
  virtual ~Thing() = default;

  /**
   * So can be used in `std::set`.
   */
  bool operator<(const Thing& other) const;

  Thing& set_type(SpaceType type);
  Thing& set_pos(int x,int y);
  Thing& set_x(int x);
  Thing& set_y(int y);

  int id() const;
  SpaceType type() const;
  int x() const;
  int y() const;

  friend std::ostream& operator<<(std::ostream& out,const Thing& thing);

protected:
  int id_ = -1;
  SpaceType type_ = SpaceType::kNil;
  int x_ = -1;
  int y_ = -1;

private:
  static int next_id_;
};

} // Namespace.
#endif
