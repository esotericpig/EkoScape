/*
 * This file is part of EkoScape.
 * Copyright (c) 2024-2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_RENDERER_GL_H_
#define CYBEL_GFX_RENDERER_GL_H_

#include "cybel/common.h"

#include "cybel/gfx/renderer.h"

namespace cybel {

class RendererGl : public Renderer {
public:
  explicit RendererGl(const Size2i& size,const Size2i& target_size,const Color4f& clear_color);

  Renderer& begin_2d_scene() override;
  Renderer& begin_3d_scene() override;

  Renderer& begin_color(const Color4f& color) override;

  Renderer& begin_tex(const Texture& tex) override;
  Renderer& end_tex() override;

  Renderer& wrap_rotate(const Pos3i& pos,float angle,const WrapCallback& callback) override;

  Renderer& draw_quad(const Pos3i& pos,const Size2i& size) override;
  Renderer& draw_quad(const Pos4f& src,const Pos3i& pos,const Size2i& size) override;
};

} // Namespace.
#endif
