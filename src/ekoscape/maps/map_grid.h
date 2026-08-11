/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAPS_MAP_GRID_H_
#define EKOSCAPE_MAPS_MAP_GRID_H_

#include "ekoscape/global.h"

#include "ekoscape/maps/space.h"

#include <cybel/math/pos.h>
#include <cybel/math/size.h>

#include <vector>

namespace ekoscape {

class MapGrid {
public:
  explicit MapGrid(const Size2i& size);

  const Size2i& size() const;
  Space* space(const Pos2i& pos);
  Space* space(const Pos3i& pos);
  const Space* space(const Pos2i& pos) const;
  const Space* space(const Pos3i& pos) const;

  Space& unsafe_space(const Pos2i& pos);
  Space& unsafe_space(const Pos3i& pos);
  const Space& unsafe_space(const Pos2i& pos) const;
  const Space& unsafe_space(const Pos3i& pos) const;

private:
  Size2i size_{};
  std::vector<Space> spaces_{};
};

} // namespace ekoscape
#endif
