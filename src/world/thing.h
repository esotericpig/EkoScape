/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_WORLD_THING_H_
#define EKOSCAPE_WORLD_THING_H_

#include "cybel/common.h"

#include "cybel/types.h"

#include "map/space_type.h"

namespace ekoscape {

using namespace cybel;

class Thing {
public:
  Thing();
  Thing(SpaceType type,const Pos2i& pos);
  virtual ~Thing() noexcept = default;

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
  const Pos2i& pos() const;

  friend std::ostream& operator<<(std::ostream& out,const Thing& thing);

protected:
  int id_ = -1;
  SpaceType type_ = SpaceType::kNil;
  Pos2i pos_{};

private:
  static int next_id_;
};

} // Namespace.
#endif
