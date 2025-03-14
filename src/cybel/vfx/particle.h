/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_VFX_PARTICLE_H_
#define CYBEL_VFX_PARTICLE_H_

#include "cybel/common.h"

#include "cybel/types/color.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"

namespace cybel {

/**
 * For birth, you only need to set:
 * - lifespan
 * - pos
 * - (optional) pos_vel
 * - (optional) spin_angle, spin_vel
 * - (optional) baby_size, elder_size
 * - (optional) baby_color, elder_color
 *
 * The rest of the vars are updated by the other functions.
 */
class Particle {
public:
  float lifespan{};
  float age{};
  int past_lives{};

  /**
   * Use this for drawing so that the size changes around a center point.
   * Else, the size fans out towards the bottom right unnaturally.
   *
   * For rotation, you should use `pos` instead, since it's the center.
   */
  Pos3f render_pos{};

  /**
   * This is the center position. Use this for rotation, not for drawing.
   */
  Pos3f pos{};
  Pos3f pos_vel{};
  float spin_angle{};
  float spin_vel{};

  Size2f size{};
  Size2f baby_size{110.0f,110.0f};
  Size2f elder_size{110.0f,110.0f};
  Color4f color{};
  Color4f baby_color{1.0f};
  Color4f elder_color{1.0f};

  virtual ~Particle() noexcept = default;

  Particle& birth();
  Particle& die();
  Particle& age_by(float delta_time);

  Particle& rebirth();
  Particle& recreate();
  Particle& fade();
  Particle& bounce();

  bool is_alive() const;
  bool is_dead() const;

  float calc_clamped_age() const;
};

} // namespace cybel
#endif
