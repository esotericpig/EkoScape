/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "renderer.h"

namespace ekoscape {

const Pos4f Renderer::kDefaultSrc{0.0f,0.0f,1.0f,1.0f};

Renderer::TextureWrapper::TextureWrapper(Renderer& ren,const Texture& texture,const Pos4f& src)
    : ren(ren),texture(texture),src(src) {}

Renderer::TextureWrapper& Renderer::TextureWrapper::draw_quad(int x,int y) {
  draw_quad(x,y,texture.width(),texture.height());
  return *this;
}

Renderer::TextureWrapper& Renderer::TextureWrapper::draw_quad(int x,int y,int width,int height) {
  ren.draw_quad(src,x,y,width,height);
  return *this;
}

Renderer::SpriteAtlasWrapper::SpriteAtlasWrapper(Renderer& ren,const SpriteAtlas& atlas)
    : ren(ren),atlas(atlas) {}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(int index,int x,int y) {
  draw_quad(index,x,y,atlas.cell_size().w,atlas.cell_size().h);
  return *this;
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(int index,int x,int y,int width
    ,int height) {
  const Pos4f* src = atlas.src(index);
  if(src == nullptr) { return *this; }

  ren.draw_quad(*src,x,y,width,height);
  return *this;
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(int column,int row,int x,int y) {
  return draw_quad(column,row,x,y,atlas.cell_size().w,atlas.cell_size().h);
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(int column,int row,int x,int y
    ,int width,int height) {
  const Pos4f* src = atlas.src(column,row);
  if(src == nullptr) { return *this; }

  ren.draw_quad(*src,x,y,width,height);
  return *this;
}

Renderer::FontAtlasWrapper::FontAtlasWrapper(Renderer& ren,const FontAtlas& font,const Pos2i& pos
    ,const Size2i& char_size,const Size2i& spacing)
    : ren(ren),font(font),init_pos(pos),pos(pos),char_size(char_size),spacing(spacing) {}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print() {
  pos.x += (char_size.w + spacing.w);
  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print(const tiny_utf8::string& str) {
  if(str.empty()) { return print(); }

  const int x_spacing = char_size.w + spacing.w;
  const int y_spacing = char_size.h + spacing.h;

  for(auto c: str) {
    if(c == '\n') {
      pos.x = init_pos.x;
      pos.y += y_spacing;
      continue;
    }

    const Pos4f* src = font.src(font.char_index(c));
    if(src != nullptr) { ren.draw_quad(*src,pos.x,pos.y,char_size.w,char_size.h); }

    pos.x += x_spacing;
  }

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print(const std::vector<tiny_utf8::string>& strs) {
  for(auto& str: strs) {
    print(str);
    print(); // Space.
  }

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts() {
  pos.x = init_pos.x;
  pos.y += (char_size.h + spacing.h);

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts(const tiny_utf8::string& str) {
  if(!str.empty()) { print(str); }
  return puts();
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts(const std::vector<tiny_utf8::string>& lines) {
  for(auto& line: lines) { puts(line); }
  return *this;
}

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

void Renderer::clear_view() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
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

Renderer& Renderer::wrap_scale(const WrapCallback& callback) {
  return wrap_scale(1.0f,callback);
}

Renderer& Renderer::wrap_scale(float scale,const WrapCallback& callback) {
  const float orig_scale = dimens_.scale;

  dimens_.scale = scale;
  callback();
  dimens_.scale = orig_scale;

  return *this;
}

Renderer& Renderer::wrap_color(const Color4f& color,const WrapCallback& callback) {
  glColor4f(color.r,color.g,color.b,color.a);
  callback();
  glColor4f(1.0f,1.0f,1.0f,1.0f);

  return *this;
}

Renderer& Renderer::wrap_texture(const Texture& texture,const WrapTextureCallback& callback) {
  return wrap_texture(texture,kDefaultSrc,callback);
}

Renderer& Renderer::wrap_texture(const Texture& texture,const Pos4f& src
    ,const WrapTextureCallback& callback) {
  TextureWrapper wrapper{*this,texture,src};

  begin_texture(texture);
  callback(wrapper);

  return end_texture();
}

Renderer& Renderer::wrap_texture(const TextureBag& tex_bag,const WrapTextureCallback& callback) {
  return wrap_texture(tex_bag.texture(),callback);
}

Renderer& Renderer::wrap_texture(const TextureBag& tex_bag,const Pos4f& src
    ,const WrapTextureCallback& callback) {
  return wrap_texture(tex_bag.texture(),src,callback);
}

Renderer& Renderer::wrap_sprite_atlas(const SpriteAtlas& atlas,const WrapSpriteAtlasCallback& callback) {
  SpriteAtlasWrapper wrapper{*this,atlas};

  begin_texture(atlas.texture());
  callback(wrapper);

  return end_texture();
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,int x,int y
    ,const WrapFontAtlasCallback& callback) {
  return wrap_font_atlas(font,x,y,font.cell_size().w,font.cell_size().h,font.spacing(),callback);
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,int x,int y,int char_width,int char_height
    ,const WrapFontAtlasCallback& callback) {
  return wrap_font_atlas(font,x,y,char_width,char_height,font.spacing(),callback);
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,int x,int y,int char_width,int char_height
    ,const Size2i& spacing,const WrapFontAtlasCallback& callback) {
  FontAtlasWrapper wrapper{*this,font,{x,y},{char_width,char_height},spacing};

  begin_texture(font.texture());
  callback(wrapper);

  return end_texture();
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,int x,int y,const Size2i& spacing
    ,const WrapFontAtlasCallback& callback) {
  return wrap_font_atlas(font,x,y,font.cell_size().w,font.cell_size().h,spacing,callback);
}

Renderer& Renderer::begin_texture(const Texture& texture) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture.id());

  return *this;
}

Renderer& Renderer::end_texture() {
  glBindTexture(GL_TEXTURE_2D,0); // Unbind.
  glDisable(GL_TEXTURE_2D);

  return *this;
}

Renderer& Renderer::draw_quad(int x,int y,int width,int height) {
  Pos4f dest = build_dest_pos4f(x,y,width,height);

  glBegin(GL_QUADS);
    glVertex2f(dest.x1,dest.y1);
    glVertex2f(dest.x2,dest.y1);
    glVertex2f(dest.x2,dest.y2);
    glVertex2f(dest.x1,dest.y2);
  glEnd();

  return *this;
}

Renderer& Renderer::draw_quad(const Pos4f& src,int x,int y,int width,int height) {
  Pos4f dest = build_dest_pos4f(x,y,width,height);

  glBegin(GL_QUADS);
    glTexCoord2f(src.x1,src.y1); glVertex2f(dest.x1,dest.y1);
    glTexCoord2f(src.x2,src.y1); glVertex2f(dest.x2,dest.y1);
    glTexCoord2f(src.x2,src.y2); glVertex2f(dest.x2,dest.y2);
    glTexCoord2f(src.x1,src.y2); glVertex2f(dest.x1,dest.y2);
  glEnd();

  return *this;
}

Pos4f Renderer::build_dest_pos4f(int x,int y,int width,int height) {
  float x1 = static_cast<float>(x) * dimens_.scale;
  float y1 = static_cast<float>(y) * dimens_.scale;
  float x2 = x1 + (static_cast<float>(width) * dimens_.scale);
  float y2 = y1 + (static_cast<float>(height) * dimens_.scale);

  return {x1,y1,x2,y2};
}

const ViewDimens& Renderer::dimens() const { return dimens_; }

Color4f& Renderer::clear_color() { return clear_color_; }

} // Namespace.
