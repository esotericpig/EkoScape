/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_SCENE_SCENE_H_
#define CYBEL_SCENE_SCENE_H_

#include "cybel/common.h"

#include "cybel/gfx/renderer.h"
#include "cybel/types/frame_step.h"
#include "cybel/types/view_dimens.h"

#include <vector>

namespace cybel {

class Scene {
public:
  /**
   * Return in update_scene_logic() to indicate no scene change (default).
   * Do not use this for your own scene types, else they'll be ignored.
   */
  static inline const int kNilType = 0;

  virtual ~Scene() noexcept = default;

  virtual void init_scene(const ViewDimens& /*dimens*/) {}

  /**
   * If you know that all of your scenes will only either be in 2D or 3D, then you can call
   * begin_2d_scene()/begin_3d_scene() in resize_scene(), instead of in the main loop with draw_scene().
   * - This also applies to: begin_*center(), begin_*scale(), begin_*offset(), etc.
   */
  virtual void resize_scene(Renderer& /*ren*/,const ViewDimens& /*dimens*/) {}

  /**
   * Called when this scene is no longer the current scene (scene changed).
   * Should not delete objects in here (i.e., not a destructor).
   */
  virtual void on_scene_exit() {}

  virtual void on_input_event(int /*id*/,const ViewDimens& /*dimens*/) {}
  virtual void handle_input_states(const std::vector<bool>& /*states*/,const ViewDimens& /*dimens*/) {}

  virtual int update_scene_logic(const FrameStep& /*step*/,const ViewDimens& /*dimens*/) { return kNilType; }
  virtual void draw_scene(Renderer& /*ren*/,const ViewDimens& /*dimens*/) {}
};

} // Namespace.
#endif
