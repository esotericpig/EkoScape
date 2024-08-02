/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_SPACE_H_
#define EKOSCAPE_MAP_SPACE_H_

#include "core/common.h"

#include "space_type.h"

namespace ekoscape {

class Space {
public:
  Space();
  Space(SpaceType empty_type);
  Space(SpaceType empty_type,SpaceType thing_type);

  /**
   * Returns previous thing, if there was one.
   */
  SpaceType place_thing(SpaceType type);

  /**
   * Returns previous thing, if there was one.
   */
  SpaceType remove_thing();

  SpaceType type() const;
  SpaceType empty_type() const;
  SpaceType thing_type() const;

  bool has_thing() const;
  bool is_walkable() const;
  bool is_non_walkable() const;

private:
  SpaceType empty_type_ = SpaceType::kEmpty;
  SpaceType thing_type_ = SpaceType::kNil;
};

} // Namespace.
#endif
