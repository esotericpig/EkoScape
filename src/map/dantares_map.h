/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_DANTARES_MAP_H_
#define EKOSCAPE_MAP_DANTARES_MAP_H_

#include "common.h"

#include "map/map.h"

#include <functional>
#include <vector>

namespace ekoscape {

/**
 * Bridge between Map and Dantares so that Map is independent of Dantares
 * and to ease development using both together.
 *
 * Example:
 *   @code
 *   DantaresMap map{dantares,[&](Dantares& dan,int z,int grid_id) {
 *     // Set textures appropriately in Dantares...
 *   }};
 *
 *   map.load_file("map.txt"); // Or build in code w/ parse_grid().
 *   map.add_to_bridge();
 *   // Now use map normally...
 *   @endcode
 */
class DantaresMap final : public Map {
public:
  using TexturesSetter = std::function<void(Dantares2&,int z,int grid_id)>;

  explicit DantaresMap(Dantares2& dantares,const TexturesSetter& set_texs);

  Map& clear_grids() override;
  void add_to_bridge() override;
  void on_context_restored() override;

  bool move_player(const Pos3i& pos) override;
  bool sync_player_pos() override;
  bool change_grid(int z) override;

  Pos3i player_pos() const override;
  const Space* player_space() const override;
  SpaceType player_space_type() const override;
  Facing player_facing() const override;

protected:
  void update_bridge_space(const Pos3i& pos,SpaceType type) override;
  void update_bridge_space(int x,int y,SpaceType type);

private:
  Dantares2& dantares_;
  TexturesSetter set_texs_{};
  std::vector<int> grid_ids_{};

  bool change_grid(int z,bool force);
};

} // namespace ekoscape
#endif
