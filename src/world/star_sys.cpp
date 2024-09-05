/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "star_sys.h"

namespace ekoscape {

// This cannot be empty!
const std::vector<Color4f> StarSys::kColors = {
  Color4f::hex(0x0000ff), // Blue.
  Color4f::hex(0x00ffff), // Cyan.
  Color4f::hex(0xff00ff), // Purple.
  Color4f::hex(0xffffff), // White.
  Color4f::hex(0xffff00), // Yellow.
};

Color4f StarSys::rand_color() {
  return kColors.at(Rando::it().rand_sizet(kColors.size()));
}

void StarSys::init(const ViewDimens& view_dimens) {
  view_dimens_ = view_dimens;
  stars_.resize(100);

  for(auto& star: stars_) {
    birth_star(star);

    // The following logic is only on init, not birth, else stars appear "popping" in & out.
    star.age = Rando::it().rand_float();
    if(Rando::it().rand_bool()) { star.fade().past_lives = 1; }
  }
}

void StarSys::birth_star(Particle& star) {
  auto& r = Rando::it();

  star.lifespan = r.rand_float(5.0f,10.0f);

  star.pos.x = static_cast<float>(r.rand_int(view_dimens_.target_size.w));
  star.pos.y = static_cast<float>(r.rand_int(view_dimens_.target_size.h));
  star.pos_vel.x = (r.rand_float() < 0.40f) ? r.rand_float_vel(20.0f) : 0.0f;
  star.pos_vel.y = (r.rand_float() < 0.40f) ? r.rand_float_vel(20.0f) : 0.0f;
  star.spin_angle = r.rand_float(360.0f);
  star.spin_vel = (r.rand_float() < 0.70f) ? r.rand_float_vel(60.0f) : 0.0f;

  star.baby_size.w = r.rand_float(10.0f,25.0f);
  star.baby_size.h = star.baby_size.w;
  star.elder_size.w = r.rand_float(75.0f,100.0f);
  star.elder_size.h = star.elder_size.w;
  star.baby_color = rand_color();
  star.baby_color.a = 0.1f;
  star.elder_color = rand_color();
  star.elder_color.a = 1.0f;

  star.birth();
}

void StarSys::clear() {
  if(stars_.empty()) { return; }

  stars_.clear();
  stars_.shrink_to_fit();
}

void StarSys::update(const FrameStep& step) {
  for(auto& star: stars_) {
    if(star.is_alive()) {
      star.age_by(static_cast<float>(step.delta_time));
    } else if(star.past_lives < 1) {
      star.fade();

      if(Rando::it().rand_bool()) { star.pos_vel.x = -star.pos_vel.x; }
      if(Rando::it().rand_bool()) { star.pos_vel.y = -star.pos_vel.y; }
      if(Rando::it().rand_bool()) { star.spin_vel = -star.spin_vel; }

      star.rebirth();
    } else {
      birth_star(star.recreate());
    }
  }
}

void StarSys::draw(Renderer& ren,const Texture& texture) {
  if(stars_.empty()) { return; }

  ren.wrap_texture(texture,[&](auto& tex) {
    for(auto& star: stars_) {
      ren.wrap_rotate(star.pos.to_pos3<int>(),star.spin_angle,[&]() {
        ren.begin_color(star.color);
        tex.draw_quad(star.render_pos.to_pos3<int>(),star.size.to_size2<int>());
      });
    }
  });
  ren.end_color();
}

bool StarSys::is_empty() const { return stars_.empty(); }

} // Namespace.
