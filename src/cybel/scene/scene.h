/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_SCENE_SCENE_H_
#define CYBEL_SCENE_SCENE_H_

#include "cybel/common.h"

#include "cybel/render/render_types.h"
#include "cybel/render/renderer.h"
#include "scene_types.h"

namespace cybel {

class Scene {
public:
  /**
   * Return in update_scene_logic() to indicate no scene change (default).
   * Do not use this for your own scene types, else they'll be ignored.
   */
  static inline const int kNilType = 0;

  virtual ~Scene() noexcept = default;

  /**
   * If you know that all of your scenes will only either be in 2D or 3D, then you can call
   * begin_2d_scene()/begin_3d_scene() in init_scene() & resize_scene(), instead of in the
   * main loop with draw_scene().
   * - This also applies to: begin_*center(), begin_*scale(), begin_*offset().
   */
  virtual void init_scene(Renderer& /*ren*/) {}
  virtual void resize_scene(Renderer& /*ren*/,const ViewDimens& /*dimens*/) {}

  /**
   * Called when this scene is no longer the current scene (scene changed).
   * Should not delete objects in here (i.e., not a destructor).
   */
  virtual void on_scene_exit() {}

  virtual void on_key_down_event(SDL_Keycode /*key*/) {}
  virtual void handle_key_states(const Uint8* /*keys*/) {}
  virtual int update_scene_logic(const FrameStep& /*step*/,const ViewDimens& /*dimens*/) { return kNilType; }
  virtual void draw_scene(Renderer& /*ren*/) {}
};

} // Namespace.
#endif
