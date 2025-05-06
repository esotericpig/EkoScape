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
  init();
}

void RendererGl::init() {
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

void RendererGl::on_context_restored() {
  Renderer::on_context_restored();
  init();
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
  glBindTexture(GL_TEXTURE_2D,tex.handle());

  return *this;
}

Renderer& RendererGl::end_tex() {
  glBindTexture(GL_TEXTURE_2D,0); // Unbind.
  // glDisable(GL_TEXTURE_2D);

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

void RendererGl::translate_model_matrix(const Pos3f& pos) {
  glTranslatef(pos.x,pos.y,pos.z);
}

void RendererGl::rotate_model_matrix(float angle,const Pos3f& axis) {
  glRotatef(angle,axis.x,axis.y,axis.z);
}

void RendererGl::update_model_matrix() {}

void RendererGl::push_model_matrix() {
  glPushMatrix();
}

void RendererGl::pop_model_matrix() {
  glPopMatrix();
}

GLuint RendererGl::gen_quad_buffers(int count) {
  return glGenLists(count);
}

void RendererGl::delete_quad_buffers(GLuint id,int count) {
  glDeleteLists(id,count);
}

void RendererGl::compile_quad_buffer(GLuint id,int index,const QuadBufferData& data) {
  glNewList(id + static_cast<GLuint>(index),GL_COMPILE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,data.tex_handle);

    glBegin(GL_QUADS);
      glNormal3f(data.normal.x,data.normal.y,data.normal.z);
      glTexCoord2f(0.0f,0.0f);
      glVertex3f(data.vertices[0].x,data.vertices[0].y,data.vertices[0].z);
      glTexCoord2f(1.0f,0.0f);
      glVertex3f(data.vertices[1].x,data.vertices[1].y,data.vertices[1].z);
      glTexCoord2f(1.0f,1.0f);
      glVertex3f(data.vertices[2].x,data.vertices[2].y,data.vertices[2].z);
      glTexCoord2f(0.0f,1.0f);
      glVertex3f(data.vertices[3].x,data.vertices[3].y,data.vertices[3].z);
    glEnd();
  glEndList();
}

void RendererGl::draw_quad_buffer(GLuint id,int index) {
  glCallList(id + static_cast<GLuint>(index));
}

} // namespace cybel
#endif // CYBEL_RENDERER_GL
