/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_SCENE_H_
#define EKOSCAPE_CORE_SCENE_H_

#include "common.h"

#include "renderer.h"
#include "scene_data.h"

namespace ekoscape {

class Scene {
public:
  virtual ~Scene() noexcept = default;

  /**
   * If you know that all of your scenes will only either be in 2D or 3D, then you can call
   * begin_2d_scene()/begin_3d_scene() in init_scene() & resize_scene(), instead of in the
   * main loop with draw_scene().
   * - This also applies to: begin_*center(), begin_*scale(), begin_*offset().
   */
  virtual void init_scene(Renderer& /*ren*/) {}
  virtual void resize_scene(Renderer& /*ren*/,const ViewDimens& /*dimens*/) {}

  virtual void on_key_down_event(SDL_Keycode /*key*/) {}
  virtual void handle_key_states(const Uint8* /*keys*/) {}
  virtual int update_scene_logic(const FrameStep& /*step*/) { return 0; }
  virtual void draw_scene(Renderer& /*ren*/) {}
};

} // Namespace.
#endif
