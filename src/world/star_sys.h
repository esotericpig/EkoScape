/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_WORLD_STAR_SYS_H_
#define EKOSCAPE_WORLD_STAR_SYS_H_

#include "common.h"

#include "cybel/gfx/texture.h"
#include "cybel/render/renderer.h"
#include "cybel/types/frame_step.h"
#include "cybel/util/rando.h"
#include "cybel/vfx/particle.h"

#include <vector>

namespace ekoscape {

class StarSys {
public:
  static Color4f rand_color();

  void init(const ViewDimens& view_dimens,bool is_flying = false);

  /**
   * Free up memory for scenes that don't need stars.
   */
  void clear();

  void update(const FrameStep& step);
  void draw(Renderer& ren,const Texture& tex);

  bool is_empty() const;

private:
  // Cannot be empty!
  static inline const std::vector<Color4f> kColors = {
    Color4f::hex(0x0000ff), // Blue.
    Color4f::hex(0x00ffff), // Cyan.
    Color4f::hex(0xff00ff), // Purple.
    Color4f::hex(0xffffff), // White.
    Color4f::hex(0xffff00), // Yellow.
  };

  ViewDimens view_dimens_{};
  bool is_flying_ = false;
  std::vector<Particle> stars_{};

  void birth_star(Particle& star);
};

} // Namespace.
#endif
