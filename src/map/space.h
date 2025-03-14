/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_SPACE_H_
#define EKOSCAPE_MAP_SPACE_H_

#include "common.h"

#include "map/space_type.h"

namespace ekoscape {

class Space {
public:
  explicit Space() noexcept = default;
  explicit Space(SpaceType empty) noexcept;
  explicit Space(SpaceType empty,SpaceType thing) noexcept;

  Space& set(SpaceType empty,SpaceType thing);

  /**
   * Returns previous empty, if there was one.
   */
  SpaceType set_empty(SpaceType empty);

  /**
   * Returns previous thing, if there was one.
   */
  SpaceType set_thing(SpaceType thing);

  /**
   * Returns thing, if there was one.
   */
  SpaceType remove_thing();

  SpaceType type() const;
  SpaceType empty_type() const;
  SpaceType thing_type() const;

  bool has_thing() const;
  bool has_robot() const;
  bool is_portal() const;
  bool is_walkable() const;
  bool is_non_walkable() const;

private:
  SpaceType empty_type_ = SpaceType::kEmpty;
  SpaceType thing_type_ = SpaceType::kNil;
};

} // namespace ekoscape
#endif
