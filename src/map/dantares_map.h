/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_DANTARES_MAP_H_
#define EKOSCAPE_MAP_DANTARES_MAP_H_

#include "common.h"

#include "cybel/util/cybel_error.h"
#include "cybel/util/util.h"

#include "facing.h"
#include "map.h"
#include "space.h"
#include "space_type.h"

#include <functional>
#include <vector>

namespace ekoscape {

/**
 * Bridge between Map and Dantares so that Map is independent of Dantares
 * and to ease development using both together.
 *
 * Example:
 *   DantaresMap map{dantares};
 *
 *   map.load_file("map.txt"); // Or build in code w/ parse_grid().
 *
 *   map.add_to_dantares([&](Dantares& d) {
 *     // Set textures appropriately in Dantares.
 *   });
 *
 *   // Use map normally.
 */
class DantaresMap : public Map {
public:
  using TexturesSetter = std::function<void(Dantares&,int z,int id)>;

  explicit DantaresMap(Dantares& dantares);

  Map& clear_grids() override;

  void add_to_dantares(const TexturesSetter& set_textures = nullptr);

  bool change_grid(int z) override;
  bool move_thing(const Pos3i& from_pos,const Pos3i& to_pos) override;
  bool remove_thing(const Pos3i& pos) override;
  bool place_thing(SpaceType type,const Pos3i& pos) override;
  bool unlock_cell(const Pos3i& pos) override;

  bool set_player_pos();
  bool set_player_pos(const Pos3i& pos);
  bool set_space(const Pos3i& pos,SpaceType empty_type,SpaceType thing_type) override;
  bool set_empty(const Pos3i& pos,SpaceType type) override;
  bool set_thing(const Pos3i& pos,SpaceType type) override;

  Pos3i player_pos() const;
  const Space* player_space() const;
  SpaceType player_space_type() const;
  Facing player_facing() const;

private:
  using Base = Map;

  Dantares& dantares_;
  std::vector<int> grid_ids_{};

  bool change_grid(int z,bool force);
  void change_square(const Pos2i& pos,SpaceType type);
  void change_square(const Pos3i& pos,SpaceType type);
};

} // Namespace.
#endif
