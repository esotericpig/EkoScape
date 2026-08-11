/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_WORLD_STAR_SYS_H_
#define EKOSCAPE_WORLD_STAR_SYS_H_

#include "ekoscape/global.h"

#include <cybel/chrono/frame_delta.h>
#include <cybel/gfx/color.h>
#include <cybel/gfx/texture.h>
#include <cybel/renderer/renderer.h>
#include <cybel/renderer/viewport.h>
#include <cybel/vfx/particle.h>

#include <array>
#include <vector>

namespace ekoscape {

class StarSys final {
public:
  static Color4f rand_color();

  void init(const Viewport& view,bool is_flying = false);
  /// Free up memory for scenes that don't need stars.
  void clear();

  void update(const FrameDelta& delta,const Viewport& view);
  void draw(Renderer& ren,const Texture& tex);

  bool is_empty() const;

private:
  static constexpr std::array<Color4f,7> kColors{
    Color4f::kBlue,
    Color4f::kBrown,
    Color4f::kCyan,
    Color4f::kPurple,
    Color4f::kRed,
    Color4f::kWhite,
    Color4f::kYellow,
  };

  bool is_flying_ = false;
  std::vector<Particle> stars_{};

  void birth_star(Particle& star,const Viewport& view);
};

} // namespace ekoscape
#endif
