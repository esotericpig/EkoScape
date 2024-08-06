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
  struct Dimens {
    int width;
    int height;
    int target_width;
    int target_height;
    int init_width;
    int init_height;
    float view_scale;
  };

  struct FrameStep {
    const Duration& dpf;
    double delta_time;
  };

  virtual ~Scene() = default;

  virtual void init_scene() {}

  /**
   * If you know that all of your scenes will only either be in 2D or 3D, then you can use
   * this method to call `begin_2d_scene()` or `begin_3d_scene()`, instead of calling it
   * inside of the loop.
   */
  virtual void resize_scene(Dimens /*dimens*/) {}

  virtual void handle_key_down_event(SDL_Keycode /*key*/) {}
  virtual void handle_key_states(const Uint8* /*keys*/) {}
  virtual int update_scene_logic(FrameStep /*step*/) { return 0; }
  virtual void draw_scene(Dimens /*dimens*/) {}
};

} // Namespace.
#endif
