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
   * Return this in update_scene_logic() to indicate no scene change (default).
   * Do not use this for your own scene types, else they'll be ignored.
   */
  static inline const int kNilType = 0;

  virtual ~Scene() noexcept = default;

  virtual void init_scene([[maybe_unused]] const ViewDimens& dimens) {}

  /**
   * If you know that all of your scenes will only either be in 2D or 3D, then you can call
   *     begin_2d_scene()/begin_3d_scene() in resize_scene(), instead of in the main loop with draw_scene().
   * This also applies to: begin_*center(), begin_*scale(), begin_*offset(), etc.
   */
  virtual void resize_scene([[maybe_unused]] Renderer& ren,[[maybe_unused]] const ViewDimens& dimens) {}

  /**
   * Called when this scene is no longer the current scene (scene changed).
   * Should not delete objects in here (i.e., not a destructor).
   */
  virtual void on_scene_exit() {}

  /**
   * On WebGL context restored, reload textures, re-init OpenGL states, etc.
   */
  virtual void on_scene_context_restored() {}

  virtual void on_scene_input_event([[maybe_unused]] int input_id,
                                    [[maybe_unused]] const ViewDimens& dimens) {}
  virtual void handle_scene_input_states([[maybe_unused]] const std::vector<bool>& states,
                                         [[maybe_unused]] const ViewDimens& dimens) {}

  virtual int update_scene_logic([[maybe_unused]] const FrameStep& step,
                                 [[maybe_unused]] const ViewDimens& dimens) { return kNilType; }
  virtual void draw_scene([[maybe_unused]] Renderer& ren,[[maybe_unused]] const ViewDimens& dimens) {}
};

} // namespace cybel
#endif
