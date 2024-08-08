/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_RENDERER_H_
#define EKOSCAPE_CORE_RENDERER_H_

#include "common.h"

#include "ekoscape_error.h"
#include "render_data.h"
#include "util.h"

namespace ekoscape {

class Renderer {
public:
  Renderer(const Size2i& size,const Size2i& target_size,const Color4f& clear_color);

  void resize(const Size2i& size);

  void begin_2d_scene();
  void begin_3d_scene();
  void clear_view();

  const ViewDimens& dimens() const;
  Color4f& clear_color();

private:
  ViewDimens dimens_{};
  Color4f clear_color_{};

  void init_gl();
};

} // Namespace.
#endif
