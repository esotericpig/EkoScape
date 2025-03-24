/*
 * This file is part of EkoScape.
 * Copyright (c) 2024-2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_RENDERER_GL_H_
#define CYBEL_GFX_RENDERER_GL_H_

#include "cybel/common.h"

#if defined(CYBEL_RENDERER_GL)

#include "cybel/gfx/renderer.h"

namespace cybel {

class RendererGl : public Renderer {
public:
  explicit RendererGl(const Size2i& size,const Size2i& target_size,const Color4f& clear_color);

  void on_context_restored() override;

  Renderer& begin_2d_scene() override;
  Renderer& begin_3d_scene() override;

  Renderer& begin_color(const Color4f& color) override;

  Renderer& begin_tex(const Texture& tex) override;
  Renderer& end_tex() override;

  Renderer& draw_quad(const Pos3i& pos,const Size2i& size) override;
  Renderer& draw_quad(const Pos4f& src,const Pos3i& pos,const Size2i& size) override;

  void translate_model_matrix(const Pos3f& pos) override;
  void rotate_model_matrix(float angle,const Pos3f& axis) override;
  void update_model_matrix() override;
  void push_model_matrix() override;
  void pop_model_matrix() override;

  GLuint gen_quad_buffers(int count) override;
  void delete_quad_buffers(GLuint id,int count) override;
  void compile_quad_buffer(GLuint id,int index,const QuadBufferData& data) override;
  void draw_quad_buffer(GLuint id,int index) override;

private:
  void init();
};

} // namespace cybel
#endif // CYBEL_RENDERER_GL
#endif
