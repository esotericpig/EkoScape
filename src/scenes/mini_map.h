/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_MINI_MAP_H_
#define EKOSCAPE_SCENES_MINI_MAP_H_

#include "cybel/common.h"

#include "cybel/render/renderer.h"
#include "cybel/util/util.h"
#include "cybel/types.h"

#include "assets/assets.h"
#include "map/dantares_map.h"

namespace ekoscape {

class MiniMap {
public:
 explicit MiniMap(const Assets& assets);

 void draw(Renderer& ren,const DantaresMap& map,bool show_mini_map);

private:
 static const Size2i kHoodRadius;
 static const Size2i kBlockSize;
 static const Size2i kSize;
 static const std::uint8_t kAlpha = 127;

 const Assets& assets_;

 Color4f eko_color_{}; // Cell & Player.
 Color4f end_color_{};
 Color4f non_walkable_color_{};
 Color4f robot_color_{};
 Color4f walkable_color_{};
};

} // Namespace.
#endif
