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
#include "cybel/render/render_types.h"
#include "cybel/render/renderer.h"
#include "cybel/scene/scene_types.h"
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
  void draw(Renderer& ren,const Texture& texture);

  bool is_empty() const;

private:
  static const std::vector<Color4f> kColors;

  ViewDimens view_dimens_{};
  bool is_flying_ = false;
  std::vector<Particle> stars_{};

  void birth_star(Particle& star);
};

} // Namespace.
#endif
