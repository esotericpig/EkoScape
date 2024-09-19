/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "map_grid.h"

namespace ekoscape {

MapGrid::MapGrid(const Size2i& size)
    : size_(size),spaces_(size.w * size.h,Space{}) {}

Space* MapGrid::space(const Pos2i& pos) {
  if(pos.x < 0 || pos.x >= size_.w || pos.y < 0 || pos.y >= size_.h) { return nullptr; }
  return &raw_space(pos);
}

Space& MapGrid::raw_space(const Pos2i& pos) { return spaces_.at(pos.x + (pos.y * size_.w)); }

const Size2i& MapGrid::size() const { return size_; }

const Space* MapGrid::space(const Pos2i& pos) const {
  if(pos.x < 0 || pos.x >= size_.w || pos.y < 0 || pos.y >= size_.h) { return nullptr; }
  return &raw_space(pos);
}

const Space& MapGrid::raw_space(const Pos2i& pos) const { return spaces_.at(pos.x + (pos.y * size_.w)); }

} // Namespace.
