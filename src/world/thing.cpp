/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "thing.h"

namespace ekoscape {

int Thing::next_id_ = -1;

Thing::Thing() {}

Thing::Thing(SpaceType type,const Pos2i& pos)
    : id_(++next_id_),type_(type),pos_(pos) {}

bool Thing::operator<(const Thing& other) const { return id_ < other.id_; }

Thing& Thing::set_type(SpaceType type) {
  type_ = type;
  return *this;
}

Thing& Thing::set_pos(int x,int y) {
  pos_.x = x;
  pos_.y = y;
  return *this;
}

Thing& Thing::set_x(int x) {
  pos_.x = x;
  return *this;
}

Thing& Thing::set_y(int y) {
  pos_.y = y;
  return *this;
}

int Thing::id() const { return id_; }

SpaceType Thing::type() const { return type_; }

const Pos2i& Thing::pos() const { return pos_; }

std::ostream& operator<<(std::ostream& out,const Thing& thing) {
  out << '{'
      << thing.id_ << ":'" << SpaceTypes::value_of(thing.type_) << "'"
      << ",(" << thing.pos_.x << ',' << thing.pos_.y << ')'
      << '}';

  return out;
}

} // Namespace.
