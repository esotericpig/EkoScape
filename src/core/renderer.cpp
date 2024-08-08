/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "renderer.h"

namespace ekoscape {

Renderer::Renderer(const Size2i& size,const Size2i& target_size,const Color4f& clear_color)
    : clear_color_(clear_color){
  // Avoid divides by 0.
  dimens_.init_size = {(size.w > 0) ? size.w : 1,(size.h > 0) ? size.h : 1};
  dimens_.size = dimens_.init_size;
  dimens_.target_size = {(target_size.w > 0) ? target_size.w : 1,(target_size.h > 0) ? target_size.h : 1};
  dimens_.scale = 1.0f;

  init_gl();
}

void Renderer::init_gl() {
  glClearColor(clear_color_.r,clear_color_.g,clear_color_.b,clear_color_.a);
  glClearDepth(1.0);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_TEXTURE_2D);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

  glShadeModel(GL_SMOOTH);
  //glShadeModel(GL_FLAT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    throw EkoScapeError{Util::build_string("Failed to init OpenGL [",error,"]: "
        ,Util::get_gl_error(error),'.')};
  }
}

void Renderer::resize(const Size2i& size) {
  // Allow resize even if the width & height haven't changed.
  // - If decide to change this logic, need to allow force resize so can resize on init.

  // Avoid divides by 0 [e.g., begin_3d_scene()].
  dimens_.size.w = (size.w > 0) ? size.w : 1;
  dimens_.size.h = (size.h > 0) ? size.h : 1;
  dimens_.scale = std::min(
    static_cast<float>(dimens_.size.w) / static_cast<float>(dimens_.target_size.w)
    ,static_cast<float>(dimens_.size.h) / static_cast<float>(dimens_.target_size.h)
  );

  glViewport(0,0,dimens_.size.w,dimens_.size.h);
}

void Renderer::begin_2d_scene() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0.0,dimens_.size.w,dimens_.size.h,0.0,-1.0,1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void Renderer::begin_3d_scene() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // With (...,0.1,100.0), it had some weird clipping on the edges for 1600x900 for some reason.
  gluPerspective(45.0,static_cast<GLdouble>(dimens_.size.w) / dimens_.size.h,0.01,5.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void Renderer::clear_view() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

const ViewDimens& Renderer::dimens() const { return dimens_; }

Color4f& Renderer::clear_color() { return clear_color_; }

} // Namespace.
