/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_SCENE_H_
#define EKOSCAPE_CORE_SCENE_H_

#include "common.h"

#include "duration.h"

namespace ekoscape {

class Scene {
public:
  virtual ~Scene() = default;

  virtual void init_scene() {}
  virtual void handle_key_down_event(SDL_Keycode /*key*/) {}
  virtual void handle_key_states(const Uint8* /*keys*/) {}
  virtual int update_scene_logic(const Duration& /*last_dpf*/,double /*delta_time*/) { return 0; }
  virtual void draw_scene() {}
};

} // Namespace.
#endif
