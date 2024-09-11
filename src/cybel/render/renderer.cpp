/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "renderer.h"

namespace cybel {

const Pos4f Renderer::kDefaultSrc{0.0f,0.0f,1.0f,1.0f};

Renderer::TextureWrapper::TextureWrapper(Renderer& ren,const Texture& texture,const Pos4f& src)
    : ren(ren),texture(texture),src(src) {}

Renderer::TextureWrapper& Renderer::TextureWrapper::draw_quad(const Pos3i& pos) {
  return draw_quad(pos,texture.size());
}

Renderer::TextureWrapper& Renderer::TextureWrapper::draw_quad(const Pos3i& pos,const Size2i& size) {
  ren.draw_quad(src,pos,size);
  return *this;
}

Renderer::SpriteWrapper::SpriteWrapper(Renderer& ren,const Sprite& sprite)
    : ren(ren),sprite(sprite) {}

Renderer::SpriteWrapper& Renderer::SpriteWrapper::draw_quad(const Pos3i& pos) {
  return draw_quad(pos,sprite.size());
}

Renderer::SpriteWrapper& Renderer::SpriteWrapper::draw_quad(const Pos3i& pos,const Size2i& size) {
  ren.draw_quad(sprite.src(),pos,size);
  return *this;
}

Renderer::SpriteAtlasWrapper::SpriteAtlasWrapper(Renderer& ren,const SpriteAtlas& atlas)
    : ren(ren),atlas(atlas) {}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(int index,const Pos3i& pos) {
  return draw_quad(index,pos,atlas.cell_size());
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(int index,const Pos3i& pos
    ,const Size2i& size) {
  const Pos4f* src = atlas.src(index);

  if(src != nullptr) { ren.draw_quad(*src,pos,size); }
  return *this;
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(const Pos2i& cell,const Pos3i& pos) {
  return draw_quad(cell,pos,atlas.cell_size());
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(const Pos2i& cell,const Pos3i& pos
    ,const Size2i& size) {
  const Pos4f* src = atlas.src(cell);

  if(src != nullptr) { ren.draw_quad(*src,pos,size); }
  return *this;
}

Renderer::FontAtlasWrapper::FontAtlasWrapper(Renderer& ren,const FontAtlas& font,const Pos3i& pos
    ,const Size2i& char_size,const Size2i& spacing)
    : ren(ren),font(font),init_pos(pos),pos(pos),char_size(char_size),spacing(spacing) {}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::draw_bg(const Color4f& color,const Size2i& str_size) {
  return draw_bg(color,str_size,{});
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::draw_bg(const Color4f& color,const Size2i& str_size,const Size2i& padding) {
  ren.end_texture();
  ren.wrap_color(color,[&] {
    ren.draw_quad(
      {pos.x - padding.w,pos.y - padding.h},
      {
        (char_size.w * str_size.w) + (font.spacing().w * (str_size.w - 1)) + (padding.w << 1),
        (char_size.h * str_size.h) + (font.spacing().h * (str_size.h - 1)) + (padding.h << 1)
      }
    );
  });
  ren.begin_texture(font.texture());

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print() { return print_blanks(1); }

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print(char32_t c) {
  const Pos4f* src = font.src(font.char_index(c));

  if(src != nullptr) { ren.draw_quad(*src,pos,char_size); }
  return print();
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print(const tiny_utf8::string& str) {
  if(str.empty()) { return print(); }

  for(char32_t c: str) {
    if(c == '\n') {
      puts();
      continue;
    }

    print(c);
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

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print_blanks(int count) {
  pos.x += ((char_size.w + spacing.w) * count);
  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts() { return puts_blanks(1); }

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts(char32_t c) {
  print(c);
  return puts();
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts(const tiny_utf8::string& str) {
  if(!str.empty()) { print(str); }
  return puts();
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts(const std::vector<tiny_utf8::string>& lines) {
  for(auto& line: lines) { puts(line); }
  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts_blanks(int count) {
  pos.x = init_pos.x;
  pos.y += ((char_size.h + spacing.h) * count);

  return *this;
}

Renderer::Renderer(const Size2i& size,const Size2i& target_size,const Color4f& clear_color)
    : clear_color_(clear_color){
  // Avoid divides by 0.
  dimens_.init_size = {(size.w > 0) ? size.w : 1,(size.h > 0) ? size.h : 1};
  dimens_.size = dimens_.init_size;
  dimens_.target_size = {(target_size.w > 0) ? target_size.w : 1,(target_size.h > 0) ? target_size.h : 1};

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

  glShadeModel(GL_SMOOTH); // GL_SMOOTH, GL_FLAT

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    throw CybelError{Util::build_str("Failed to init OpenGL [",error,"]: "
        ,Util::get_gl_error(error),'.')};
  }
}

void Renderer::resize(const Size2i& size) {
  // Allow resize even if the width & height haven't changed.
  // - If decide to change this logic, need to allow force resize so can resize on init.

  // Avoid divides by 0 [e.g., in begin_3d_scene()].
  dimens_.size.w = (size.w > 0) ? size.w : 1;
  dimens_.size.h = (size.h > 0) ? size.h : 1;
  // `target_size.w/h` should never be 0 (checked in ctor).
  dimens_.scale.x = static_cast<float>(dimens_.size.w) / static_cast<float>(dimens_.target_size.w);
  dimens_.scale.y = static_cast<float>(dimens_.size.h) / static_cast<float>(dimens_.target_size.h);
  dimens_.aspect_scale = std::min(dimens_.scale.x,dimens_.scale.y);

  glViewport(0,0,dimens_.size.w,dimens_.size.h);
}

void Renderer::clear_view() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

Renderer& Renderer::begin_2d_scene() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0.0,dimens_.size.w,dimens_.size.h,0.0,-5.0,5.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return *this;
}

Renderer& Renderer::begin_3d_scene() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // With (...,0.1,100.0), it had some weird clipping on the edges for 1600x900 for some reason.
  gluPerspective(45.0,static_cast<GLdouble>(dimens_.size.w) / dimens_.size.h,0.01,5.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return *this;
}

Renderer& Renderer::begin_auto_center_scale() {
  return begin_auto_anchor_scale({0.5f,0.5f});
}

Renderer& Renderer::begin_auto_anchor_scale(const Pos2f& anchor) {
  const float w = static_cast<float>(dimens_.size.w);
  const float h = static_cast<float>(dimens_.size.h);
  const float tw = static_cast<float>(dimens_.target_size.w);
  const float th = static_cast<float>(dimens_.target_size.h);

  scale_.x = dimens_.aspect_scale;
  scale_.y = dimens_.aspect_scale;
  aspect_scale_ = dimens_.aspect_scale;
  offset_.x = (w - (tw * scale_.x)) * anchor.x;
  offset_.y = (h - (th * scale_.y)) * anchor.y;

  return *this;
}

Renderer& Renderer::begin_auto_scale() {
  scale_.x = dimens_.scale.x;
  scale_.y = dimens_.scale.y;
  aspect_scale_ = dimens_.aspect_scale;

  return *this;
}

Renderer& Renderer::end_scale() {
  scale_.x = 1.0f;
  scale_.y = 1.0f;
  aspect_scale_ = 1.0f;
  offset_.x = 0.0f;
  offset_.y = 0.0f;

  return *this;
}

Renderer& Renderer::begin_color(const Color4f& color) {
  glColor4f(color.r,color.g,color.b,color.a);
  return *this;
}

Renderer& Renderer::end_color() {
  glColor4f(1.0f,1.0f,1.0f,1.0f);
  return *this;
}

Renderer& Renderer::begin_add_blend() {
  glGetIntegerv(GL_BLEND_SRC_RGB,&blend_src_rgb_);
  glGetIntegerv(GL_BLEND_SRC_ALPHA,&blend_src_alpha_);
  glGetIntegerv(GL_BLEND_DST_RGB,&blend_dst_rgb_);
  glGetIntegerv(GL_BLEND_DST_ALPHA,&blend_dst_alpha_);

  glBlendFunc(GL_ONE,GL_ONE);

  return *this;
}

Renderer& Renderer::end_blend() {
  glBlendFuncSeparate(blend_src_rgb_,blend_dst_rgb_,blend_src_alpha_,blend_dst_alpha_);
  return *this;
}

Renderer& Renderer::begin_texture(const Texture& texture) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture.gl_id());

  return *this;
}

Renderer& Renderer::end_texture() {
  glBindTexture(GL_TEXTURE_2D,0); // Unbind.
  glDisable(GL_TEXTURE_2D);

  return *this;
}

Renderer& Renderer::wrap_color(const Color4f& color,const WrapCallback& callback) {
  begin_color(color);
  callback();
  return end_color();
}

Renderer& Renderer::wrap_rotate(const Pos3i& pos,float angle,const WrapCallback& callback) {
  const GLfloat x = offset_.x + (static_cast<GLfloat>(pos.x) * scale_.x);
  const GLfloat y = offset_.y + (static_cast<GLfloat>(pos.y) * scale_.y);
  const GLfloat z = static_cast<GLfloat>(pos.z);

  glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(angle,0.0f,0.0f,1.0f);
    glTranslatef(-x,-y,-z);
    callback();
  glPopMatrix();

  return *this;
}

Renderer& Renderer::wrap_add_blend(const WrapCallback& callback) {
  begin_add_blend();
  callback();
  return end_blend();
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

Renderer& Renderer::wrap_sprite(const Sprite& sprite,const WrapSpriteCallback& callback) {
  SpriteWrapper wrapper{*this,sprite};

  begin_texture(sprite.texture());
  callback(wrapper);
  return end_texture();
}

Renderer& Renderer::wrap_sprite_atlas(const SpriteAtlas& atlas,const WrapSpriteAtlasCallback& callback) {
  SpriteAtlasWrapper wrapper{*this,atlas};

  begin_texture(atlas.texture());
  callback(wrapper);
  return end_texture();
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,const Pos3i& pos
    ,const WrapFontAtlasCallback& callback) {
  return wrap_font_atlas(font,pos,font.cell_size(),callback);
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& char_size
    ,const WrapFontAtlasCallback& callback) {
  return wrap_font_atlas(font,pos,char_size,font.spacing(),callback);
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& char_size
    ,const Size2i& spacing,const WrapFontAtlasCallback& callback) {
  FontAtlasWrapper wrapper{*this,font,pos,char_size,spacing};

  begin_texture(font.texture());
  callback(wrapper);
  return end_texture();
}

Renderer& Renderer::draw_quad(const Pos3i& pos,const Size2i& size) {
  Pos5f dest = build_dest_pos5f(pos,size);

  glBegin(GL_QUADS);
    glVertex3f(dest.x1,dest.y1,dest.z);
    glVertex3f(dest.x2,dest.y1,dest.z);
    glVertex3f(dest.x2,dest.y2,dest.z);
    glVertex3f(dest.x1,dest.y2,dest.z);
  glEnd();

  return *this;
}

Renderer& Renderer::draw_quad(const Pos4f& src,const Pos3i& pos,const Size2i& size) {
  Pos5f dest = build_dest_pos5f(pos,size);

  glBegin(GL_QUADS);
    glTexCoord2f(src.x1,src.y1); glVertex3f(dest.x1,dest.y1,dest.z);
    glTexCoord2f(src.x2,src.y1); glVertex3f(dest.x2,dest.y1,dest.z);
    glTexCoord2f(src.x2,src.y2); glVertex3f(dest.x2,dest.y2,dest.z);
    glTexCoord2f(src.x1,src.y2); glVertex3f(dest.x1,dest.y2,dest.z);
  glEnd();

  return *this;
}

Pos5f Renderer::build_dest_pos5f(const Pos3i& pos,const Size2i& size) {
  float x1 = offset_.x + (static_cast<float>(pos.x) * scale_.x);
  float y1 = offset_.y + (static_cast<float>(pos.y) * scale_.y);
  float x2 = x1 + (static_cast<float>(size.w) * aspect_scale_);
  float y2 = y1 + (static_cast<float>(size.h) * aspect_scale_);
  float z = static_cast<float>(pos.z);

  return {x1,y1,x2,y2,z};
}

const ViewDimens& Renderer::dimens() const { return dimens_; }

Color4f& Renderer::clear_color() { return clear_color_; }

} // Namespace.
