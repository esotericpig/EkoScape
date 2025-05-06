/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "renderer.h"

#include "cybel/str/utf8/rune_range.h"
#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

Renderer::Renderer(const Size2i& size,const Size2i& target_size,const Color4f& clear_color)
  : clear_color_(clear_color) {
  // Avoid divides by 0.
  dimens_.init_size = Size2i{(size.w > 0) ? size.w : 1,(size.h > 0) ? size.h : 1};
  dimens_.size = dimens_.init_size;
  dimens_.target_size = Size2i{
    (target_size.w > 0) ? target_size.w : 1,
    (target_size.h > 0) ? target_size.h : 1
  };

  init_context();
}

void Renderer::init_context() {
  SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE,&depth_bits_);
  std::cout << "[INFO] OpenGL depth bits: " << depth_bits_ << '.' << std::endl;

  glClearColor(clear_color_.r,clear_color_.g,clear_color_.b,clear_color_.a);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_BLEND);
  begin_blend(curr_blend_mode_);

  const GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    throw CybelError{"Failed to init OpenGL [",error,"]: ",Util::get_gl_error(error),'.'};
  }
}

void Renderer::on_context_lost() {}

void Renderer::on_context_restored() {
  Util::clear_gl_errors();
  init_context();
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
}

Renderer& Renderer::begin_auto_center_scale() {
  return begin_auto_anchor_scale(Pos2f{0.5f,0.5f});
}

Renderer& Renderer::begin_auto_anchor_scale(const Pos2f& anchor) {
  const auto w = static_cast<float>(dimens_.size.w);
  const auto h = static_cast<float>(dimens_.size.h);
  const auto tw = static_cast<float>(dimens_.target_size.w);
  const auto th = static_cast<float>(dimens_.target_size.h);

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

Renderer& Renderer::end_color() { return begin_color(Color4f{1.0f}); }

Renderer& Renderer::begin_blend(const BlendMode& mode) {
  glBlendFunc(mode.src_factor,mode.dst_factor);

  return *this;
}

Renderer& Renderer::begin_add_blend() { return begin_blend(kAddBlendMode); }

Renderer& Renderer::end_blend() { return begin_blend(kDefaultBlendMode); }

Renderer& Renderer::wrap_color(const Color4f& color,const WrapCallback& callback) {
  const auto prev_color = curr_color_;

  begin_color(color);
  curr_color_ = color;

  callback();

  begin_color(prev_color);
  curr_color_ = prev_color;

  return *this;
}

Renderer& Renderer::wrap_rotate(const Pos3i& pos,float angle,const WrapCallback& callback) {
  const Pos3f scaled_pos{
    offset_.x + (static_cast<float>(pos.x) * scale_.x),
    offset_.y + (static_cast<float>(pos.y) * scale_.y),
    static_cast<float>(pos.z)
  };

  push_model_matrix();
    translate_model_matrix(scaled_pos);
    rotate_model_matrix(angle,Pos3f{0.0f,0.0f,1.0f});
    translate_model_matrix(-scaled_pos);
    update_model_matrix();
    callback();
  pop_model_matrix();

  return *this;
}

Renderer& Renderer::wrap_add_blend(const WrapCallback& callback) {
  const auto prev_mode = curr_blend_mode_;

  begin_blend(kAddBlendMode);
  curr_blend_mode_ = kAddBlendMode;

  callback();

  begin_blend(prev_mode);
  curr_blend_mode_ = prev_mode;

  return *this;
}

Renderer& Renderer::wrap_tex(const Texture& tex,const WrapTextureCallback& callback) {
  return wrap_tex(tex,kDefaultSrc,callback);
}

Renderer& Renderer::wrap_tex(const Texture& tex,const Pos4f& src,const WrapTextureCallback& callback) {
  TextureWrapper wrapper{*this,tex,src};

  return wrap_tex(tex,[&] { callback(wrapper); });
}

Renderer& Renderer::wrap_tex(const Texture& tex,const WrapCallback& callback) {
  const auto* prev_tex = curr_tex_;

  begin_tex(tex);
  curr_tex_ = &tex;

  callback();

  if(prev_tex != nullptr) {
    begin_tex(*prev_tex);
  } else {
    end_tex();
  }
  curr_tex_ = prev_tex;

  return *this;
}

Renderer& Renderer::wrap_sprite(const Sprite& sprite,const WrapSpriteCallback& callback) {
  SpriteWrapper wrapper{*this,sprite};

  return wrap_tex(sprite.tex(),[&] { callback(wrapper); });
}

Renderer& Renderer::wrap_sprite_atlas(const SpriteAtlas& atlas,const WrapSpriteAtlasCallback& callback) {
  SpriteAtlasWrapper wrapper{*this,atlas};

  return wrap_tex(atlas.tex(),[&] { callback(wrapper); });
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,
                                    const WrapFontAtlasCallback& callback) {
  return wrap_font_atlas(font,pos,font.cell_size(),callback);
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& rune_size,
                                    const WrapFontAtlasCallback& callback) {
  return wrap_font_atlas(font,pos,rune_size,font.spacing(),callback);
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& rune_size,
                                    const Size2i& spacing,const WrapFontAtlasCallback& callback) {
  FontAtlasWrapper wrapper{*this,font,pos,rune_size,spacing};

  return wrap_tex(font.tex(),[&] { callback(wrapper); });
}

Pos5f Renderer::build_dest_pos5f(const Pos3i& pos,const Size2i& size) const {
  const auto x1 = offset_.x + (static_cast<float>(pos.x) * scale_.x);
  const auto y1 = offset_.y + (static_cast<float>(pos.y) * scale_.y);
  const auto x2 = x1 + (static_cast<float>(size.w) * aspect_scale_);
  const auto y2 = y1 + (static_cast<float>(size.h) * aspect_scale_);
  const auto z = static_cast<float>(pos.z);

  return Pos5f{x1,y1,x2,y2,z};
}

const ViewDimens& Renderer::dimens() const { return dimens_; }

Color4f& Renderer::clear_color() { return clear_color_; }

Renderer::TextureWrapper::TextureWrapper(Renderer& ren,const Texture& tex,const Pos4f& src)
  : ren(ren),tex(tex),src(src) {}

Renderer::TextureWrapper& Renderer::TextureWrapper::draw_quad(const Pos3i& pos) {
  return draw_quad(pos,tex.size());
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

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(std::size_t index,const Pos3i& pos) {
  return draw_quad(index,pos,atlas.cell_size());
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(std::size_t index,const Pos3i& pos,
                                                                      const Size2i& size) {
  const Pos4f* src = atlas.src(index);

  if(src != nullptr) { ren.draw_quad(*src,pos,size); }

  return *this;
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(const Pos2i& cell,const Pos3i& pos) {
  return draw_quad(cell,pos,atlas.cell_size());
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(const Pos2i& cell,const Pos3i& pos,
                                                                      const Size2i& size) {
  const Pos4f* src = atlas.src(cell);

  if(src != nullptr) { ren.draw_quad(*src,pos,size); }

  return *this;
}

Renderer::FontAtlasWrapper::FontAtlasWrapper(Renderer& ren,const FontAtlas& font,const Pos3i& pos,
                                             const Size2i& rune_size,const Size2i& spacing)
  : ren(ren),font(font),init_pos(pos),pos(pos),rune_size(rune_size),spacing(spacing) {}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::draw_bg(const Color4f& color,const Size2i& str_size) {
  ren.end_tex(); // Temporarily unbind the font texture.
  ren.wrap_color(color,[&] {
    ren.draw_quad(Pos3i{pos.x - bg_padding_.w,pos.y - bg_padding_.h,pos.z},calc_total_size(str_size));
  });
  ren.begin_tex(font.tex()); // Bind back the font texture.

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print() { return print_blanks(1); }

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print(char32_t rune) {
  const Pos4f* src = font.src(font.rune_index(rune));

  if(src != nullptr) { ren.draw_quad(*src,pos,rune_size); }

  return print();
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print(std::string_view str) {
  if(str.empty()) { return *this; }

  for(const auto rune : utf8::RuneRange{str}) {
    if(rune == '\n') {
      puts();
      continue;
    }

    print(rune);
  }

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print_blanks(int count) {
  pos.x += ((rune_size.w + spacing.w) * count);

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts() { return puts_blanks(1); }

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts(char32_t rune) {
  return print(rune).puts();
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts(std::string_view str) {
  return print(str).puts();
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts_blanks(int count) {
  pos.x = init_pos.x;
  pos.y += ((rune_size.h + spacing.h) * count);

  return *this;
}

Size2i Renderer::FontAtlasWrapper::calc_total_size(const Size2i& str_size) const {
  return Size2i{
    (str_size.w * rune_size.w) + ((str_size.w - 1) * spacing.w) + (bg_padding_.w << 1),
    (str_size.h * rune_size.h) + ((str_size.h - 1) * spacing.h) + (bg_padding_.h << 1)
  };
}

void Renderer::FontAtlasWrapper::set_bg_padding(const Size2i& padding) {
  // Remove the previous padding (if any) and add the new padding.
  init_pos.x = (init_pos.x - bg_padding_.w) + padding.w;
  init_pos.y = (init_pos.y - bg_padding_.h) + padding.h;
  pos.x = (pos.x - bg_padding_.w) + padding.w;
  pos.y = (pos.y - bg_padding_.h) + padding.h;

  bg_padding_ = padding;
}

} // namespace cybel
