/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_DANTARES_MAP_H_
#define EKOSCAPE_MAP_DANTARES_MAP_H_

#include "cybel/common.h"

#include "cybel/util/cybel_error.h"
#include "cybel/util/util.h"
#include "cybel/types.h"

#include "facing.h"
#include "map.h"
#include "space.h"
#include "space_type.h"

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
  explicit DantaresMap(Dantares& dantares);

  Map& clear_spaces() override;

  DantaresMap& add_to_dantares();
  DantaresMap& delete_from_dantares();

  DantaresMap& make_current_in_dantares();
  DantaresMap& generate_in_dantares();

  bool move_thing(const Pos2i& from_pos,const Pos2i& to_pos) override;
  bool remove_thing(const Pos2i& pos) override;
  bool place_thing(SpaceType type,const Pos2i& pos) override;
  bool unlock_cell(const Pos2i& pos) override;

  bool set_player_pos();
  bool set_player_pos(const Pos2i& pos);
  bool set_space(const Pos2i& pos,SpaceType empty_type,SpaceType thing_type) override;
  bool set_empty(const Pos2i& pos,SpaceType type) override;
  bool set_thing(const Pos2i& pos,SpaceType type) override;

  int id() const;
  Pos2i player_pos() const;
  int player_x() const;
  int player_y() const;
  const Space* player_space() const;
  SpaceType player_space_type() const;
  Facing player_facing() const;

private:
  using Base = Map;

  Dantares& dantares_;
  int id_ = -1;

  void change_square(const Pos2i& pos,SpaceType type);
};

} // Namespace.
#endif
