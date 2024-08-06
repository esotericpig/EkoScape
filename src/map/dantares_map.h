/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_DANTARES_MAP_H_
#define EKOSCAPE_MAP_DANTARES_MAP_H_

#include "core/common.h"

#include "core/ekoscape_error.h"
#include "core/util.h"

#include "facing.h"
#include "map.h"
#include "space.h"
#include "space_type.h"

#include <functional>

namespace ekoscape {

/**
 * Bridge between Map and Dantares so that Map is independent of Dantares
 * and to ease development using both together.
 *
 * Example:
 *   DantaresMap map{dantares};
 *
 *   map.load_file("map.txt"); // Or build in memory w/ parse_grid().
 *
 *   map.add_to_dantares();
 *   map.make_current_in_dantares();
 *
 *   // Set textures appropriately in Dantares.
 *
 *   map.generate_in_dantares();
 *
 *   // Use map normally.
 */
class DantaresMap : public Map {
public:
  using SpaceCallback = std::function<void(int x,int y,Space&,SpaceType)>;

  DantaresMap(Dantares& dantares);

  Map& clear_spaces() override;

  DantaresMap& add_to_dantares(const SpaceCallback& on_space = nullptr);
  DantaresMap& delete_from_dantares();

  DantaresMap& make_current_in_dantares();
  DantaresMap& generate_in_dantares();

  bool move_thing(int from_x,int from_y,int to_x,int to_y) override;
  bool remove_thing(int x,int y) override;
  bool place_thing(SpaceType type,int x,int y) override;
  bool unlock_cell(int x,int y) override;

  bool set_space(int x,int y,SpaceType empty_type,SpaceType thing_type) override;
  bool set_empty(int x,int y,SpaceType type) override;
  bool set_thing(int x,int y,SpaceType type) override;

  int id() const;
  int player_x() const;
  int player_y() const;
  const Space* player_space() const;
  SpaceType player_space_type() const;
  Facing player_facing() const;

private:
  using Base = Map;

  Dantares& dantares_;
  int id_ = -1;

  void change_square(int x,int y,SpaceType type);
};

} // Namespace.
#endif
