/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "particle.h"

namespace cybel {

Particle& Particle::birth() { return age_by(0.0f); }

Particle& Particle::die() {
  age = 1.1f;

  return *this;
}

Particle& Particle::age_by(float delta_time) {
  if(lifespan == 0.0f) { return *this; } // Prevent divide by 0.

  age += (delta_time / lifespan); // Divide by lifespan to normalize to [0,1].

  const float cage = clamped_age(); // Prevent invalid values, Nicolas Cage style.

  pos.x += (pos_vel.x * delta_time);
  pos.y += (pos_vel.y * delta_time);
  pos.z += (pos_vel.z * delta_time);
  render_pos.x = pos.x - (size.w / 2.0f);
  render_pos.y = pos.y - (size.h / 2.0f);
  render_pos.z = pos.z;
  spin_angle += (spin_vel * delta_time);

  size.w = baby_size.w + ((elder_size.w - baby_size.w) * cage);
  size.h = baby_size.h + ((elder_size.h - baby_size.h) * cage);
  color.r = baby_color.r + ((elder_color.r - baby_color.r) * cage);
  color.g = baby_color.g + ((elder_color.g - baby_color.g) * cage);
  color.b = baby_color.b + ((elder_color.b - baby_color.b) * cage);
  color.a = baby_color.a + ((elder_color.a - baby_color.a) * cage);

  return *this;
}

Particle& Particle::rebirth() {
  age = 0.0f;
  ++past_lives;

  return birth();
}

Particle& Particle::recreate() {
  age = 0.0f;
  past_lives = 0;

  return *this;
}

Particle& Particle::fade() {
  std::swap(baby_size,elder_size);
  std::swap(baby_color,elder_color);

  return *this;
}

Particle& Particle::bounce() {
  pos_vel.x = -pos_vel.x;
  pos_vel.y = -pos_vel.y;
  pos_vel.z = -pos_vel.z;
  spin_vel = -spin_vel;

  return *this;
}

bool Particle::is_alive() const { return age <= 1.0f; }

bool Particle::is_dead() const { return !is_alive(); }

float Particle::clamped_age() const { return std::clamp(age,0.0f,1.0f); }

} // namespace cybel
