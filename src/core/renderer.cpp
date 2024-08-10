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

void Renderer::begin_texture(const Texture& texture) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture.id());
}

void Renderer::end_texture() {
  glBindTexture(GL_TEXTURE_2D,0); // Unbind.
  glDisable(GL_TEXTURE_2D);
}

void Renderer::begin_color(const Color4f& color) {
  glColor4f(color.r,color.g,color.b,color.a);
}

void Renderer::end_color() {
  glColor4f(1.0f,1.0f,1.0f,1.0f);
}

void Renderer::draw_quad(int x,int y,int width,int height) {
  Pos2f pos1 = {
    static_cast<GLfloat>(x) * dimens_.scale,
    static_cast<GLfloat>(y) * dimens_.scale
  };
  Pos2f pos2 = {
    pos1.x + (static_cast<GLfloat>(width) * dimens_.scale),
    pos1.y + (static_cast<GLfloat>(height) * dimens_.scale)
  };

  glBegin(GL_QUADS);
    glVertex2f(pos1.x,pos1.y);
    glVertex2f(pos2.x,pos1.y);
    glVertex2f(pos2.x,pos2.y);
    glVertex2f(pos1.x,pos2.y);
  glEnd();
}

void Renderer::draw_quad(const Pos4f& src,int x,int y,int width,int height) {
  Pos2f dest1 = {
    static_cast<GLfloat>(x) * dimens_.scale,
    static_cast<GLfloat>(y) * dimens_.scale
  };
  Pos2f dest2 = {
    dest1.x + (static_cast<GLfloat>(width) * dimens_.scale),
    dest1.y + (static_cast<GLfloat>(height) * dimens_.scale)
  };

  glBegin(GL_QUADS);
    glTexCoord2f(src.x1,src.y1); glVertex2f(dest1.x,dest1.y);
    glTexCoord2f(src.x2,src.y1); glVertex2f(dest2.x,dest1.y);
    glTexCoord2f(src.x2,src.y2); glVertex2f(dest2.x,dest2.y);
    glTexCoord2f(src.x1,src.y2); glVertex2f(dest1.x,dest2.y);
  glEnd();
}

void Renderer::draw_quad(const Sprite& sprite,int x,int y) {
  draw_quad(sprite,x,y,sprite.size().w,sprite.size().h);
}

void Renderer::draw_quad(const Sprite& sprite,int x,int y,int width,int height) {
  draw_quad(sprite.src(),x,y,width,height);
}

void Renderer::draw_quad(const SpriteAtlas& atlas,int index,int x,int y) {
  draw_quad(atlas,index,x,y,atlas.cell_size().w,atlas.cell_size().h);
}

void Renderer::draw_quad(const SpriteAtlas& atlas,int index,int x,int y,int width,int height) {
  const Pos4f* src = atlas.src(index);
  if(src == nullptr) { return; }

  draw_quad(*src,x,y,width,height);
}

void Renderer::draw_quad(const SpriteAtlas& atlas,int column,int row,int x,int y) {
  draw_quad(atlas,column,row,x,y,atlas.cell_size().w,atlas.cell_size().h);
}

void Renderer::draw_quad(const SpriteAtlas& atlas,int column,int row,int x,int y,int width,int height) {
  const Pos4f* src = atlas.src(column,row);
  if(src == nullptr) { return; }

  draw_quad(*src,x,y,width,height);
}

void Renderer::draw_str(const FontAtlas& font,int x,int y,const tiny_utf8::string& str) {
  draw_str(font,x,y,font.spacing(),str);
}

void Renderer::draw_str(const FontAtlas& font,int x,int y,const Size2i& spacing
    ,const tiny_utf8::string& str) {
  draw_str(font,x,y,font.cell_size().w,font.cell_size().h,spacing,str);
}

void Renderer::draw_str(const FontAtlas& font,int x,int y,int char_width,int char_height
    ,const tiny_utf8::string& str) {
  draw_str(font,x,y,char_width,char_height,font.spacing(),str);
}

void Renderer::draw_str(const FontAtlas& font,int x,int y,int char_width,int char_height
    ,const Size2i& spacing,const tiny_utf8::string& str) {
  const int x_spacing = char_width + spacing.w;
  const int y_spacing = char_height + spacing.h;
  int char_x = x;
  int char_y = y;

  for(auto c: str) {
    if(c == '\n') {
      char_x = x;
      char_y += y_spacing;
      continue;
    }

    draw_quad(font,font.char_index(c),char_x,char_y,char_width,char_height);
    char_x += x_spacing;
  }
}

void Renderer::draw_strs(const FontAtlas& font,int x,int y,const std::vector<tiny_utf8::string>& lines) {
  draw_strs(font,x,y,font.spacing(),lines);
}

void Renderer::draw_strs(const FontAtlas& font,int x,int y,const Size2i& spacing
    ,const std::vector<tiny_utf8::string>& lines) {
  draw_strs(font,x,y,font.cell_size().w,font.cell_size().h,spacing,lines);
}

void Renderer::draw_strs(const FontAtlas& font,int x,int y,int char_width,int char_height
    ,const std::vector<tiny_utf8::string>& lines) {
  draw_strs(font,x,y,char_width,char_height,font.spacing(),lines);
}

void Renderer::draw_strs(const FontAtlas& font,int x,int y,int char_width,int char_height
    ,const Size2i& spacing,const std::vector<tiny_utf8::string>& lines) {
  const int x_spacing = char_width + spacing.w;
  const int y_spacing = char_height + spacing.h;
  int char_x = x;
  int char_y = y;

  for(auto line: lines) {
    for(auto c: line) {
      draw_quad(font,font.char_index(c),char_x,char_y,char_width,char_height);
      char_x += x_spacing;
    }

    char_x = x;
    char_y += y_spacing;
  }
}

const ViewDimens& Renderer::dimens() const { return dimens_; }

Color4f& Renderer::clear_color() { return clear_color_; }

} // Namespace.
