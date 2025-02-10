/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "renderer_gles.h"

#if defined(CYBEL_RENDER_GLES)

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

RendererGles::RendererGles(const Size2i& size,const Size2i& target_size,const Color4f& clear_color)
  : Renderer(size,target_size,clear_color) {
  glDepthRangef(0.0f,1.0f);
  glClearDepthf(1.0f);

  const GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    throw CybelError{"Failed to init OpenGL ES renderer [",error,"]: ",Util::get_gl_error(error),'.'};
  }
}

Renderer& RendererGles::begin_2d_scene() {
  return *this;
}

Renderer& RendererGles::begin_3d_scene() {
  return *this;
}

Renderer& RendererGles::begin_color(const Color4f& /*color*/) {
  return *this;
}

Renderer& RendererGles::begin_tex(const Texture& /*tex*/) {
  return *this;
}

Renderer& RendererGles::end_tex() {
  return *this;
}

Renderer& RendererGles::wrap_rotate(const Pos3i& /*pos*/,float /*angle*/,const WrapCallback& /*callback*/) {
  // const auto x = offset_.x + (static_cast<GLfloat>(pos.x) * scale_.x);
  // const auto y = offset_.y + (static_cast<GLfloat>(pos.y) * scale_.y);
  // const auto z = static_cast<GLfloat>(pos.z);

  return *this;
}

Renderer& RendererGles::draw_quad(const Pos3i& /*pos*/,const Size2i& /*size*/) {
  // const auto [x1,y1,x2,y2,z] = build_dest_pos5f(pos,size);

  return *this;
}

Renderer& RendererGles::draw_quad(const Pos4f& /*src*/,const Pos3i& /*pos*/,const Size2i& /*size*/) {
  // const auto [x1,y1,x2,y2,z] = build_dest_pos5f(pos,size);

  return *this;
}

} // Namespace.
#endif // CYBEL_RENDER_GLES.
