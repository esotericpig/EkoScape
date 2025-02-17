/*
 * This file is part of EkoScape.
 * Copyright (c) 2024-2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "renderer_gl.h"

#if defined(CYBEL_RENDERER_GL)

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

RendererGl::RendererGl(const Size2i& size,const Size2i& target_size,const Color4f& clear_color)
  : Renderer(size,target_size,clear_color) {
  glClearDepth(1.0);

  glEnable(GL_TEXTURE_2D);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  glShadeModel(GL_SMOOTH); // GL_SMOOTH, GL_FLAT.

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  const auto error = glGetError();

  if(error != GL_NO_ERROR) {
    throw CybelError{"Failed to init OpenGL renderer: ",Util::get_gl_error(error),'.'};
  }
}

Renderer& RendererGl::begin_2d_scene() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0.0,dimens_.size.w,dimens_.size.h,0.0,-5.0,5.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return *this;
}

Renderer& RendererGl::begin_3d_scene() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // With (...,0.1,100.0), it had some weird clipping on the edges for 1600x900 for some reason.
  gluPerspective(45.0,static_cast<GLdouble>(dimens_.size.w) / dimens_.size.h,0.01,5.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return *this;
}

Renderer& RendererGl::begin_color(const Color4f& color) {
  glColor4f(color.r,color.g,color.b,color.a);

  return *this;
}

Renderer& RendererGl::begin_tex(const Texture& tex) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,tex.gl_id());

  return *this;
}

Renderer& RendererGl::end_tex() {
  glBindTexture(GL_TEXTURE_2D,0); // Unbind.
  // glDisable(GL_TEXTURE_2D);

  return *this;
}

Renderer& RendererGl::wrap_rotate(const Pos3i& pos,float angle,const WrapCallback& callback) {
  const auto x = offset_.x + (static_cast<GLfloat>(pos.x) * scale_.x);
  const auto y = offset_.y + (static_cast<GLfloat>(pos.y) * scale_.y);
  const auto z = static_cast<GLfloat>(pos.z);

  glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(angle,0.0f,0.0f,1.0f);
    glTranslatef(-x,-y,-z);
    callback();
  glPopMatrix();

  return *this;
}

Renderer& RendererGl::draw_quad(const Pos3i& pos,const Size2i& size) {
  const auto [x1,y1,x2,y2,z] = build_dest_pos5f(pos,size);

  glBegin(GL_QUADS);
    glVertex3f(x1,y1,z);
    glVertex3f(x2,y1,z);
    glVertex3f(x2,y2,z);
    glVertex3f(x1,y2,z);
  glEnd();

  return *this;
}

Renderer& RendererGl::draw_quad(const Pos4f& src,const Pos3i& pos,const Size2i& size) {
  const auto [x1,y1,x2,y2,z] = build_dest_pos5f(pos,size);

  glBegin(GL_QUADS);
    glTexCoord2f(src.x1,src.y1); glVertex3f(x1,y1,z);
    glTexCoord2f(src.x2,src.y1); glVertex3f(x2,y1,z);
    glTexCoord2f(src.x2,src.y2); glVertex3f(x2,y2,z);
    glTexCoord2f(src.x1,src.y2); glVertex3f(x1,y2,z);
  glEnd();

  return *this;
}

} // Namespace.
#endif // CYBEL_RENDERER_GL.
