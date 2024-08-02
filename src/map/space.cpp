/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "space.h"

namespace ekoscape {

Space::Space() {}

Space::Space(SpaceType empty_type)
    : empty_type_(empty_type) {}

Space::Space(SpaceType empty_type,SpaceType thing_type)
    : empty_type_(empty_type),thing_type_(thing_type) {}

SpaceType Space::place_thing(SpaceType type) {
  SpaceType old_thing = thing_type_;
  thing_type_ = type;

  return old_thing;
}

SpaceType Space::remove_thing() {
  SpaceType old_thing = thing_type_;
  thing_type_ = SpaceType::kNil;

  return old_thing;
}

SpaceType Space::type() const {
  return (thing_type_ != SpaceType::kNil) ? thing_type_ : empty_type_;
}

SpaceType Space::empty_type() const { return empty_type_; }

SpaceType Space::thing_type() const { return thing_type_; }

bool Space::has_thing() const { return thing_type_ != SpaceType::kNil; }

bool Space::is_walkable() const { return SpaceTypes::is_walkable(empty_type_); }

bool Space::is_non_walkable() const { return SpaceTypes::is_non_walkable(empty_type_); }

} // Namespace.
