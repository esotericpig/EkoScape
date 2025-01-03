/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_MAP_GRID_H_
#define EKOSCAPE_MAP_MAP_GRID_H_

#include "common.h"

#include "cybel/types/pos.h"
#include "cybel/types/size.h"

#include "map/space.h"

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
  Space& raw_space(const Pos2i& pos);
  Space& raw_space(const Pos3i& pos);
  const Space& raw_space(const Pos2i& pos) const;
  const Space& raw_space(const Pos3i& pos) const;

private:
  Size2i size_{};
  std::vector<Space> spaces_{};
};

} // Namespace.
#endif
