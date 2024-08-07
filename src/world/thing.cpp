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

Thing::Thing(SpaceType type,int x,int y)
    : id_(++next_id_),type_(type),x_(x),y_(y) {}

bool Thing::operator<(const Thing& other) const { return id_ < other.id_; }

Thing& Thing::set_type(SpaceType type) {
  type_ = type;
  return *this;
}

Thing& Thing::set_pos(int x,int y) {
  x_ = x;
  y_ = y;
  return *this;
}

Thing& Thing::set_x(int x) {
  x_ = x;
  return *this;
}

Thing& Thing::set_y(int y) {
  y_ = y;
  return *this;
}

int Thing::id() const { return id_; }

SpaceType Thing::type() const { return type_; }

int Thing::x() const { return x_; }

int Thing::y() const { return y_; }

std::ostream& operator<<(std::ostream& out,const Thing& thing) {
  out << '{'
      << thing.id_ << ":'" << SpaceTypes::value_of(thing.type_) << "'"
      << ",(" << thing.x_ << ',' << thing.y_ << ')'
      << '}';

  return out;
}

} // Namespace.
