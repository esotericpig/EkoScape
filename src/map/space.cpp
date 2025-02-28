/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "space.h"

namespace ekoscape {

Space::Space(SpaceType empty) noexcept
  : empty_type_(empty) {}

Space::Space(SpaceType empty,SpaceType thing) noexcept
  : empty_type_(empty),thing_type_(thing) {}

Space& Space::set(SpaceType empty,SpaceType thing) {
  empty_type_ = empty;
  thing_type_ = thing;

  return *this;
}

SpaceType Space::set_empty(SpaceType empty) {
  const SpaceType old_empty = empty_type_;
  empty_type_ = empty;

  return old_empty;
}

SpaceType Space::set_thing(SpaceType thing) {
  const SpaceType old_thing = thing_type_;
  thing_type_ = thing;

  return old_thing;
}

SpaceType Space::remove_thing() {
  const SpaceType old_thing = thing_type_;
  thing_type_ = SpaceType::kNil;

  return old_thing;
}

SpaceType Space::type() const {
  return has_thing() ? thing_type_ : empty_type_;
}

SpaceType Space::empty_type() const { return empty_type_; }

SpaceType Space::thing_type() const { return thing_type_; }

bool Space::has_thing() const { return thing_type_ != SpaceType::kNil; }

bool Space::has_robot() const { return SpaceTypes::is_robot(thing_type_); }

bool Space::is_portal() const { return SpaceTypes::is_portal(empty_type_); }

bool Space::is_walkable() const { return SpaceTypes::is_walkable(empty_type_); }

bool Space::is_non_walkable() const { return SpaceTypes::is_non_walkable(empty_type_); }

} // Namespace.
