/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "map_grid.h"

namespace ekoscape {

MapGrid::MapGrid(const Size2i& size)
  : size_(size),spaces_(size_.w * size_.h,Space{}) {}

const Size2i& MapGrid::size() const { return size_; }

Space* MapGrid::space(const Pos2i& pos) {
  if(pos.x < 0 || pos.x >= size_.w || pos.y < 0 || pos.y >= size_.h) { return nullptr; }

  return &unsafe_space(pos);
}

Space* MapGrid::space(const Pos3i& pos) {
  if(pos.x < 0 || pos.x >= size_.w || pos.y < 0 || pos.y >= size_.h) { return nullptr; }

  return &unsafe_space(pos);
}

const Space* MapGrid::space(const Pos2i& pos) const {
  if(pos.x < 0 || pos.x >= size_.w || pos.y < 0 || pos.y >= size_.h) { return nullptr; }

  return &unsafe_space(pos);
}

const Space* MapGrid::space(const Pos3i& pos) const {
  if(pos.x < 0 || pos.x >= size_.w || pos.y < 0 || pos.y >= size_.h) { return nullptr; }

  return &unsafe_space(pos);
}

Space& MapGrid::unsafe_space(const Pos2i& pos) { return spaces_.at(pos.x + (pos.y * size_.w)); }

Space& MapGrid::unsafe_space(const Pos3i& pos) { return spaces_.at(pos.x + (pos.y * size_.w)); }

const Space& MapGrid::unsafe_space(const Pos2i& pos) const { return spaces_.at(pos.x + (pos.y * size_.w)); }

const Space& MapGrid::unsafe_space(const Pos3i& pos) const { return spaces_.at(pos.x + (pos.y * size_.w)); }

} // namespace ekoscape
